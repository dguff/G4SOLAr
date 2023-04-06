/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArDetSuperCellArray.cc
 * @created     Fri Mar 24, 2023 16:08:29 CET
 */

#include "detector/SuperCell/SLArDetSuperCellArray.hh"
#include "config/SLArCfgSuperCellArray.hh"
#include "config/SLArCfgSuperCell.hh"
#include "G4PVParameterised.hh"
#include "G4Box.hh"
#include "G4VisAttributes.hh"

SLArDetSuperCellArray::SLArDetSuperCellArray() : 
  SLArBaseDetModule(), 
  fTPCID(0), 
  fMaterialBase(nullptr), fSuperCell(nullptr),  
  fPosition(0, 0, 0), fGlobalPosition(0, 0, 0), fNormal(0, 1, 0),
  fRotation(nullptr), fPhotoDetModel("")
{
  fGeoInfo = new SLArGeoInfo(); 
}

SLArDetSuperCellArray::~SLArDetSuperCellArray() {}

void SLArDetSuperCellArray::BuildMaterial(G4String materials_db) {
  fMaterialBase = new SLArMaterial("LAr"); 
  fMaterialBase->BuildMaterialFromDB(materials_db); 
  return;
}

void SLArDetSuperCellArray::Init(const rapidjson::Value& jconf) {
  G4String xyz_suffix[3] = {"x", "y", "z"}; 
  G4String ang_suffix[3] = {"phi", "theta", "psi"}; 

  printf("0 - here\n");
  assert(jconf.IsObject()); 
  auto jarray = jconf.GetObject(); 

  assert(jarray.HasMember("copyID")); 
  assert(jarray.HasMember("position")); 
  assert(jarray.HasMember("dimensions")); 
  assert(jarray.HasMember("rot")); 
  assert(jarray.HasMember("normal")); 
  assert(jarray.HasMember("tpcID")); 
  assert(jarray.HasMember("photodet_model"));


  SetID( jarray["copyID"].GetInt() ); 

  fPhotoDetModel = jarray["photodet_model"].GetString(); 

  fTPCID = jarray["tpcID"].GetInt();

  auto jpos = jarray["position"].GetObject(); 
  int idim = 0; 
  G4double vunit = SLArGeoInfo::Unit2Val(jpos["unit"]); 
  for (const auto &v : jpos["xyz"].GetArray()) {
    fPosition[idim] = (v.GetDouble() * vunit); 
    fGeoInfo->RegisterGeoPar("pos_"+xyz_suffix[idim], fPosition[idim]); 
    idim++; 
  }

  auto jrot = jarray["rot"].GetObject(); 
  vunit = SLArGeoInfo::Unit2Val(jrot["unit"]); 
  assert(jrot.HasMember("val")); 
  assert(jrot["val"].IsArray()); 
  assert(jrot["val"].GetArray().Size() == 3);
  double eulerAngles[3] = {0.}; 
  idim = 0; 
  for (const auto &v : jrot["val"].GetArray()) {
     eulerAngles[idim] = v.GetDouble()*vunit; 
     fGeoInfo->RegisterGeoPar("scarray_"+ang_suffix[idim], eulerAngles[idim]); 
     idim++; 
  }
  fRotation = new G4RotationMatrix(eulerAngles[0], eulerAngles[1], eulerAngles[2]); 
 
  auto jdim = jarray["dimensions"].GetArray(); 
  fGeoInfo->ReadFromJSON(jdim); 

  idim = 0;
  for (const auto &v : jarray["normal"].GetArray()) {
    fNormal[idim] = v.GetDouble(); 
    idim++; 
  }

  if (jarray.HasMember("replication_data")) {
    if (jarray["replication_data"].IsObject()) {
      auto parameterisation = new SLArPlaneParameterisation(jarray["replication_data"]); 
      fParameterisation.push_back(parameterisation); 
    } else if (jarray["replication_data"].IsArray()) {
      for (const auto &rdata : jarray["replication_data"].GetArray()) {
        auto parameterisation = new SLArPlaneParameterisation(rdata); 
        fParameterisation.push_back(parameterisation);         
      }
    } else {
      printf("WARNING: CANNOT PARSE SUPERCELL ARRAY REPLICATION DATA\n");
    }
  }
  
  return; 
}

