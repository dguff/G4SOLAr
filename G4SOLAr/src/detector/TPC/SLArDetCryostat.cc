/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArDetCryostat.cc
 * @created     Wed Mar 15, 2023 11:57:41 CET
 */
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4PVParameterised.hh"

#include "detector/TPC/SLArDetCryostat.hh"
#include "detector/SLArPlaneParameterisation.hpp"

SLArCryostatLayer::SLArCryostatLayer() : 
  fName("CryostatLayer"), 
  fHalfSizeX(1.), fHalfSizeY(1.), fHalfSizeZ(1.), fThickness(1.), fImportance(1),
  fMaterialName("MATERIAL_NAME"), fMaterial(nullptr), fModule(nullptr)
{}

SLArCryostatLayer::SLArCryostatLayer(
    G4String   model_name, 
    G4double*  halfSize,  
    G4double   thickness,
    G4String   material_name, 
    G4int      importance)
  : fMaterial(nullptr), fModule(nullptr)
{

  fName      = model_name;
  fHalfSizeX =  halfSize[0];
  fHalfSizeY =  halfSize[1]; 
  fHalfSizeZ =  halfSize[2]; 
  fThickness =  thickness;
  fImportance=  importance;

  fMaterialName = material_name;
}



SLArDetCryostat::SLArDetCryostat() : 
  fMatWorld(nullptr), fMatWaffle(nullptr), fWaffleUnit(nullptr), fBuildSupport(false)
{
    
}

SLArDetCryostat::~SLArDetCryostat()
{
}


void SLArDetCryostat::BuildCryostatStructure(const rapidjson::Value& jcryo) {
  assert(jcryo.HasMember("Cryostat_structure")); 
  assert(jcryo["Cryostat_structure"].IsArray()); 

  const auto jlayers = jcryo["Cryostat_structure"].GetArray(); 
  printf("SLArDetCryostat::BuildCryostatStructure\n");

  G4double tgtZ = fGeoInfo->GetGeoPar("target_z");
  G4double tgtY = fGeoInfo->GetGeoPar("target_y");
  G4double tgtX = fGeoInfo->GetGeoPar("target_x");

  G4double cryostat_tk = 0.; 

  // compute total thickness
  for (const auto &layer : jlayers) {
    if (layer.HasMember("thickness")) 
      cryostat_tk += SLArGeoInfo::ParseJsonVal(layer["thickness"]); 
  }

  G4double cryostat_waffle_tk = cryostat_tk; 

  if (jcryo.HasMember("Cryostat_support")) {
    fBuildSupport = true; 
    G4double support_wd = 0.; 
    const auto jsupport = jcryo["Cryostat_support"].GetObj(); 
    support_wd += SLArGeoInfo::ParseJsonVal(jsupport["steel_major_width"]); 
    printf("Adding waffle major width = %.2f mm\n", support_wd);
    cryostat_waffle_tk += support_wd; 

    fGeoInfo->RegisterGeoPar("waffle_major_width", 
        SLArGeoInfo::ParseJsonVal(jsupport["steel_major_width"])); 
    fGeoInfo->RegisterGeoPar("waffle_minor_width", 
        SLArGeoInfo::ParseJsonVal(jsupport["steel_minor_width"])); 
    fGeoInfo->RegisterGeoPar("steel_thickness", 
        SLArGeoInfo::ParseJsonVal(jsupport["steel_thickness"])); 
    fGeoInfo->RegisterGeoPar("waffle_spacing", 
        SLArGeoInfo::ParseJsonVal(jsupport["main_spacing"])); 

  }

  fGeoInfo->RegisterGeoPar("cryostat_tk", cryostat_tk); 
  fGeoInfo->RegisterGeoPar("cryostat_total_tk", cryostat_waffle_tk); 

  G4double halfSize[3] = {tgtX*0.5 + cryostat_tk, 
                          tgtY*0.5 + cryostat_tk, 
                          tgtZ*0.5 + cryostat_tk}; 
  for (const auto &layer : jlayers) {
    if (layer.HasMember("thickness")) {
      G4double tk_ = SLArGeoInfo::ParseJsonVal(layer["thickness"]);
      if (tk_ == 0.) continue;

      assert(layer.HasMember("id")); 
      assert(layer.HasMember("name")); 
      assert(layer.HasMember("material")); 
      for (size_t k=0; k<3; k++) halfSize[k] -= tk_;

      G4int importance = 1; 
      if (layer.HasMember("importance")) {
        importance = layer["importance"].GetInt(); 
      }
      
      SLArCryostatLayer* ll = new SLArCryostatLayer(
          layer["name"].GetString(), halfSize, 
          SLArGeoInfo::ParseJsonVal(layer["thickness"]), 
          layer["material"].GetString(),importance);

      fCryostatStructure.insert(std::make_pair(layer["id"].GetInt(), ll)); 
    }
  }

  printf("SLArDetCryostat::BuildCryostatStructure: Cryostat layered structure built\n");
  for (const auto& l : fCryostatStructure) {
    printf("%i: %s %g mm %s\n", 
        l.first, l.second->fName.c_str(), l.second->fThickness, 
        l.second->fMaterialName.c_str());
  }
  return; 
}

