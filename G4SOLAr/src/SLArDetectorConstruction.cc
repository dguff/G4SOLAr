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
#include "detector/SLArDetectorSize.hh"

#include "detector/SLArBaseDetModule.hh"
#include "detector/Tank/SLArDetTank.hh"
#include "detector/Tank/SLArDetTankMsgr.hh"
#include "detector/Tank/SLArTankSD.hh"

#include "detector/ReadoutTile/SLArDetReadoutTile.hh"

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

SLArDetectorConstruction::SLArDetectorConstruction()
 : G4VUserDetectorConstruction(), 
   fTank(nullptr), fTankMsgr(nullptr), 
   fSuperCell(nullptr),
   fWorldLog(nullptr)
{ }

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
  SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
  SLArAnaMgr->fAnaMsgr->AssignDetectorConstruction(this);

  // open geometry configuration file
  const char* geo_cfg_file_path = Form("%s/geometry.json", SLAR_BASE_DIR);
  FILE* geo_cfg_file = std::fopen(geo_cfg_file_path, "r");
  char readBuffer[65536];
  rapidjson::FileReadStream is(geo_cfg_file, readBuffer, sizeof(readBuffer));

  rapidjson::Document d;
  d.ParseStream(is);
  assert(d.IsObject());
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Parse world dimensions
  if (d.HasMember("World")) {
    fWorldGeoPars.ReadFromJSON(d, "World");
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
    fTank->GetGeoInfo()->ReadFromJSON(d, "Tank");
  } else {
    fTank->BuildDefalutGeoParMap();
  }
  fTank->BuildMaterial();
  fTankMsgr = new SLArDetTankMsgr(this); 
  G4cerr << "SLArDetectorConstruction::Init Tank DONE" << G4endl;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Initialize Photodetectors
  G4cout << "SLArDetectorConstruction::Init SuperCells" << G4endl;
  
  SLArPDSystemConfig* pdsCfg = new SLArPDSystemConfig("PDSCfg"); 

  fSuperCell = new SLArDetSuperCell(); 

  if (d.HasMember("SuperCell")) {
    const auto sc = d["SuperCell"].GetObj();
    fSuperCell->GetGeoInfo()->ReadFromJSON(d, "SuperCell"); 
    
    if (sc.HasMember("modules")) {
      assert(sc["modules"].IsArray());
      for (const auto &mdl : sc["modules"].GetArray()) {
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

            array->RegisterElement(scCfg); 
          }
        }
        
        pdsCfg->RegisterModule(array);
      }
    }
  }

  SLArAnaMgr->LoadPDSCfg(pdsCfg);
  fSuperCell->BuildMaterial(); 

  G4cout << "SLArDetectorConstruction::Init PDS DONE" << G4endl;
  
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Initialize ReadoutTile
  if (d.HasMember("ReadoutTile")) {
    fReadoutTile = new SLArDetReadoutTile();
    fReadoutTile->GetGeoInfo()->ReadFromJSON(d, "ReadoutTile"); 
    fReadoutTile->BuildMaterial();

    const auto tile = d["ReadoutTile"].GetObj(); 
    if (tile.HasMember("modules")) {
      assert(tile["modules"].IsArray()); 

      for (const auto &mtile : tile["modules"].GetArray()) {
        SLArDetReadoutPlane* megatile = new SLArDetReadoutPlane(); 
        assert(mtile.HasMember("dimensions")); 
        assert(mtile["dimensions"].IsArray()); 
        for (const auto &dim : mtile["dimensions"].GetArray()) {
          const char* unit = dim["unit"].GetString(); 
          const char* name = dim["name"].GetString(); 
          G4double val = dim["val"].GetDouble()*G4UIcommand::ValueOf(unit); 
          G4cout << name << " = " << val << G4endl;
          megatile->SetGeoPar(name, val); 
        }
        megatile->BuildMaterial(); 
        fReadoutMegaTile.push_back(megatile); 
      }
    }
  }

  std::fclose(geo_cfg_file);
}

G4VPhysicalVolume* SLArDetectorConstruction::Construct()
{

// Init modules size
  Init();

//// ------------- Volumes --------------

  G4RotationMatrix* rotY = new G4RotationMatrix();
  rotY->rotateX(pi*0.5);

// The experimental Hall

  G4Box* expHall_box = new G4Box("World", 
      fWorldGeoPars.GetGeoPar("size_x"), 
      fWorldGeoPars.GetGeoPar("size_y"), 
      fWorldGeoPars.GetGeoPar("size_z"));  

 fWorldLog   
    = new G4LogicalVolume(expHall_box, fColl.fGEOWorld.fMaterial, "World",0,0,0);

  G4VPhysicalVolume* expHall_phys
    = new G4PVPlacement(0,G4ThreeVector(),fWorldLog,"World",0,false,0);
  
  // The Cryostat/LAr target
  G4cerr << "\nSLArDetectorConstruction: Building the Tank" << G4endl;
  fTank->BuildTank();
  fTank->GetModPV("Tank", 0,
        G4ThreeVector(0, 0, 0), 
        fWorldLog, false, 20);

  // SuperCell
  BuildAndPlaceSuperCells();

  // ReadoutTile 
  fReadoutTile->BuildReadoutTile(); 
  fReadoutTile->SetVisAttributes();
  for (auto &megatile : fReadoutMegaTile) {
    megatile->BuildReadoutPlane(fReadoutTile); 
    megatile->GetModPV("megatile", new G4RotationMatrix(0., 0., -0.5*pi), 
        G4ThreeVector(+1.87*CLHEP::m, 0., 0.), fTank->GetModLV(), false, 1000); 
  }
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //Visualization attributes

  //fTank->SetVisAttributes();
  //fSuperCell->SetVisAttributes();

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
  
  //fParser.Write("beamCell_v0.gdml", expHall_phys);

  //always return the physical World
  return expHall_phys;
}