void SLArDetSuperCellArray::BuildSuperCellArray(SLArDetSuperCell* superCell) {
  G4ThreeVector max_dim; 
  fSuperCell = superCell;
  max_dim[0] = fGeoInfo->GetGeoPar("dim_x");
  max_dim[1] = fGeoInfo->GetGeoPar("dim_y");
  max_dim[2] = fGeoInfo->GetGeoPar("dim_z");

  auto build_parameterised_vol = [&](
      SLArBaseDetModule* origin, 
      SLArBaseDetModule* target, 
      G4String target_prefix,
      SLArPlaneParameterisation* rpars) {

    G4ThreeVector tmp_dim = max_dim; 
    G4ThreeVector origin_dim; 
    
    G4Box* originBox = (G4Box*)origin->GetModSV();
    origin->GetGeoInfo()->DumpParMap(); 
    target->GetGeoInfo()->DumpParMap(); 

    origin_dim[0] = 2*originBox->GetXHalfLength(); 
    origin_dim[1] = 2*((G4Box*)origin->GetModSV())->GetYHalfLength(); 
    origin_dim[2] = 2*((G4Box*)origin->GetModSV())->GetZHalfLength(); 

    G4ThreeVector perp_ax = fNormal.cross(rpars->GetReplicationAxisVector()); 
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
          fMaterialBase->GetMaterial(), target_prefix+"_lv")); 
    auto start_ = ComputeArrayTrueLength(
        origin_dim.dot(rpars->GetReplicationAxisVector()), 
        rpars->GetSpacing(), 
        rpars->GetReplicationAxisVector().dot(max_dim)); 
    rpars->SetStartPos( 
        0.5*rpars->GetReplicationAxisVector()
        *(-start_.second + origin_dim.dot(rpars->GetReplicationAxisVector())) 
        -0.5*fNormal*(fNormal.dot(max_dim))
        );
    G4cout<< "start pos: " << rpars->GetStartPos() << G4endl; 
    target->SetModPV(
        new G4PVParameterised(target_prefix+"_ppv", 
          origin->GetModLV(), target->GetModLV(), 
          rpars->GetReplicationAxis(), start_.first,
          rpars, true)); 
  }; 

  for (auto &rpars : fParameterisation) {
    SLArBaseDetModule* target = nullptr; 
    SLArBaseDetModule* origin = nullptr;
    G4String prefix = "";
    if (rpars == fParameterisation.back()) {
      target = this; 
      origin = fSubModules.back();
      prefix = "SC_row";
    } 
    else if (rpars == fParameterisation.front()) {
      fSubModules.push_back( new SLArBaseDetModule() ); 
      target = fSubModules.back(); 
      origin = superCell;
      prefix = "SC";
    }
    else {
      G4cout << "SLArDetSuperCellArray::BuildSuperCellArray() WARNING: " << G4endl;
      G4cout << "I should not be here!" << G4endl;
      fSubModules.push_back( new SLArBaseDetModule() ); 
      target = fSubModules.back(); 
      origin = fSubModules.rbegin()[1];
    }

    build_parameterised_vol(origin, target, prefix, rpars);
  }

  fModLV->SetVisAttributes( G4VisAttributes(false) ); 

}

std::pair<int, G4double> SLArDetSuperCellArray::ComputeArrayTrueLength(
    G4double sc_dim, G4double spacing, G4double max_len) {
  G4double len = sc_dim;
  G4double len_tmp = len;
  G4int n_replica = 0; 
  while (len_tmp <= max_len) {
    len = len_tmp; 
    n_replica++;
    len_tmp += (spacing); 
  } 

  return std::make_pair(n_replica, fabs(len));
};

