// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file SLAr/src/SLArDetectorConstruction.cc
/// \brief Implementation of the SLArDetectorConstruction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/encodings.h"
#include "rapidjson/stringbuffer.h"

#include "SLArUserPath.hh"
#include "SLArAnalysisManager.hh"

#include "SLArDetectorConstruction.hh"

#include "detector/SLArBaseDetModule.hh"
#include "detector/Tank/SLArDetTank.hh"
#include "detector/Tank/SLArTankSD.hh"

#include "detector/ReadoutTile/SLArDetReadoutTile.hh"
#include "detector/ReadoutTile/SLArReadoutTileSD.hh"

#include "config/SLArCfgBaseSystem.hh"
#include "config/SLArCfgSuperCell.hh"
#include "config/SLArCfgReadoutTile.hh"
#include "config/SLArCfgMegaTile.hh"

//#include "G4GDMLParser.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Element.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"
#include "G4OpRayleigh.hh"
#include "G4VisAttributes.hh"
#include "G4PVReplica.hh"
#include "G4UImanager.hh"

#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4RunManager.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"

#include "G4PhysicalConstants.hh"
#include "G4UnitsTable.hh"

#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArDetectorConstruction::SLArDetectorConstruction(
    G4String geometry_cfg_file, G4String material_db_file)
 : G4VUserDetectorConstruction(),
   fGeometryCfgFile(""), 
   fMaterialDBFile(""),
   fTank(nullptr),
   fSuperCell(nullptr),
   fWorldLog(nullptr)
{ 
  fGeometryCfgFile = geometry_cfg_file; 
  fMaterialDBFile  = material_db_file; 
#ifdef SLAR_DEBUG
  printf("SLArDetectorConstruction Build with\ngeometry %s\nmaterials %s\n",
      fGeometryCfgFile.c_str(), fMaterialDBFile.c_str());
#endif
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArDetectorConstruction::~SLArDetectorConstruction(){
  G4cerr << "Deleting SLArDetectorConstruction... " << G4endl;
  for (G4int i=0; i<G4int(fVisAttributes.size()); ++i) 
  {
    delete fVisAttributes[i];
  }
  
  G4cerr << "SLArDetectorConstruction DONE" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArDetectorConstruction::Init() {
#ifdef SLAR_DEBUG
  printf("SLArDetectorConstruction::Init\ngeometry: %s\nmaterials: %s\n", 
      fGeometryCfgFile.c_str(), fMaterialDBFile.c_str());
#endif
  SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
  SLArAnaMgr->fAnaMsgr->AssignDetectorConstruction(this);

  // open geometry configuration file
  FILE* geo_cfg_file = std::fopen(fGeometryCfgFile, "r");
  char readBuffer[65536];
  rapidjson::FileReadStream is(geo_cfg_file, readBuffer, sizeof(readBuffer));

  rapidjson::Document d;
  d.ParseStream<rapidjson::kParseCommentsFlag>(is);
  assert(d.IsObject());
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Parse world dimensions
  if (d.HasMember("World")) {
    const rapidjson::Value& wrld = d["World"]; 
    assert(wrld.HasMember("dimensions")); 
    fWorldGeoPars.ReadFromJSON(wrld["dimensions"]);
  } else {
    fWorldGeoPars.RegisterGeoPar("size_x", 5*CLHEP::m); 
    fWorldGeoPars.RegisterGeoPar("size_y", 8*CLHEP::m); 
    fWorldGeoPars.RegisterGeoPar("size_z",20*CLHEP::m); 
  }


  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Initialize Tank objects
  G4cerr << "SLArDetectorConstruction::Init Tank" << G4endl;
  fTank     = new SLArDetTank();
  if (d.HasMember("Tank")) {
    const rapidjson::Value& tk = d["Tank"]; 
    assert(tk.HasMember("dimensions")); 
    fTank->GetGeoInfo()->ReadFromJSON(tk["dimensions"]);
  } else {
    fTank->BuildDefalutGeoParMap();
  }
  fTank->BuildMaterial(fMaterialDBFile);
  G4cerr << "SLArDetectorConstruction::Init Tank DONE" << G4endl;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Initialize Photodetectors
  G4cout << "SLArDetectorConstruction::Init SuperCells" << G4endl;
  
  if (d.HasMember("SuperCell")) {
    const auto sc = d["SuperCell"].GetObj();
    InitPDS(sc); 
  }


  G4cout << "SLArDetectorConstruction::Init PDS DONE" << G4endl;
  
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Initialize ReadoutTile
  if (d.HasMember("ReadoutTile")) {
    InitPix(d["ReadoutTile"].GetObj()); 
  }

  std::fclose(geo_cfg_file);
}

void SLArDetectorConstruction::InitPDS(const rapidjson::Value& pds) {
  fSuperCell = new SLArDetSuperCell(); 
  assert(pds.HasMember("dimensions")); 
  fSuperCell->GetGeoInfo()->ReadFromJSON(pds["dimensions"]); 
  fSuperCell->BuildMaterial(fMaterialDBFile); 

  SLArPDSystemConfig* pdsCfg = new SLArPDSystemConfig("PDSCfg"); 
  if (pds.HasMember("modules")) {
    assert(pds["modules"].IsArray());
    for (const auto &mdl : pds["modules"].GetArray()) {
      SLArCfgSuperCellArray* array = 
        new SLArCfgSuperCellArray(mdl["name"].GetString(), mdl["id"].GetInt());

      if (mdl.HasMember("positions")) {
        assert(mdl["positions"].IsArray());
        for (const auto &isc : mdl["positions"].GetArray()) {
          SLArCfgSuperCell* scCfg = new SLArCfgSuperCell(); 
          const char* cunit = isc["unit"].GetString(); 
          auto xyz = isc["xyz"].GetArray(); 
          auto rot = isc["rot"].GetArray(); 

          scCfg->SetIdx(isc["copy"].GetInt()); 

          scCfg->SetX(xyz[0].GetDouble()*G4UIcommand::ValueOf(cunit)); 
          scCfg->SetY(xyz[1].GetDouble()*G4UIcommand::ValueOf(cunit)); 
          scCfg->SetZ(xyz[2].GetDouble()*G4UIcommand::ValueOf(cunit)); 

          scCfg->SetPhi  (rot[0].GetDouble()*TMath::DegToRad()); 
          scCfg->SetTheta(rot[1].GetDouble()*TMath::DegToRad()); 
          scCfg->SetPsi  (rot[2].GetDouble()*TMath::DegToRad()); 

          scCfg->Set2DSize_X(fSuperCell->GetGeoPar("cell_z")); 
          scCfg->Set2DSize_Y(fSuperCell->GetGeoPar("cell_x")); 

          array->RegisterElement(scCfg); 
        }
      }

      pdsCfg->RegisterModule(array);
    }
  }

  SLArAnalysisManager::Instance()->LoadPDSCfg(pdsCfg);

}

void SLArDetectorConstruction::InitPix(const rapidjson::Value& pixsys) {
  SLArCfgPixSys* pixCfg = new SLArCfgPixSys("PixCfg");

  fReadoutTile = new SLArDetReadoutTile();
  fReadoutTile->GetGeoInfo()->ReadFromJSON(pixsys["dimensions"]); 
  fReadoutTile->BuildMaterial(fMaterialDBFile);

  if (pixsys.HasMember("modules")) {
    assert(pixsys["modules"].IsArray()); 

    for (const auto &mtile : pixsys["modules"].GetArray()) {
      // Setup megatile
      SLArDetReadoutPlane* megatile = new SLArDetReadoutPlane(); 
      assert(mtile.HasMember("dimensions")); 
      megatile->GetGeoInfo()->ReadFromJSON(mtile["dimensions"]); 
      megatile->BuildMaterial(fMaterialDBFile); 
      fReadoutMegaTile.insert(std::make_pair(mtile["name"].GetString(),megatile)); 

      assert(mtile.HasMember("positions")); 
      assert(mtile["positions"].IsArray()); 

      for (const auto &pos : mtile["positions"].GetArray()) {
        SLArCfgMegaTile* mtileCfg = new SLArCfgMegaTile(); 
        if (pos.HasMember("copy")) mtileCfg->SetIdx(pos["copy"].GetInt()); 
        if (mtile.HasMember("name")) {
          G4String mtname =Form("%s_%i", mtile["name"].GetString(), mtileCfg->GetIdx());
          mtileCfg->SetName(mtname);
        }
        
        assert(pos.HasMember("xyz")); 
        assert(pos.HasMember("rot")); 
        auto xyz = pos["xyz"].GetArray(); 
        auto rot = pos["rot"].GetArray(); 
        const char* unit = pos["unit"].GetString(); 

        mtileCfg->SetX(xyz[0].GetDouble()*G4UIcommand::ValueOf(unit)); 
        mtileCfg->SetY(xyz[1].GetDouble()*G4UIcommand::ValueOf(unit)); 
        mtileCfg->SetZ(xyz[2].GetDouble()*G4UIcommand::ValueOf(unit)); 

        mtileCfg->SetPhi  (rot[0].GetDouble()*TMath::DegToRad()); 
        mtileCfg->SetTheta(rot[1].GetDouble()*TMath::DegToRad()); 
        mtileCfg->SetPsi  (rot[2].GetDouble()*TMath::DegToRad()); 
        
        double norm[3] = {1, 0, 0}; 
        if (pos.HasMember("norm")) {
          assert(pos["norm"].IsArray()); 
          const auto pnorm = pos["norm"].GetArray();
          norm[0] = pnorm[0].GetDouble(); 
          norm[1] = pnorm[1].GetDouble(); 
          norm[2] = pnorm[2].GetDouble(); 

          mtileCfg->SetNormal(norm[0], norm[1], norm[2]); 
        }

        printf("SLArDetectorConstruction::InitPix()\n"); 
        printf("Registering module %s at [%.2f, %.2f, %.2f]\n",
            mtileCfg->GetName(), mtileCfg->GetX(), mtileCfg->GetY(), mtileCfg->GetZ()); 
        printf("size: [%.2f, %.2f, %.2f] × %g mm\n", 
            xyz[0].GetDouble(), xyz[1].GetDouble(), xyz[2].GetDouble(), 
            G4UIcommand::ValueOf(unit)); 
        pixCfg->RegisterModule(mtileCfg);
      } // end of positions loop
    } // end of Megatile models loop
    SLArAnalysisManager::Instance()->LoadPixCfg(pixCfg); 
  } // endif pixsys.HasMember("modules")
}

G4VPhysicalVolume* SLArDetectorConstruction::Construct()
{
#ifdef SLAR_DEBUG
  printf("SLArDetectorConstruction::Construct\ngeometry: %s\nmaterials: %s\n", 
      fGeometryCfgFile.c_str(), fMaterialDBFile.c_str());
#endif
  Init();

  // ------------- Volumes --------------
  // 1. Build and place WORLD volume
  G4Box* expHall_box = new G4Box("World", 
      fWorldGeoPars.GetGeoPar("size_x"), 
      fWorldGeoPars.GetGeoPar("size_y"), 
      fWorldGeoPars.GetGeoPar("size_z"));  

  SLArMaterial* matWorld = new SLArMaterial("Air"); 
  matWorld->BuildMaterialFromDB(fMaterialDBFile); 

  fWorldLog   
    = new G4LogicalVolume(expHall_box, matWorld->GetMaterial(), "World",0,0,0);

  G4VPhysicalVolume* expHall_phys
    = new G4PVPlacement(0,G4ThreeVector(),fWorldLog,"World",0,false,0);

  // 2. Build and place the Cryostat/LAr target
  G4cerr << "\nSLArDetectorConstruction: Building the Tank" << G4endl;
  fTank->BuildTank();
  fTank->GetModPV("Tank", 0,
      G4ThreeVector(0, 0, 0), 
      fWorldLog, false, 20);

  // 3. Build and place the "conventional" Photon Detection System 
  if (fSuperCell) BuildAndPlaceSuperCells();

  // 4. Build and place the "pixel-based" readout system 
  BuildAndPlaceReadoutTiles(); 

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //Visualization attributes
  fTank->SetVisAttributes();
  if (fSuperCell) fSuperCell->SetVisAttributes();

  G4VisAttributes* visAttributes = new G4VisAttributes();
  visAttributes->SetColor(0.25,0.54,0.79, 0.0);
  fWorldLog->SetVisAttributes(visAttributes);

  //G4PhysicalVolumeStore* pvStore = G4PhysicalVolumeStore::GetInstance();
  /*
   *for (auto &pv : *pvStore)
   *{
   *  G4cout << pv->GetName() << ", LV: " 
   *         << pv->GetLogicalVolume()->GetName()  
   *         << pv->GetCopyNo() << " at " << pv << G4endl;
   *  G4String name = pv->GetName();
   *  if (name.contains("PMT"))
   *  {
   *    G4cout << "mother = " << 
   *      pv->GetMotherLogical()->GetName() << G4endl;
   *  }
   *}
   */
  

  //always return the physical World
  return expHall_phys;
}

void SLArDetectorConstruction::ConstructSDandField()
{
    // sensitive detectors 
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    G4String SDname;
    
    //Set PMT SD
    G4VSensitiveDetector* sipmSD
      = new SLArReadoutTileSD(SDname="/tile/sipm");
    SDman->AddNewDetector(sipmSD);
    SetSensitiveDetector(
          fReadoutTile->GetSiPM()->GetModLV(), 
          sipmSD);

    // Set Tank SD
    G4VSensitiveDetector* targetSD
      = new SLArTankSD(SDname="/Tank/Target");
    SDman->AddNewDetector(targetSD);
    SetSensitiveDetector(
        fTank->GetTarget()->GetModLV(), 
        targetSD);
        
}

SLArDetTank* SLArDetectorConstruction::GetDetTank() 
{
  return fTank;
}

//SLArDetPMT* SLArDetectorConstruction::GetDetPMT(const char* mod) 
//{
  //SLArDetPMT* pmt = nullptr;
  //pmt = fPMTs.find(mod)->second;
  //return pmt;
//}

//std::map<G4String,SLArDetPMT*>& SLArDetectorConstruction::GetDetPMTs()
//{
  //return fPMTs;
//}


G4LogicalVolume* SLArDetectorConstruction::GetLogicWorld()
{
  return fWorldLog;
}

G4String SLArDetectorConstruction::GetFirstChar(G4String line)
{
  int length = line.length();
  G4String ch = "";
  int i = 0;
  while (isspace(line[i])) i++;

  if (i==length) return "#";
  else           return &line[i];
}


void SLArDetectorConstruction::BuildAndPlaceSuperCells()
{
  fSuperCell->BuildSuperCell();

  //new G4LogicalSkinSurface("SCCoating_skin", 
      //fSuperCell->GetCoating()->GetModLV(), 
      //fSuperCell->GetCoatingMaterial()->
      //GetMaterialBuilder()->GetSurface()
      //);


  // Get PMTSystem Configuration
  SLArAnalysisManager* SLArAnaMgr  = SLArAnalysisManager::Instance();
  SLArPDSystemConfig*  pdsCfg = SLArAnaMgr->GetPDSCfg();

  for (auto &pdsArray : pdsCfg->GetModuleMap())
  {
    SLArCfgSuperCellArray* arrayCfg = pdsArray.second;
    G4cout << arrayCfg->GetName() <<" map: " 
      << arrayCfg->GetMap().size() << " entries" << G4endl;

    int iSC = 0;

    for (auto &scinfo : arrayCfg->GetMap())
    {

      //G4cout << "Getting PMT model info...";
      G4RotationMatrix* rotPMT = new G4RotationMatrix();
      rotPMT->rotateX(scinfo.second->GetPhi()); 
      rotPMT->rotateY(scinfo.second->GetTheta()); 
      rotPMT->rotateZ(scinfo.second->GetPsi()); 
      
      G4ThreeVector basePos(
          scinfo.second->GetX(), 
          scinfo.second->GetY(), 
          scinfo.second->GetZ() 
          );

      G4ThreeVector pmtPos = basePos; 
 
      fSuperCellsPV.push_back(  
          fSuperCell->GetModPV(
            Form("SC%i", iSC), rotPMT, 
            pmtPos,
            fTank->GetModLV(), false, scinfo.second->GetIdx()
            )
          );
      // set physical placement in pmt cfg
      scinfo.second->SetPhysX(pmtPos.x());
      scinfo.second->SetPhysY(pmtPos.y());
      scinfo.second->SetPhysZ(pmtPos.z());
      scinfo.second->BuildGShape();
      iSC++;
    }

  }
   G4cout << "SLArDetectorConstruction::BuildAndPlacePMTs DONE" << G4endl;
  return;
}

void SLArDetectorConstruction::BuildAndPlaceReadoutTiles() {
  fReadoutTile->BuildReadoutTile(); 
  fReadoutTile->SetVisAttributes();

  fReadoutTile->BuildLogicalSkinSurface(); 

  SLArCfgPixSys* pixCfg = SLArAnalysisManager::Instance()->GetPixCfg(); 
  
  printf("SLArDetectorConstruction::BuildAndPlaceReadoutTiles\n"); 
  printf("Registered readout tiles models:\n"); 
  for (const auto &models : fReadoutMegaTile) {
    printf("- %s\n", models.first.c_str());
  }
  printf("Nr of readout tiles modules: %lu\n", pixCfg->GetModuleMap().size()); 

  for (auto &mtileCfg_ : pixCfg->GetModuleMap()) {
    SLArCfgMegaTile* mtileCfg = mtileCfg_.second;
    G4String mtile_module_name = mtileCfg->GetName();
    mtile_module_name.resize(mtile_module_name.find("_"));
    printf("mtile model: %s\n", mtile_module_name.c_str()); 
    if (fReadoutMegaTile.count(mtile_module_name)) {
      SLArDetReadoutPlane* megatile = fReadoutMegaTile[mtile_module_name];
      if (!megatile->GetModLV()) {
        printf("SLArDetectorConstruction::BuildAndPlaceReadoutTiles\n"); 
        printf("Building readout megatile %s\n", mtile_module_name.c_str());
        fReadoutMegaTile[mtile_module_name]->BuildReadoutPlane(fReadoutTile); 
      }

      G4ThreeVector tile_pos(
          mtileCfg->GetX(), 
          mtileCfg->GetY(), 
          mtileCfg->GetZ()); 
      G4RotationMatrix* tile_rot = new G4RotationMatrix(
          mtileCfg->GetPhi(), 
          mtileCfg->GetTheta(), 
          mtileCfg->GetPsi());

      auto pv = megatile->GetModPV(mtileCfg->GetName(), 
          tile_rot, tile_pos, fTank->GetTarget()->GetModLV(), 
          false, mtileCfg->GetIdx());
      auto lv = megatile->GetModLV();
      auto row = megatile->GetTileRow();

      EAxis kRowAxis = kZAxis, kTileAxis = kXAxis;
      G4int nRowReplicas = 0, nTileReplicas = 0;
      G4double wdtRow = 0., wdtTile = 0.;
      G4double oftRow = 0., oftTile = 0.;
      G4bool   cnsRow = kFALSE, cnsTile = kFALSE;

      if (lv->GetDaughter(0)->IsReplicated()) {
        lv->GetDaughter(0)->GetReplicationData(kRowAxis, nRowReplicas, wdtRow, oftRow, cnsRow);
#ifdef SLAR_DEBUG
        printf("%s is replicated %i times along axis %i (wdt=%g mm, offset= %g mm)\n", 
            pv->GetName().c_str(), nRowReplicas, kRowAxis, wdtRow, oftRow); 
        //getchar(); 

#endif
      } else {
        printf("%s is not replicated! Why??\n", pv->GetName().c_str()); 
        getchar(); 
      }


      auto row_pv = (G4PVReplica*)row->GetModPV();
      if (row_pv->IsReplicated()) {
        row_pv->GetReplicationData(kTileAxis, nTileReplicas, wdtTile, oftTile, cnsTile);
#ifdef SLAR_DEBUG
        printf("%s is replicated %i times along axis %i (wdt=%g mm, offset=%g mm)\n", 
            row_pv->GetName().c_str(), nTileReplicas, kTileAxis, 
            wdtTile, oftTile); 
        //getchar(); 
#endif
      } else {
        printf("%s is not replicated! Why??\n", row_pv->GetName().c_str()); 
        getchar(); 
      }

      oftRow  += megatile->GetGeoPar("rdoutplane_x")*0.5; 
      oftTile -= megatile->GetGeoPar("rdoutplane_z")*0.5; 

      for (int ii =0; ii<nRowReplicas; ii++) {
        
        for (int jj=0; jj<nTileReplicas; jj++) {
          SLArCfgReadoutTile* cell_cfg = new SLArCfgReadoutTile((ii+1)*100 + jj);
          G4ThreeVector cell_pos(-ii*wdtRow + oftRow, 0., jj*wdtTile + oftTile);
          cell_cfg->SetZ(cell_pos.z()); 
          cell_cfg->SetY(cell_pos.y());
          cell_cfg->SetX(cell_pos.x());
          
          G4ThreeVector phys_pos = tile_pos 
            + cell_pos.rotate(tile_rot->getPhi(), tile_rot->getTheta(), tile_rot->getPsi()); 
          
          cell_cfg->SetPhysX( (phys_pos).x() ); 
          cell_cfg->SetPhysY( (phys_pos).y() ); 
          cell_cfg->SetPhysZ( (phys_pos).z() );  

#ifdef SLAR_DEBUG
          // TODO: check positioning is ok
          // seems ok, but better check it twice
          // printf("%s megatile pos: [%.2f, %.2f, %.2f]: cell %i pos [%.2f, %.2f, %.2f]: phys pos [%.2f, %.2f, %.2f]\n", 
              //mtileCfg->GetName(), tile_pos.x(), tile_pos.y(), tile_pos.z(), 
              //cell_cfg->GetIdx(), cell_pos.x(), cell_pos.y(), cell_pos.z(),
              //phys_pos.x(), phys_pos.y(), phys_pos.z()
              //);
#endif

          cell_cfg->Set2DSize_X(fReadoutTile->GetGeoPar("tile_z")); 
          cell_cfg->Set2DSize_Y(fReadoutTile->GetGeoPar("tile_x")); 
          cell_cfg->BuildGShape(); 
          mtileCfg->RegisterElement(cell_cfg); 
        }
        //getchar(); 

      }
    } else {
      G4cerr << "MegaTile model " << mtile_module_name 
        << " is not registered in fReadoutMegaTile!" << G4endl; 
    }
  }

}