void SLArDetCryostat::BuildSupportStructureUnit() {
  const G4double spacing     = fGeoInfo->GetGeoPar("waffle_spacing"); 
  const G4double major_width = fGeoInfo->GetGeoPar("waffle_major_width"); 
  const G4double minor_width = fGeoInfo->GetGeoPar("waffle_minor_width"); 
  const G4double majorT_width = 0.20*major_width; 
  const G4double minorT_width = 0.20*minor_width; 
  const G4double tk = fGeoInfo->GetGeoPar("steel_thickness"); 
  const G4double trnv_width  = major_width-2*tk; 

  fWaffleUnit = new SLArBaseDetModule(); 
  fWaffleUnit->SetSolidVolume( new G4Box("waffle_unit_sv", 
      0.5*(spacing), 0.5*major_width, 0.5*(spacing) ) ); 
  fWaffleUnit->SetLogicVolume( new G4LogicalVolume( 
        fWaffleUnit->GetModSV(), fMatWorld->GetMaterial(), "waffle_unit_lv"));
  fWaffleUnit->GetModLV()->SetVisAttributes( G4VisAttributes(false) ); 

  // build waffle unit steel structure
  // Main frame (along the z axis)
  auto waffle_main_bar = new G4Box("waffle_main_sv", 0.25*tk, 0.5*major_width, 0.5*spacing); 
  auto waffle_main_T  = new G4Box("waffle_main_T", 0.5*majorT_width, 0.5*tk, 0.5*spacing); 
  auto waffle_main_sv = new G4UnionSolid(
      "waffle_main_sv_tmp", waffle_main_bar, waffle_main_T, 
      0, G4ThreeVector(+0.5*(majorT_width + 0.5*tk), 0.5*(major_width - tk), 0)); 
  waffle_main_sv = new G4UnionSolid(
      "waffle_main_sv", waffle_main_sv, waffle_main_T, 
      nullptr, 
      G4ThreeVector(+0.5*(majorT_width + 0.5*tk), -0.5*(major_width - tk), 0)); 

  auto waffle_main_lv = new G4LogicalVolume(
      waffle_main_sv, fMatWaffle->GetMaterial(), "waffle_main_lv"); 
  waffle_main_lv->SetVisAttributes( new G4VisAttributes( G4Colour(1, 0, 0) ) ); 
  new G4PVPlacement(0, 
      G4ThreeVector(-0.5*(spacing - 0.5*tk), 0, 0), 
      waffle_main_lv, "waffle_main_pv0", fWaffleUnit->GetModLV(), 0, 1); 
  auto rot = new G4RotationMatrix(0, 0, 0); 
  rot->rotateY(CLHEP::pi); 
  new G4PVPlacement(rot, 
      G4ThreeVector(+0.5*(spacing - 0.5*tk), 0, 0), 
      waffle_main_lv, "waffle_main_pv1", fWaffleUnit->GetModLV(), 0, 2); 
  
  // Transverse frame (along the x-axis)
  auto waffle_trnv_bar = new G4Box(
      "waffle_trnv_sv", 0.5*(spacing-tk), 0.5*trnv_width, 0.25*tk); 
  auto waffle_trnv_T  = new G4Box(
      "waffle_trnv_T", 0.5*(spacing - tk -2*majorT_width), 0.5*tk, 0.5*majorT_width); 
  auto waffle_trnv_sv = new G4UnionSolid(
      "waffle_trnv_sv_tmp", waffle_trnv_bar, waffle_trnv_T, 
      0, G4ThreeVector(0, 0.5*(trnv_width+tk), 0.5*(majorT_width - 0.5*tk) )); 
  waffle_trnv_sv = new G4UnionSolid(
      "waffle_trnv_sv", waffle_trnv_sv, waffle_trnv_T, 
      nullptr, 
      G4ThreeVector(0.0, -0.5*(major_width - tk), 0.5*(majorT_width + 0.5*tk) )); 
  auto waffle_trnv_lv = new G4LogicalVolume(
      waffle_trnv_sv, fMatWaffle->GetMaterial(), "waffle_trnv_lv"); 
  waffle_trnv_lv->SetVisAttributes( new G4VisAttributes( G4Colour(1, 0, 0) ) ); 
  new G4PVPlacement(0, 
      G4ThreeVector(0, 0, -0.5*(spacing - 0.5*tk)), 
      waffle_trnv_lv, "waffle_trnv_pv0", fWaffleUnit->GetModLV(), 0, 3); 
  new G4PVPlacement(rot, 
      G4ThreeVector(0, 0, +0.5*(spacing - 0.5*tk)), 
      waffle_trnv_lv, "waffle_trnv_pv1", fWaffleUnit->GetModLV(), 0, 4); 
  
  // Minor bar (along z-axis) 
  auto waffle_minor_bar = new G4Box(
      "waffle_minor_bar", 0.5*tk, 0.5*minor_width, 0.5*(spacing-tk)); 
  auto waffle_minor_T   = new G4Box(
      "waffle_minor_T", minorT_width, 0.5*tk, 0.5*(spacing-tk)); 
  auto waffle_minor_Tdown   = new G4Box(
      "waffle_minor_Tdown", minorT_width, 0.5*tk, 0.5*(spacing-tk-2*majorT_width)); 
  auto waffle_minor_sv = new G4UnionSolid(
      "waffle_minor_tmp", waffle_minor_bar, waffle_minor_T, nullptr, 
      G4ThreeVector(0, 0.5*(minor_width+tk), 0)); 
  waffle_minor_sv = new G4UnionSolid(
      "waffle_minor_tmp2", waffle_minor_sv, waffle_minor_Tdown, nullptr, 
      G4ThreeVector(0, -0.5*(minor_width+tk), 0)); 
  G4RotationMatrix* rotY2 = new G4RotationMatrix(0, 0, 0); 
  rotY2->rotateY(0.5*CLHEP::pi); 
  waffle_minor_sv = new G4UnionSolid(
      "waffle_minor_sv", waffle_minor_sv, waffle_minor_sv,
      rotY2, 
      G4ThreeVector(0, 0, 0.165*spacing)); 

  auto waffle_minor_lv = new G4LogicalVolume(waffle_minor_sv, fMatWaffle->GetMaterial(),
      "waffle_minor_lv"); 
  waffle_minor_lv->SetVisAttributes( new G4VisAttributes( G4Colour(1, 0, 0) ) ); 
  new G4PVPlacement(0, G4ThreeVector(0, -0.5*(major_width - minor_width - 2*tk), 0), 
      waffle_minor_lv, "waffle_minor_pv", fWaffleUnit->GetModLV(), 0, 5); 

}

