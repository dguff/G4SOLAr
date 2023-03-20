/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArDetCryostat.cc
 * @created     Wed Mar 15, 2023 11:57:41 CET
 */
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"

#include "detector/TPC/SLArDetCryostat.hh"

SLArCryostatLayer::SLArCryostatLayer() : 
  fName("CryostatLayer"), 
  fHalfSizeX(1.), fHalfSizeY(1.), fHalfSizeZ(1.), fThickness(1.), 
  fMaterialName("MATERIAL_NAME"), fMaterial(nullptr), fModule(nullptr)
{}

SLArCryostatLayer::SLArCryostatLayer(
    G4String   model_name, 
    G4double*  halfSize,  
    G4double   thickness,
    G4String   material_name)
  : fMaterial(nullptr), fModule(nullptr)
{

  fName      = model_name;
  fHalfSizeX =  halfSize[0];
  fHalfSizeY =  halfSize[1]; 
  fHalfSizeZ =  halfSize[2]; 
  fThickness =  thickness; 

  fMaterialName = material_name;
}



SLArDetCryostat::SLArDetCryostat() : 
  fMatWorld(nullptr)
{
    
}

SLArDetCryostat::~SLArDetCryostat()
{
}


void SLArDetCryostat::BuildCryostatStructure(const rapidjson::Value& jcryo) {
  assert(jcryo.IsArray()); 
  printf("SLArDetCryostat::BuildCryostatStructure\n");

  G4double tgtZ = fGeoInfo->GetGeoPar("target_z");
  G4double tgtY = fGeoInfo->GetGeoPar("target_y");
  G4double tgtX = fGeoInfo->GetGeoPar("target_x");

  G4double cryostat_tk = 0.; 

  // compute total thickness
  for (const auto &layer : jcryo.GetArray()) {
    if (layer.HasMember("thickness")) 
      cryostat_tk += fGeoInfo->ParseJsonVal(layer["thickness"]); 
  }

  fGeoInfo->RegisterGeoPar("cryostat_tk", cryostat_tk); 

  G4double halfSize[3] = {tgtX*0.5 + cryostat_tk, 
                          tgtY*0.5 + cryostat_tk, 
                          tgtZ*0.5 + cryostat_tk}; 
  for (const auto &layer : jcryo.GetArray()) {
    if (layer.HasMember("thickness")) {
      G4double tk_ = SLArGeoInfo::ParseJsonVal(layer["thickness"]);
      if (tk_ == 0.) continue;

      assert(layer.HasMember("id")); 
      assert(layer.HasMember("name")); 
      assert(layer.HasMember("material")); 
      for (size_t k=0; k<3; k++) halfSize[k] -= tk_; 
      
      SLArCryostatLayer* ll = new SLArCryostatLayer(
          layer["name"].GetString(), halfSize, 
          SLArGeoInfo::ParseJsonVal(layer["thickness"]), 
          layer["material"].GetString());

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

void SLArDetCryostat::BuildCryostat()
{
  G4cerr << "SLArDetCryostat::BuildCryostat()" << G4endl;
  G4double tgtZ         = fGeoInfo->GetGeoPar("target_z");
  G4double tgtY         = fGeoInfo->GetGeoPar("target_y");
  G4double tgtX         = fGeoInfo->GetGeoPar("target_x");
  G4double cryo_tk      = fGeoInfo->GetGeoPar("cryostat_tk"); 
   
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Create Tank 
  G4double x_ = tgtX*0.5 + cryo_tk;
  G4double y_ = tgtY*0.5 + cryo_tk;
  G4double z_ = tgtZ*0.5 + cryo_tk;
  // Create outer box 
  G4Box* boxOut = new G4Box("fBoxOut_solid", 
      x_, y_, z_); 

  // Create inner box 
  G4Box* boxInn = new G4Box("fBoxInn_solid", 
      x_-cryo_tk, y_-cryo_tk, z_-cryo_tk);

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

  // create a daughter volume on the -z face to be used as a sensitive detector 
  // for neutron shielding studies

  G4Box* b_test_sv = new G4Box("b_test_sv_"+name, x_, y_, 0.5*tk_); 
  G4LogicalVolume* b_test_lv = new G4LogicalVolume(b_test_sv, mat, "b_test_lv_"+name, 
      0, 0, 0, 0); 
  new G4PVPlacement(0, G4ThreeVector(0, 0, -z_-0.5*tk_), b_test_lv, "b_test_pv_"+name, 
      mod->GetModLV(), false, 8, true); 

  return mod; 
}

void SLArDetCryostat::SetVisAttributes() {
  G4VisAttributes* visAttributes = new G4VisAttributes();
  visAttributes->SetColour(0.611, 0.847, 0.988, 0.6);
  visAttributes->SetVisibility(true); 
  fModLV->SetVisAttributes( visAttributes );
  for (size_t j=0; j<fModLV->GetNoDaughters(); j++) {
    auto pv = fModLV->GetDaughter(j); 
    printf("%s\n", pv->GetName().c_str());
    pv->GetLogicalVolume()->SetVisAttributes(visAttributes); 
  }
}