void SLArDetectorConstruction::ConstructSDandField()
{
    // sensitive detectors 
    
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    G4String SDname;
    
    //Set PMT SD
    //G4VSensitiveDetector* pmtSD
      //= new SLArPMTSD(SDname="/PMT/Cathode");
    //SDman->AddNewDetector(pmtSD);
    //for (auto &pmt : fPMTs)
      //SetSensitiveDetector(
          //pmt.second->GetCathode()->GetModLV(), 
          //pmtSD);

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
  G4cout << "\nSLArDetectorConstruction:BuildSuperCell" << G4endl;
  fSuperCell->BuildSuperCell();

  new G4LogicalSkinSurface("SCCoating_skin", 
      fSuperCell->GetCoating()->GetModLV(), 
      fSuperCell->GetCoatingMaterial()->
      GetMaterialBuilder()->GetSurface()
      );

  G4cout << "\nSLArDetectorConstruction:PlaceSuperCells" << G4endl;

  // Get PMTSystem Configuration
  G4cout << "Getting BCAnaManager" << G4endl;
  SLArAnalysisManager* SLArAnaMgr  = SLArAnalysisManager::Instance();
  SLArPDSystemConfig*  pdsCfg = SLArAnaMgr->GetPDSCfg();
  G4cout << "Getting PDS Cfg (" << pdsCfg->GetName() << ")" << G4endl;


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
      scinfo.second->SetPhysX(pmtPos[0]);
      scinfo.second->SetPhysY(pmtPos[1]);
      scinfo.second->SetPhysZ(pmtPos[2]);
      iSC++;
    }

    //break;

  }
   G4cout << "SLArDetectorConstruction::BuildAndPlacePMTs DONE" << G4endl;
  return;
}

//int SLArDetectorConstruction::RemovePMTs() 
//{

  //int npmts = fPMTPV.size();
  //G4cout << "SLArDetectorConstruction::RemovePMTs:" << G4endl;
  //G4cout << "Removing " << npmts << " extisting PMTs\n" << G4endl;

  //if (npmts > 0) {
    //for (auto &&pmtpv : fPMTPV) {
      //G4LogicalVolume* target = pmtpv->GetLogicalVolume();
      //target->RemoveDaughter( pmtpv );
      //delete pmtpv; pmtpv = nullptr;
    //}
  //}
  //// clear vector of physical volume
  //fPMTPV.clear();
  
  //// clear map of SLArDetPMTs used in previous cfg
  /*
   *  for (auto &detPmt : fPMTs)
   *    if (detPmt.second) 
   *    {delete detPmt.second; detPmt.second = nullptr;}
   *  fPMTs.clear();
   *
   */
  //G4RunManager::GetRunManager()->GeometryHasBeenModified();

  //return npmts;
//}

//int SLArDetectorConstruction::RemoveHodoModules() 
//{

  //int nmod = fHodoModulePV.size();
  //G4cout << "SLArDetectorConstruction::RemoveHodoPlanes:" << G4endl;
  //G4cout << "Removing " << nmod << " extisting Hodo planes\n" 
         //<< G4endl;

  //if (nmod > 0) {
    //for (auto &&module : fHodoModulePV) {
      //G4LogicalVolume* target = module->GetLogicalVolume();
      //target->RemoveDaughter( module );
      //delete module; module = nullptr;
    //}
  //}
  //// clear vector of physical volume
  //fHodoModulePV.clear();
  
  //G4RunManager::GetRunManager()->GeometryHasBeenModified();

  //return nmod;
//}

//void SLArDetectorConstruction::BuildPMTModel(const char* mod)
//{
  //SLArDetPMT* pmt = new SLArDetPMT();
  //pmt->LoadPMTModel(mod);
  //pmt->BuildPMT();
  //fPMTs.insert(std::make_pair(mod, pmt));
//}

//void SLArDetectorConstruction::SetLAPPD(bool kLAPPD)
//{
  //fIsLAPPD = kLAPPD;
//}

//bool SLArDetectorConstruction::IsLAPPD()
//{
  //return fIsLAPPD;
//}

//std::vector<G4VPhysicalVolume*>& SLArDetectorConstruction::GetVecPMT()
//{
  //return fPMTPV;
//}