SLArBaseDetModule* SLArDetCryostat::BuildSupportStructure(slargeo::EBoxFace kFace) {
  SLArBaseDetModule* waffle = new SLArBaseDetModule(); 

  // get cryostat dimensions
  G4ThreeVector dim;
  G4double cryo_tk = fGeoInfo->GetGeoPar("cryostat_tk"); 
  dim.set(
      fGeoInfo->GetGeoPar("target_x") + 2*cryo_tk,
      fGeoInfo->GetGeoPar("target_y") + 2*cryo_tk, 
      fGeoInfo->GetGeoPar("target_z") + 2*cryo_tk); 

  // get normal versor for the given cryostat face
  const G4ThreeVector normal = slargeo::BoxFaceNormal[kFace]; 
  
  // get unit waffle dimensions 
  G4ThreeVector wffl_unit_min, wffl_unit_max;
  fWaffleUnit->GetModSV()->BoundingLimits(wffl_unit_min, wffl_unit_max); 
  const G4ThreeVector wffl_dim = wffl_unit_max - wffl_unit_min; 
  G4ThreeVector wfflNormal(0, -1, 0); 

  const G4double _angle = wfflNormal.angle(normal);
  G4ThreeVector rot_axis = wfflNormal.cross(normal);
  if (rot_axis.mag2() < 1e-6) rot_axis = wfflNormal;
  G4RotationMatrix* rot = new G4RotationMatrix(rot_axis, _angle); 

  G4ThreeVector plane_dim = dim; 
  plane_dim.transform(*rot);
  printf("Cryostat size: %.0f %.0f %.0f\n", dim.x(), dim.y(), dim.z()); 
  printf("face normal: [%.0f, %.0f, %.0f]\n", normal.x(), normal.y(), normal.z()); 
  printf("rot axis: (%.0f, %.0f, %.0f), angle = %g\n", rot_axis.x(), rot_axis.y(), rot_axis.z(), _angle*CLHEP::rad);
  printf("Face %i: %.0f mm,  %.0f mm, %.0f mm\n", kFace, plane_dim.x(), plane_dim.y(), plane_dim.z()); 

  const int nz = floor( fabs(plane_dim.z() / wffl_dim.z()) ); 
  const int nx = floor( fabs(plane_dim.x() / wffl_dim.x()) ); 
  const int ny = 0; 
  const int nReplica[3] = {nx, ny, nz};

  G4ThreeVector max_dim(nx*wffl_dim.x(), wffl_dim.y(), nz*wffl_dim.z()); 

  G4String face_name = "waffle_face"+std::to_string(kFace)+"_sv"; 
  waffle->SetSolidVolume( new G4Box(
        face_name, 0.5*wffl_dim.x()*nx, 0.5*wffl_dim.y(), 0.5*wffl_dim.z()*nz)); 
  face_name = "waffle_face"+std::to_string(kFace)+"_lv"; 
  waffle->SetLogicVolume( new G4LogicalVolume(waffle->GetModSV(), fMatWorld->GetMaterial(), face_name)); 
  waffle->GetModLV()->SetVisAttributes( G4VisAttributes( G4Colour( 1, 0.9, 0.2) )); 

  std::vector<SLArPlaneParameterisation*> prmtr = 
  {
    new SLArPlaneParameterisation(
        kXAxis, G4ThreeVector(-0.5*(nx-1)*wffl_dim.x(), 0, 0), wffl_dim.x()), 
    new SLArPlaneParameterisation(
        kZAxis, G4ThreeVector(0, 0, -0.5*(nz-1)*wffl_dim.z()), wffl_dim.z()), 
  }; 

  auto build_parameterised_vol = [&](
      SLArBaseDetModule* origin, 
      SLArBaseDetModule* target, 
      G4String target_prefix,
      SLArPlaneParameterisation* rpars,
      int copyNo) {

    G4ThreeVector tmp_dim = max_dim; 
    G4ThreeVector origin_dim; 
    
    G4Box* originBox = (G4Box*)origin->GetModSV();
    //origin->GetGeoInfo()->DumpParMap(); 
    //target->GetGeoInfo()->DumpParMap(); 

    origin_dim[0] = 2*originBox->GetXHalfLength(); 
    origin_dim[1] = 2*originBox->GetYHalfLength(); 
    origin_dim[2] = 2*originBox->GetZHalfLength(); 

    G4ThreeVector perp_ax = wfflNormal.cross(rpars->GetReplicationAxisVector()); 
    G4double module_wdt = 0.; 
    for (int i=0; i<3; i++) {
      if ( fabs(perp_ax[i] * origin_dim[i] ) > 0 ) {
        tmp_dim[i] = origin_dim[i]; 
        module_wdt = origin_dim[i]; 
      }
    }

    target->SetSolidVolume(new G4Box(target_prefix+"_sv", 
          0.5*tmp_dim[0], 0.5*tmp_dim[1], 0.5*tmp_dim[2])); 
    target->SetLogicVolume(new G4LogicalVolume(target->GetModSV(), 
          fMatWorld->GetMaterial(), target_prefix+"_lv")); 
    G4cout<< "start pos: " << rpars->GetStartPos() << G4endl; 
    target->SetModPV(
        new G4PVParameterised(target_prefix+"_ppv", 
          origin->GetModLV(), target->GetModLV(), 
          rpars->GetReplicationAxis(), nReplica[rpars->GetReplicationAxis()],
          rpars, true));
    target->GetModPV()->SetCopyNo(copyNo); 
  }; 

  std::vector<SLArBaseDetModule*> waffle_plane_submodule; 

  for (auto &rpars : prmtr) {
    SLArBaseDetModule* target = nullptr; 
    SLArBaseDetModule* origin = nullptr;
    G4String prefix = "";
    G4int copyNo = 666;
    if (rpars == prmtr.back()) {
      target = waffle; 
      origin = waffle_plane_submodule.back();
      prefix = "waffle_plane";
      copyNo = 7; 
    } 
    else if (rpars == prmtr.front()) {
      waffle_plane_submodule.push_back( new SLArBaseDetModule() ); 
      target = waffle_plane_submodule.back();
      origin = fWaffleUnit;
      prefix = "waffle_row";
      copyNo = 6; 
    }
    else {
      G4cout << "SLArDetSuperCellArray::BuildSuperCellArray() WARNING: " << G4endl;
      G4cout << "I should not be here!" << G4endl;
      getchar(); 
      waffle_plane_submodule.push_back( new SLArBaseDetModule() ); 
      target = waffle_plane_submodule.back(); 
      origin = waffle_plane_submodule.rbegin()[1];
    }

    build_parameterised_vol(origin, target, prefix, rpars, copyNo);
  }

  for (auto &subModules : waffle_plane_submodule) {
    subModules->GetModLV()->SetVisAttributes( G4VisAttributes(false) ); 
  }

  return waffle;
}

