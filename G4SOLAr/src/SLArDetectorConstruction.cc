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
#include "detector/TPC/SLArDetTPC.hh"
#include "detector/TPC/SLArLArSD.hh"

#include "detector/ReadoutTile/SLArDetReadoutTile.hh"
#include "detector/ReadoutTile/SLArReadoutTileSD.hh"

#include "detector/SuperCell/SLArSuperCellSD.hh"

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
#include "G4PVParameterised.hh"
#include "G4UImanager.hh"

#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4RunManager.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4PSTermination.hh"
#include "G4PSFlatSurfaceCurrent.hh"
#include "G4SDParticleFilter.hh"

#include "G4PhysicalConstants.hh"
#include "G4UnitsTable.hh"

#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArDetectorConstruction::SLArDetectorConstruction(
    G4String geometry_cfg_file, G4String material_db_file)
 : G4VUserDetectorConstruction(),
   fGeometryCfgFile(""), 
   fMaterialDBFile(""),
   fTPC(nullptr),
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
  // Initialize TPC objects
  G4cerr << "SLArDetectorConstruction::Init TPC" << G4endl;
  fTPC     = new SLArDetTPC();
  if (d.HasMember("TPC")) {
    const rapidjson::Value& tpc = d["TPC"]; 
    assert(tpc.HasMember("dimensions"));
    assert(tpc.HasMember("Cryostat_structure")); 
    fTPC->GetGeoInfo()->ReadFromJSON(tpc["dimensions"]);
    fTPC->BuildCryostatStructure(tpc["Cryostat_structure"]); 
  } else {
    fTPC->BuildDefalutGeoParMap();
  }
  fTPC->BuildMaterial(fMaterialDBFile);
  G4cerr << "SLArDetectorConstruction::Init Tank DONE" << G4endl;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Initialize Photodetectors
  if (d.HasMember("SuperCell")) {
    G4cout << "SLArDetectorConstruction::Init SuperCells" << G4endl;
    const auto sc = d["SuperCell"].GetObj();
    InitPDS(sc); 
    G4cout << "SLArDetectorConstruction::Init PDS DONE" << G4endl;
  }

  
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Initialize ReadoutTile

  if (d.HasMember("ReadoutTile")) {
    G4cout << "SLArDetectorConstruction::Init Pix..." << G4endl;
    InitPix(d["ReadoutTile"].GetObj()); 
    G4cout << "SLArDetectorConstruction::Init Pix DONE" << G4endl;
  }

  std::fclose(geo_cfg_file);
}

void SLArDetectorConstruction::InitPDS(const rapidjson::Value& pds) {
  fSuperCell = new SLArDetSuperCell(); 
  assert(pds.HasMember("dimensions")); 
  fSuperCell->GetGeoInfo()->ReadFromJSON(pds["dimensions"]); 
  fSuperCell->BuildMaterial(fMaterialDBFile); 

  printf("Building SuperCell configuration object...\n");
  SLArPDSystemConfig* pdsCfg = new SLArPDSystemConfig("PDSCfg"); 
  if (pds.HasMember("modules")) {
    printf("is modules an array?\n");
    assert(pds["modules"].IsArray());
    printf("yes\n"); 
    for (const auto &mdl : pds["modules"].GetArray()) {
      printf("[%i] Module %s\n", mdl["id"].GetInt(), mdl["name"].GetString());

      SLArCfgSuperCellArray* sc_array = 
        new SLArCfgSuperCellArray(mdl["name"].GetString(), mdl["id"].GetInt());
      sc_array->SetIdx(mdl["id"].GetInt()); 

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

          if (isc.HasMember("norm")) {
            assert(isc["norm"].IsArray()); 
            scCfg->SetNormal(
                  isc["norm"].GetArray()[0].GetDouble(),
                  isc["norm"].GetArray()[1].GetDouble(),
                  isc["norm"].GetArray()[2].GetDouble()
                ); 
          }

          sc_array->RegisterElement(scCfg); 
        }
      }

      pdsCfg->RegisterModule(sc_array);
    }
  }


  SLArAnalysisManager::Instance()->LoadPDSCfg(pdsCfg);

}