SLArCfgSuperCellArray* SLArDetSuperCellArray::BuildSuperCellArrayCfg() {
  SLArCfgSuperCellArray* arrayCfg = new SLArCfgSuperCellArray("SC_array_"+std::to_string(fID), fID); 
  arrayCfg->SetIdx( fID ); 

  arrayCfg->SetNormal( fNormal.x(), fNormal.y(), fNormal.z() ); 
  arrayCfg->SetupAxes(); 
  arrayCfg->SetPhi  ( fGeoInfo->GetGeoPar("scarray_phi") ); 
  arrayCfg->SetTheta( fGeoInfo->GetGeoPar("scarray_theta") ); 
  arrayCfg->SetPsi  ( fGeoInfo->GetGeoPar("scarray_psi") ); 

  auto sc_array = (G4PVParameterised*)fModLV->GetDaughter(0); 
  auto sc_row   = (G4PVParameterised*)fSubModules.front()->GetModLV()->GetDaughter(0);  

  auto get_replication_data = [](G4PVParameterised* pv) {
    SLArPlaneParameterisation::PlaneReplicationData_t data; 
    pv->GetReplicationData(data.fReplicaAxis, data.fNreplica, 
        data.fWidth, data.fOffset, data.fConsuming); 
    auto parameterisation = (SLArPlaneParameterisation*)pv->GetParameterisation(); 
    data.fReplicaAxisVec = parameterisation->GetReplicationAxisVector(); 
    data.fStartingPos = parameterisation->GetStartPos(); 
    data.fWidth = parameterisation->GetSpacing(); 
    return data;
  };

  auto rpl_sc_row = get_replication_data(sc_array); 
  auto rpl_sc_clm = get_replication_data(sc_row); 
  auto rot_inv = new G4RotationMatrix(*fRotation); 
  rot_inv->invert(); 

  for (int i_sc_row = 0; i_sc_row<rpl_sc_row.fNreplica; i_sc_row++) {
    G4ThreeVector pos_sc_row = 
      rpl_sc_row.fStartingPos + rpl_sc_row.fWidth*i_sc_row*rpl_sc_row.fReplicaAxisVec;

    for (int i_sc_clm = 0; i_sc_clm < rpl_sc_clm.fNreplica; i_sc_clm++) {
      G4int sc_idx = (i_sc_row+1)*100 + i_sc_clm;
      G4String scName = Form("%s_%i_%i", 
          fPhotoDetModel.data(), arrayCfg->GetIdx(), sc_idx); 
      SLArCfgSuperCell* scCfg = new SLArCfgSuperCell(sc_idx);
      scCfg->SetName(scName);

      G4ThreeVector sc_local_pos = pos_sc_row + 
        rpl_sc_clm.fStartingPos + rpl_sc_clm.fWidth*i_sc_clm*rpl_sc_clm.fReplicaAxisVec;
      scCfg->SetX(sc_local_pos.x()); 
      scCfg->SetY(sc_local_pos.y()); 
      scCfg->SetZ(sc_local_pos.z()); 

      G4ThreeVector sc_abs_pos = fGlobalPosition + sc_local_pos.transform(*rot_inv); 

      scCfg->SetPhysX( sc_abs_pos.x() ); 
      scCfg->SetPhysY( sc_abs_pos.y() ); 
      scCfg->SetPhysZ( sc_abs_pos.z() ); 

      scCfg->SetPhi( arrayCfg->GetPhi() ); 
      scCfg->SetTheta( arrayCfg->GetTheta() ); 
      scCfg->SetPsi( arrayCfg->GetPsi() ); 

      scCfg->SetNormal( arrayCfg->GetNormal() ); 
      scCfg->SetupAxes(); 

      const auto scBox = (G4Box*)fSuperCell->GetModSV();
      scCfg->SetSize( 2*scBox->GetXHalfLength(),
                      2*scBox->GetYHalfLength(), 
                      2*scBox->GetZHalfLength() ); 

      arrayCfg->RegisterElement( scCfg );
    }
  }

  return arrayCfg;
}