void SLArDetCryostat::BuildCryostat()
{
  G4cerr << "SLArDetCryostat::BuildCryostat()" << G4endl;
  G4double tgtZ         = fGeoInfo->GetGeoPar("target_z");
  G4double tgtY         = fGeoInfo->GetGeoPar("target_y");
  G4double tgtX         = fGeoInfo->GetGeoPar("target_x");
  G4double cryo_tk      = fGeoInfo->GetGeoPar("cryostat_tk"); 
  G4double cryo_tot_tk  = fGeoInfo->GetGeoPar("cryostat_total_tk"); 
   
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Create Tank 
  G4double x_ = tgtX*0.5 + cryo_tot_tk;
  G4double y_ = tgtY*0.5 + cryo_tot_tk;
  G4double z_ = tgtZ*0.5 + cryo_tot_tk;
  const G4ThreeVector cryostat_dim(x_, y_, z_); 

  // Create outer box 
  G4Box* boxOut = new G4Box("fBoxOut_solid", 
      x_, y_, z_); 

  // Create inner box 
  G4Box* boxInn = new G4Box("fBoxInn_solid", 
      x_-cryo_tot_tk, y_-cryo_tot_tk, z_-cryo_tot_tk);

  fModSV =  
    new G4SubtractionSolid("cryostat_solid", 
        boxOut, boxInn, 0, G4ThreeVector(0,0,0));

  // Create Cryostat container volume
  G4cerr << "Create Cryostat" << G4endl;
  fGeoInfo->SetGeoPar("cryostat_x", 2*x_);
  fGeoInfo->SetGeoPar("cryostat_y", 2*y_);
  fGeoInfo->SetGeoPar("cryostat_z", 2*z_);
  fMaterial = fMatWorld->GetMaterial(); 

  SetLogicVolume(
    new G4LogicalVolume(GetModSV(), 
      fMaterial,
      "CryostatLV", 0, 0, 0)
    );

  // -------------------------------------------------------------------------
  // create cryostat layers

  G4cerr << "create cryostat layers" << G4endl; 
  for (const auto& ll : fCryostatStructure) {
    auto layer = ll.second;
    printf("layer: %i\n", ll.first); 
    layer->fMaterial = SLArMaterial::FindInMaterialTable(layer->fMaterialName); 
    layer->fModule = BuildCryostatLayer(layer->fName, 
        layer->fHalfSizeX, layer->fHalfSizeY, layer->fHalfSizeZ, 
        layer->fThickness, layer->fMaterial); 
    printf("placing layer in LV %p\n", static_cast<void*>(fModLV)); 
    layer->fModule->GetModPV(
        layer->fName, 0, G4ThreeVector(0, 0, 0), fModLV, 
        false, ll.first);
  }

  if (fBuildSupport) {
    BuildSupportStructureUnit();

    for (int i=0; i<6; i++) {
      auto kFace = (slargeo::EBoxFace)i; 
      auto waffle_face = BuildSupportStructure(kFace);
      waffle_face->GetModLV()->SetVisAttributes( G4VisAttributes(false) ); 
      const auto face_normal = slargeo::BoxFaceNormal[kFace]; 
      const G4ThreeVector waffle_local_normal(0, -1, 0); 
      G4ThreeVector rot_axis = face_normal.cross(waffle_local_normal); 
      G4double rot_angle = face_normal.angle(waffle_local_normal); 
      if (rot_axis.mag2() < 1e-6) rot_axis = face_normal; 

      G4String face_pv_name = "waffle_face"+std::to_string(i)+"_pv"; 
      G4ThreeVector pos = -face_normal * 
        (fabs(face_normal.dot(cryostat_dim)) 
         - 0.5*fGeoInfo->GetGeoPar("waffle_major_width")); 
      G4RotationMatrix* rot = new G4RotationMatrix(rot_axis, rot_angle);
      waffle_face->GetModPV(face_pv_name, rot, pos, fModLV, false, i+1); 
      fSupportStructure.insert( std::make_pair(kFace, waffle_face) ); 
    }
  }

  G4cerr << "done" << G4endl; 
  return; 
}