void SLArDetectorConstruction::InitPix(const rapidjson::Value& pixsys) {
  SLArCfgPixSys* pixCfg = new SLArCfgPixSys("PixCfg");

  fReadoutTile = new SLArDetReadoutTile();
  
  assert(pixsys.HasMember("dimensions")); 
  assert(pixsys.HasMember("components")); 
  assert(pixsys.HasMember("unit_cell")); 

  fReadoutTile->GetGeoInfo()->ReadFromJSON(pixsys["dimensions"]); 
  fReadoutTile->BuildComponentsDefinition(pixsys["components"]); 
  fReadoutTile->BuildUnitCellStructure(pixsys["unit_cell"]); 
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
  fTPC->BuildTPC();
  fTPC->GetModPV("TPC", 0,
      G4ThreeVector(0, 0, 0), 
      fWorldLog, false, 20);

  // 3. Build and place the "conventional" Photon Detection System 
  if (fSuperCell) BuildAndPlaceSuperCells();

  // 4. Build and place the "pixel-based" readout system 
  BuildAndPlaceReadoutTiles(); 

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //Visualization attributes
  fTPC->SetVisAttributes();
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

  //Set ReadoutTile SD
  if (fReadoutTile) {
  G4VSensitiveDetector* sipmSD
    = new SLArReadoutTileSD(SDname="/tile/sipm");
  SDman->AddNewDetector(sipmSD);
  SetSensitiveDetector(
      fReadoutTile->GetSiPMActive()->GetModLV(), 
      sipmSD);
  }

  //Set SuperCell SD
  if (fSuperCell) {
  G4VSensitiveDetector* superCellSD
    = new SLArSuperCellSD(SDname="/supercell"); 
  SDman->AddNewDetector(superCellSD); 
  SetSensitiveDetector(
      fSuperCell->GetCoating()->GetModLV(), 
      superCellSD
      );
  }
  
  // Set LAr-volume SD
  G4VSensitiveDetector* targetSD
    = new SLArLArSD(SDname="/TPC/LAr");
  SDman->AddNewDetector(targetSD);
  SetSensitiveDetector(
      fTPC->GetTarget()->GetModLV(), 
      targetSD);

  ConstructCryostatScorer(); 
}

void SLArDetectorConstruction::ConstructCryostatScorer() {
  G4SDParticleFilter* neutronFilter = new G4SDParticleFilter("neutronFilter"); 
  neutronFilter->add("neutron"); 

  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  G4MultiFunctionalDetector* BPolyethileneSD[2]; 
  G4int iBPltCryostatLayerID[2] = {23, 31};

  for (int i=0; i<2; i++) {
    int ilayer = i+1; 
    BPolyethileneSD[i] = 
      new G4MultiFunctionalDetector("BPolyethilene_"+std::to_string(ilayer)); 
    G4PSTermination* captureCnts = 
      new G4PSTermination("captureCnts"+std::to_string(ilayer)); 
    captureCnts->SetFilter(neutronFilter); 
    BPolyethileneSD[i]->RegisterPrimitive(captureCnts); 

    SDman->AddNewDetector( BPolyethileneSD[i] );  

    G4LogicalVolume* lv_mother = 
      fTPC->GetCryostatStructure()[iBPltCryostatLayerID[i]]->fModule->GetModLV(); 
    auto lv = lv_mother->GetDaughter(0)->GetLogicalVolume();

    SetSensitiveDetector(lv, BPolyethileneSD[i]); 
  }

  G4int iCryostatWallLayerID[2] = {21, 33}; 
  G4MultiFunctionalDetector* CryostatWall[2] = {nullptr}; 
  for (int i=0; i <2; i++) {
    CryostatWall[i] = new G4MultiFunctionalDetector("CryostatWall"+std::to_string(i)); 
    G4PSFlatSurfaceCurrent* scorer = 
      new G4PSFlatSurfaceCurrent("nCurrent"+std::to_string(i), 1);
    scorer->DivideByArea(false); 
    scorer->SetFilter(neutronFilter); 

    CryostatWall[i]->RegisterPrimitive(scorer); 
    SDman->AddNewDetector( CryostatWall[i] ); 
    
    G4LogicalVolume* lv_mother = 
      fTPC->GetCryostatStructure()[iCryostatWallLayerID[i]]->fModule->GetModLV(); 
    auto lv = lv_mother->GetDaughter(0)->GetLogicalVolume();

    SetSensitiveDetector(lv, CryostatWall[i]); 
  }

}