SLArBaseDetModule* SLArDetCryostat::BuildCryostatLayer(
    G4String name, 
    G4double x_, G4double y_, G4double z_, G4double tk_, 
    G4Material* mat) {

  G4Box* b_out = new G4Box("b_out_"+name, x_+tk_, y_+tk_, z_+tk_); 
  G4Box* b_in  = new G4Box("b_in_" +name, x_    , y_    , z_    ); 

  G4SubtractionSolid* solid = new G4SubtractionSolid(name+"_solid", 
      b_out, b_in, 0, G4ThreeVector(0, 0, 0)); 

  SLArBaseDetModule* mod = new SLArBaseDetModule(); 
  mod->SetMaterial(mat); 
  mod->SetSolidVolume(solid); 
  mod->SetLogicVolume(new G4LogicalVolume(
        mod->GetModSV(), mod->GetMaterial(), name+"LV", 0, 0, 0)); 

/*
 *  //create a daughter volume on the -z face to be used as a sensitive detector 
 *  //for neutron shielding studies
 *
 *  G4Box* b_test_sv = new G4Box("b_test_sv_"+name, x_, y_, 0.5*tk_); 
 *  G4LogicalVolume* b_test_lv = new G4LogicalVolume(b_test_sv, mat, "b_test_lv_"+name, 
 *      0, 0, 0, 0); 
 *  new G4PVPlacement(0, G4ThreeVector(0, 0, -z_-0.5*tk_), b_test_lv, "b_test_pv_"+name, 
 *      mod->GetModLV(), false, 8, true); 
 *
 */
  return mod; 
}

void SLArDetCryostat::SetVisAttributes() {
  G4VisAttributes* visAttributes = new G4VisAttributes();
  visAttributes->SetColour(0.611, 0.847, 0.988, 0.6);
  visAttributes->SetVisibility(true); 
  fModLV->SetVisAttributes( G4VisAttributes(false) );
  for (auto &ll : fCryostatStructure) {
    auto lv = ll.second->fModule->GetModLV();
    printf("%s\n", lv->GetName().c_str());
    lv->SetVisAttributes(visAttributes); 
  }
}

void SLArDetCryostat::BuildMaterials(G4String material_db) {
  for (auto &layer : fCryostatStructure) {
    SLArMaterial* mat = new SLArMaterial; 
    mat->SetMaterialID( layer.second->fMaterialName ); 
    mat->BuildMaterialFromDB(material_db); 
    layer.second->fMaterial = mat->GetMaterial(); 
  }

  fMatWaffle = new SLArMaterial(); 
  fMatWaffle->SetMaterialID("Steel"); 
  fMatWaffle->BuildMaterialFromDB(material_db); 

  return;
}