SLArDetTPC* SLArDetectorConstruction::GetDetTPC() 
{
  return fTPC;
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
  fSuperCell->BuildLogicalSkinSurface(); 

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
            Form("SC%i", scinfo.first), rotPMT, 
            pmtPos,
            fTPC->GetTarget()->GetModLV(), 
            false, 
            scinfo.second->GetIdx()
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

  SLArAnaMgr->LoadPDSCfg(pdsCfg); 
  G4cout << "SLArDetectorConstruction::BuildAndPlaceSuperCell DONE" << G4endl;
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

      G4ThreeVector mtile_pos(
          mtileCfg->GetX(), 
          mtileCfg->GetY(), 
          mtileCfg->GetZ()); 
      G4RotationMatrix* mtile_rot = new G4RotationMatrix(
          mtileCfg->GetPhi(), 
          mtileCfg->GetTheta(), 
          mtileCfg->GetPsi());

      auto pv = megatile->GetModPV(mtileCfg->GetName(), 
          mtile_rot, mtile_pos, fTPC->GetTarget()->GetModLV(), 
          false, mtileCfg->GetIdx());
      auto lv  = megatile->GetModLV();
      auto row = megatile->GetTileRow();

      EAxis kRowAxis = kZAxis, kTileAxis = kXAxis;
      G4int nRowReplicas = 0, nTileReplicas = 0;
      G4double wdtRow = 0., wdtTile = 0.;
      G4double oftRow = 0., oftTile = 0.;
      G4bool   cnsRow = kFALSE, cnsTile = kFALSE;

      auto mtile_parameterised = (G4PVParameterised*)lv->GetDaughter(0); 
      if (mtile_parameterised->IsParameterised()) {
          auto parametrization = 
            (SLArDetReadoutPlane::SLArMTileParametrization*)mtile_parameterised->GetParameterisation();
        mtile_parameterised->GetReplicationData(kRowAxis, nRowReplicas, wdtRow, oftRow, cnsRow);
        kRowAxis = parametrization->GetReplicationAxis(); 
        wdtRow  = parametrization->GetSpacing(); 
#ifdef SLAR_DEBUG
        printf("%s is replicated %i times along axis %i (wdt=%g mm, offset= %g mm)\n", 
            pv->GetName().c_str(), nRowReplicas, kRowAxis, wdtRow, oftRow); 
        //getchar(); 

#endif
      } else {
        printf("%s is not replicated! Why??\n", pv->GetName().c_str()); 
        getchar(); 
      }


      auto row_pv = (G4PVParameterised*)row->GetModPV();
      if (row_pv->IsParameterised()) {
        auto parametrization = (SLArDetReadoutPlane::SLArMTileParametrization*)row_pv->GetParameterisation();
        row_pv->GetReplicationData(kTileAxis, nTileReplicas, wdtTile, oftTile, cnsTile);
        wdtTile = parametrization->GetSpacing(); 
        kTileAxis = parametrization->GetReplicationAxis(); 
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
          SLArCfgReadoutTile* tile_cfg = new SLArCfgReadoutTile((ii+1)*100 + jj);
          G4ThreeVector tile_pos(-ii*wdtRow + oftRow, 0., jj*wdtTile + oftTile);
          tile_cfg->SetZ(tile_pos.z()); 
          tile_cfg->SetY(tile_pos.y());
          tile_cfg->SetX(tile_pos.x());
          
          G4ThreeVector phys_pos = mtile_pos 
            + tile_pos.rotate(mtile_rot->getPhi(), mtile_rot->getTheta(), mtile_rot->getPsi()); 
          
          tile_cfg->SetPhysX( (phys_pos).x() ); 
          tile_cfg->SetPhysY( (phys_pos).y() ); 
          tile_cfg->SetPhysZ( (phys_pos).z() );  

#ifdef SLAR_DEBUG
          // TODO: check positioning is ok
          // seems ok, but better check it twice
          // printf("%s megatile pos: [%.2f, %.2f, %.2f]: cell %i pos [%.2f, %.2f, %.2f]: phys pos [%.2f, %.2f, %.2f]\n", 
              //mtileCfg->GetName(), tile_pos.x(), tile_pos.y(), tile_pos.z(), 
              //cell_cfg->GetIdx(), cell_pos.x(), cell_pos.y(), cell_pos.z(),
              //phys_pos.x(), phys_pos.y(), phys_pos.z()
              //);
#endif

          tile_cfg->Set2DSize_X(fReadoutTile->GetGeoPar("tile_z")); 
          tile_cfg->Set2DSize_Y(fReadoutTile->GetGeoPar("tile_x")); 
          tile_cfg->SetNormal(mtileCfg->GetNormal());
          tile_cfg->BuildGShape(); 
          mtileCfg->RegisterElement(tile_cfg); 
        }
        //getchar(); 

      }
    } else {
      G4cerr << "MegaTile model " << mtile_module_name 
        << " is not registered in fReadoutMegaTile!" << G4endl; 
    }
  }

}

