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

#include "SLArUserPath.hh"
#include "SLArAnalysisManager.hh"
//#include "config/SLArCfgHodoModule.hh"
//#include "config/SLArSystemConfigPMT.hh"

#include "SLArDetectorConstruction.hh"
#include "detector/SLArDetectorSize.hh"

#include "detector/SLArBaseDetModule.hh"
#include "detector/Tank/SLArDetTank.hh"
#include "detector/Tank/SLArDetTankMsgr.hh"
#include "detector/Tank/SLArTankSD.hh"

//#include "detector/LAPPD/SLArDetLAPPD.hh"
//#include "detector/LAPPD/SLArDetLAPPDMsgr.hh"
//#include "detector/LAPPD/SLArLAPPDSD.hh"
//#include "detector/LAPPD/SLArLAPPDHit.hh"

//#include "detector/PMT/SLArDetPMT.hh"
//#include "detector/PMT/SLArDetPMTMsgr.hh"
//#include "detector/PMT/SLArPMTSD.hh"
//#include "detector/PMT/SLArPMTHit.hh"

//#include "detector/Hodoscope/SLArDetHodoscope.hh"
//#include "detector/Hodoscope/SLArHodoscopeHit.hh"
//#include "detector/Hodoscope/SLArHodoscopeSD.hh"


#include "G4GDMLParser.hh"
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
#include "G4SystemOfUnits.hh"

#include <fstream>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/encodings.h"
#include "rapidjson/stringbuffer.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArDetectorConstruction::SLArDetectorConstruction()
 : G4VUserDetectorConstruction(), 
   fTank(nullptr), fTankMsgr(nullptr), 
   fRotPMTBDwnStr(nullptr), fRotPMTBTop(nullptr),
   fRotPMTBBottom(nullptr), fRotPMTBLeft(nullptr),
   fRotPMTBRight(nullptr),
   //fLAPPD(nullptr), fLAPPDMsgr(nullptr), 
   fWorldLog(nullptr)
{
  //fIsLAPPD      = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArDetectorConstruction::~SLArDetectorConstruction(){
  G4cerr << "Deleting SLArDetectorConstruction... " << G4endl;
  for (G4int i=0; i<G4int(fVisAttributes.size()); ++i) 
  {
    delete fVisAttributes[i];
  }
  //if (fPMTMsgr     ) delete fPMTMsgr  ;
  
/*  Let Geant4 to deal with destructing volumes...
 *  for (auto &pmt : fPMTs) 
 *  { if (pmt.second) {delete pmt.second; pmt.second = nullptr;} }
 *  fPMTs.clear();
 *
 *  for (auto &hodo : fHodoscopes) 
 *  { if (hodo.second) {delete hodo.second; hodo.second = nullptr;} }
 *  fHodoscopes.clear();
 *
 *  if (fTank     )    delete fTank;
 *  if (fTankMsgr )    delete fTankMsgr;
 *  if (fLAPPD    )    delete fLAPPD;
 *  if (fLAPPDMsgr)    delete fLAPPDMsgr;
 *  if (fRotPMTtop)    delete fRotPMTtop;
 *  if (fRotPMTbottom) delete fRotPMTbottom;
 *
 */
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
  // Initialize Tank objects
  G4cerr << "SLArDetectorConstruction::Init Tank" << G4endl;
  fTank     = new SLArDetTank();
  if (d.HasMember("Tank")) {
    const auto target = d["Tank"].GetObj(); 
    assert(target.HasMember("dimensions"));
    assert(target["dimensions"].IsArray());

    const auto dimensions = target["dimensions"].GetArray();
    for (const auto &xx : dimensions) {
      const auto entry = xx.GetObj(); 
      const char* name = entry["name"].GetString();
      const char* unit = entry["unit"].GetString();
      G4double val = entry["val"].GetFloat() * G4UIcommand::ValueOf(unit);
      fTank->GetGeoInfo()->RegisterGeoPar(name, val);
    }
  } else {
    fTank->BuildDefalutGeoParMap();
  }
  fTank->BuildMaterial();
  fTankMsgr = new SLArDetTankMsgr(this); 
  // retrieve tank dimensions
  G4cerr << "SLArDetectorConstruction::Init Tank DONE" << G4endl;

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Initialize Photodetectors
  //G4cout << "SLArDetectorConstruction::Init PMTs" << G4endl;
  //fRotPMTBDwnStr = new G4RotationMatrix();
  //fRotPMTBTop    = new G4RotationMatrix();
  //fRotPMTBDwnStr->rotateY(+1.00*pi);
  //fRotPMTBTop   ->rotateX(-0.50*pi);
  //// Load PMT maps and build PMT models
  //// Get SLArAnlysisManager and create PMT cfg
  //G4String pmtMapPath = 
    //"./../WbLSBeamCell/BeamCellMC/PMTModels/";
  //SLArSystemConfigPMT* SLArPMTSysCfg = 
    //new SLArSystemConfigPMT("PMTSystemConfig");

  //// create and register array cfgs
  //SLArCfgPMTArray* PMTSLArfgArray[2];
  //double    rotArray[2] = {0            ,  0.5*pi};
  //G4String nameArray[2] = {"BDownstream",  "BTop"};
  //G4String mapPath  [2] = {"map_face_v0.txt", "map_side_v0.txt"};
  //G4ThreeVector mapPlanePos[2] = {
    //G4ThreeVector(0, 
        //-0.5*tnkExtraTop, 
        //innerTnkLenght*0.5-tnkExtraDwnstr+acrThck+1*cm),
    //G4ThreeVector(0, 
        //innerTnkHeight*0.5-tnkExtraTop, 
        //-0.5*tnkExtraDwnstr)};
  //G4ThreeVector mapPlaneRot[2] = {
    //G4ThreeVector(0.0*pi, 0.0*pi, 0.0*pi), 
    //G4ThreeVector(0.5*pi, 0.0*pi, 0.0*pi)};

  //for (int i=0; i<2; i++)
  //{
    //PMTSLArfgArray[i] = new SLArCfgPMTArray(nameArray[i], 1+i);
    //PMTSLArfgArray[i]->LoadPMTMapAscii(
          //pmtMapPath+mapPath[i], nameArray[i], rotArray[i]);
    //PMTSLArfgArray[i]->SetPMTArrayPosition(
        //mapPlanePos[i].x(), mapPlanePos[i].y(), mapPlanePos[i].z());
    //PMTSLArfgArray[i]->SetArrayRotAngle(
        //mapPlaneRot[i].x(), mapPlaneRot[i].y(), mapPlaneRot[i].z());
    
    //// scan PMTs and build when find a new model
    //for (auto &pmtinfo : PMTSLArfgArray[i]->GetMap())
    //{
      //if (!fPMTs.count(pmtinfo.second->GetModel())) 
      //{
        //G4cout <<"\tBuilding model: " << pmtinfo.second->GetModel() 
        //<< G4endl;
        //BuildPMTModel(pmtinfo.second->GetModel());
        //G4String skinName = pmtinfo.second->GetModel();
        //skinName += "PMTCathode_surf";
        //new G4LogicalSkinSurface(skinName, 
            //fPMTs.find(pmtinfo.second->GetModel())->
            //second->GetCathode()->GetModLV(), 
            //fPMTs.find(pmtinfo.second->GetModel())->
            //second->GetCathodeMaterial()->
            //GetMaterialBuilder()->GetSurface()
            //);
      //}
    //}

    //SLArPMTSysCfg->RegisterArray(PMTSLArfgArray[i]);
  //}

  //SLArAnaMgr->LoadPMTCfg(SLArPMTSysCfg);

  
  //G4cout << "SLArDetectorConstruction::Init PMTs DONE" << G4endl;

  /*
   * // Initialize Hodoscope
   *G4cout << "SLArDetectorConstruction::Init Hodoscope" << G4endl;
   *fHodoscopes.insert(
   *    std::make_pair("MiniTrkPlane", 
   *    new SLArDetHodoscope("MiniTrkPlane")));
   *SLArDetHodoscope* MiniTrkPlane = fHodoscopes.find("MiniTrkPlane")->second;
   *MiniTrkPlane->BuildGeoParMap(19*cm, 1.9*cm, 1.5*cm);
   *MiniTrkPlane->BuildMaterial();
   *MiniTrkPlane->SetNBars(10);
   */

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

  G4Box* expHall_box = new G4Box("World", 1.5*m, 1.5*m, 4*m);
  //G4Box* expHall_box = new G4Box("World", 0.5*m, 0.5*m, 0.5*m);

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

  // PMTs
  //BuildAndPlacePMTs();

  //DumpPMTMap("./output/pmtMapFile.txt");

/*
 *  G4String model = "ETL9351_bare";
 *  BuildPMTModel(model);
 *  SLArDetPMT* pmt = fPMTs.find(model)->second;
 *  printf("Theta Angle: %.2f \n", 
 *      pmt->GetGlass()->GetGeoPar("PMTGlassTheta"));
 *  fPMTPV.push_back(  
 *      pmt->GetModPV(
 *        Form("ETL_PMT%i", 0), 0, 
 *        G4ThreeVector(0, 0, 0),
 *        fWorldLog, false, 666
 *        )
 *      );
 *
 */
 
  // Hodoscopes
  //BuildAndPlaceHodoscope();

 // ------------- Surfaces --------------
 //
 // Water Tank
 //
 //  G4OpticalSurface* opWaterSurface = new G4OpticalSurface("WaterSurface");
 //  opWaterSurface->SetType(dielectric_dielectric);
 //  opWaterSurface->SetFinish(ground);
 //  opWaterSurface->SetModel(unified);
   //opWaterSurface->SetType(dielectric_LUTDAVIS);
   //opWaterSurface->SetFinish(Rough_LUT);
   //opWaterSurface->SetModel(DAVIS);
 
   //G4LogicalBorderSurface* waterSurface =
           //new G4LogicalBorderSurface("WaterSurface",
                                  //Liquid_phys,expHall_phys,opWaterSurface);
 
   //G4OpticalSurface* opticalSurface = dynamic_cast <G4OpticalSurface*>
         //(waterSurface->GetSurface(Liquid_phys,expHall_phys)->
                                                        //GetSurfaceProperty());
   //if (opticalSurface) opticalSurface->DumpInfo();
 
 //
 // Generate & Add Material Properties Table attached to the optical surfaces
 //
   
   //  opWaterSurface->SetMaterialPropertiesTable(myST1);
 
   //OpticalAirSurface
   //G4double reflectivity[num] = {0.3, 0.5};
   //G4double efficiency[num]   = {0.8, 1.0};
 
   //G4MaterialPropertiesTable *myST2 = new G4MaterialPropertiesTable();
 
   //myST2->AddProperty("REFLECTIVITY", ephoton, reflectivity, num);
   //myST2->AddProperty("EFFICIENCY",   ephoton, efficiency,   num);
 
   //G4cout << "Air Surface G4MaterialPropertiesTable" << G4endl;
   //myST2->DumpTable();
 
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //Visualization attributes

  fTank->SetVisAttributes();
  //for (auto &pmt : fPMTs) pmt.second->SetVisAttributes();
  //for (auto &hodo : fHodoscopes) hodo.second->SetVisAttributes();

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

    // Set Hodoscope SD
    //if (fHodoscopes.size())
    //{
      //G4VSensitiveDetector* hodoSD
        //= new SLArHodoSD(SDname="/Hodoscope/Bar");
      //SDman->AddNewDetector(hodoSD);
      //for (auto &hodo : fHodoscopes)
        //SetSensitiveDetector(
            //hodo.second->GetBar()->GetModLV(), 
            //hodoSD 
            //);
    //}

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

//SLArDetLAPPD* SLArDetectorConstruction::GetDetLAPPD() 
//{
  //return fLAPPD;
//}


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

//std::map<G4String,SLArDetHodoscope*>& SLArDetectorConstruction::GetDetHodoscopes()
//{
  //return fHodoscopes;
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

//void SLArDetectorConstruction::LoadPMTMap(G4String path)
//{
  //fPMTMapPath = path;

  //std::ifstream mapfile;
  //mapfile.open(path);

  //if (!mapfile.is_open())
  //{
    //G4cerr<<"Map file in "<<fPMTMapPath<<" not opened!"<<G4endl;
    //return;
  //}
  //else
  //{
    //G4String cline;
    //while (std::getline(mapfile, cline))
    //{
      //G4String c0 = GetFirstChar(cline);
      //if (!c0.contains("#"))
      //{
        //std::stringstream stream(cline);
        //int idx; double x, y;
        //G4cout << "Stream: " << stream.str() << G4endl;
        //stream >> idx >> x >> y;
        //fPMTMap.push_back( PMTGeoInfo(x*cm, y*cm, idx) );  
      //}
    //}  
  //}

  //mapfile.close();
  ////Print map:
  ////for (auto &xy : fPMTMap)
    ////printf("idx: %i\t%.3f\t%.3f\n", xy.fNum, xy.fX, xy.fY);

  //return;
//}

//void SLArDetectorConstruction::DumpPMTMap(G4String path)
//{
  //std::ofstream outputFile;
  //outputFile.open(path);

  //if (!outputFile.is_open()) 
  //{
    //G4cout<<"SLArDetectorConstruction::DumpPMTMap Output file " 
          //<<path<<" not opened" << G4endl;
    //return;
  //}
  //else {
    //G4cout<<"SLArDetectorConstruction::DumpPMTMap"  << G4endl; 
    //G4cout<<(int)fPMTMap.size()<<" PMT registered"<< G4endl; 
  

    //for (int i=0; i<(int)fPMTMap.size(); i++) 
    //{
      //outputFile << "idx: " << fPMTMap.at(i).fNum       << "\t" 
                 //<< "xc: "  << fPMTMap.at(i).fX   / cm  << "\t" 
                 //<< "yc: "  << fPMTMap.at(i).fY   / cm  << "\n";
    //}
  //}
  //outputFile.close();
  //return;
//}

//void  SLArDetectorConstruction::BuildAndPlaceHodoscope()
//{
  //G4cout << "\nSLArDetectorConstruction:BuildAndPlaceHodoscope" 
         //<< G4endl;

  //SLArSystemConfigHodo* HodoSysCfg = 
    //new SLArSystemConfigHodo("HodoConfig");

  //SLArDetHodoscope* detHodo = fHodoscopes.find("MiniTrkPlane")->second;
  //detHodo->BuildHodoPlane();
  //detHodo->GetGeoInfo()->DumpParMap();

  //G4PVReplica * barReplica = detHodo->GetReplicaBar();
  //EAxis         barRepAxis = kZAxis;
  //G4double      barWidth, barOffset;
  //G4int         nBars   = 0;
  //G4bool        SLAronsuming; 
  //barReplica->GetReplicationData(barRepAxis, nBars, 
      //barWidth, barOffset, SLAronsuming);

  //G4Box*        hodoBox    = (G4Box*)detHodo->GetModSV();
  //G4double      hodoHalfZ  = hodoBox->GetZHalfLength();

  //SLArCfgHodoModule* cfgModule = nullptr;

  //auto placeHodoPlane = [&] 
    //(int idx, G4ThreeVector pos, G4RotationMatrix* rot)
    //{
      //G4int iHodoSeries = 5000 + idx;
      //G4String planeName = "MiniTrkPlane"+std::to_string(idx);

      //printf("placing module %s at y = %.2f\n", 
          //planeName.c_str(), pos.getY());

      //fHodoModulePV.push_back(
          //detHodo->GetModPV(planeName, 
            //rot, pos, fWorldLog, false, iHodoSeries)
      //);

      //cfgModule = new SLArCfgHodoModule(iHodoSeries);
      //cfgModule->SetName(planeName);
      //cfgModule->SetVerticalPos(pos.getY());
      //for (G4int i=0; i<nBars; i++)
      //{
        //SLArCfgHodoBar* hodobar = new SLArCfgHodoBar(i);
        //hodobar->SetCenter(pos.getX(), (2*i+1)*barWidth*0.5-hodoHalfZ);
        //hodobar->SetLength(detHodo->GetBarLength());
        //hodobar->SetWidth (detHodo->GetBarWidth ());
        //hodobar->BuildGShape();
        //cfgModule->RegisterBar((SLArCfgHodoBar*)hodobar->
            //Clone(Form("hodobar%i", i)));
      //}
      //cfgModule->SetTH2BinIdx();
      //HodoSysCfg->RegisterModule(cfgModule);
    //};

  //G4RotationMatrix* rotHodo = new G4RotationMatrix();
  //rotHodo->rotateX(pi*0.5);

  //G4ThreeVector pos = G4ThreeVector(0, 0,
      //-0.5*fTank->GetGeoPar("TargetLength")+
      //-detHodo->GetGeoInfo()->GetGeoPar("HodoBarHeight")*9-10*cm);

  //for (G4int ip=0; ip<8; ip++)
  //{
    //pos[2] += detHodo->GetGeoInfo()->GetGeoPar("HodoBarHeight");

    //if (ip==4) pos[2] += 3*cm;
  
    //rotHodo->rotateZ(0.5*pi);

    //printf("calling placeHodoPlane at y = %.2f\n", pos.getY());
    //placeHodoPlane(ip+1, pos, rotHodo);
  //}
  
  //SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
  //SLArAnaMgr->LoadHodoCfg(HodoSysCfg);
//}

//void SLArDetectorConstruction::BuildAndPlaceLAPPD()
//{
  //G4cout << "\nSLArDetectorConstruction:BuildAndPlaceLAPPD" << G4endl;
  //fLAPPD->BuildLAPPD();

  //fLAPPD->GetModPV("LAPPD", 0, 
      //G4ThreeVector(0, 
        //-0.5*(fTank->GetGeoPar("BarrelHeight") + 
          //fLAPPD->GetTotalHeight()), 0), 
      //fWorldLog, false, 30);

  //G4cout << "Build Photocathode logical skin" << G4endl;
  //G4cout << "Cathode name: " << 
    //fLAPPD->GetCathode()->GetModLV()->GetName() << G4endl;
  ////G4cout << "Surface properties" << G4endl;
  ////fLAPPD.GetCathodeMaterial()->GetMaterialBuilder()->
  ////GetSurface()->DumpInfo();

  //new G4LogicalSkinSurface("LAPPDCathode_surf", 
      //fLAPPD->GetCathode()->GetModLV(), 
      //fLAPPD->GetCathodeMaterial()->GetMaterialBuilder()->GetSurface()
      //);
//}

//void SLArDetectorConstruction::BuildAndPlaceLAPPD(
    //G4RotationMatrix* rot, const G4ThreeVector& pos, 
    //G4String         name, G4int             copyNo)
//{
  //G4cout << "\nSLArDetectorConstruction:BuildAndPlaceLAPPD" << G4endl;
  //G4cout << ">\tBuilding LAPPD" << G4endl;
  //fLAPPD->BuildLAPPD();

  //fLAPPD->GetModPV(name, rot, pos, fWorldLog, false, copyNo);

  //G4cout << "Build Photocathode logical skin" << G4endl;
  //G4cout << "Cathode name: " << 
    //fLAPPD->GetCathode()->GetModLV()->GetName() << G4endl;
  ////G4cout << "Surface properties" << G4endl;
  ////fLAPPD.GetCathodeMaterial()->GetMaterialBuilder()->
  ////GetSurface()->DumpInfo();

  //new G4LogicalSkinSurface("LAPPDCathode_surf", 
      //fLAPPD->GetCathode()->GetModLV(), 
      //fLAPPD->GetCathodeMaterial()->GetMaterialBuilder()->GetSurface()
      //);
//}


//void SLArDetectorConstruction::BuildAndPlacePMTs()
//{
  //G4cout << "\nSLArDetectorConstruction:PlacePMTs" << G4endl;
  //// Get PMTSystem Configuration
  //G4cout << "Getting SLArAnaManager" << G4endl;
  //SLArAnalysisManager* SLArAnaMgr  = SLArAnalysisManager::Instance();
  //SLArSystemConfigPMT* pmtSysCfg = SLArAnaMgr->GetPMTCfg();
  //G4cout << "Getting PMT Cfg (" << pmtSysCfg->GetName() << ")" << G4endl;

  //for (auto &pmtArray : pmtSysCfg->GetArrayMap())
  //{
    //SLArCfgPMTArray* arrayCfg = pmtArray.second;
    //G4cout << arrayCfg->GetName() <<" map: " 
      //<< arrayCfg->GetMap().size() << " entries" << G4endl;
    ////arrayCfg->DumpMap();

    //int iPmt = 0;

    //for (auto &pmtinfo : arrayCfg->GetMap())
    //{
      //if (!fPMTs.count(pmtinfo.second->GetModel())) 
      //{
        //G4cout <<"\tBuilding model: " << pmtinfo.second->GetModel() 
        //<< G4endl;
        //BuildPMTModel(pmtinfo.second->GetModel());
        //G4String skinName = pmtinfo.second->GetModel();
        //skinName += "PMTCathode_surf";
        //new G4LogicalSkinSurface(skinName, 
            //fPMTs.find(pmtinfo.second->GetModel())->
            //second->GetCathode()->GetModLV(), 
            //fPMTs.find(pmtinfo.second->GetModel())->
            //second->GetCathodeMaterial()->
            //GetMaterialBuilder()->GetSurface()
            //);
      //}

      ////G4cout << "Getting PMT model info...";
      //SLArDetPMT* pmt = nullptr;
      //G4RotationMatrix* rotPMT = nullptr;
      //pmt = fPMTs.find(pmtinfo.second->GetModel())->second;
      //G4double pmtShft = 0.0;
      //if (!strcmp(pmtArray.second->GetName(), "BDownstream"))
      //{
        //pmtShft = 0.5*pmt->GetGeoPar("PMTCaseHeight");
        //if (pmt->GetModel()->GetModelName().contains("cone"))
        //{
          //pmtShft = 0.5*pmt->GetGeoPar("PMTCaseHeight") +
            //pmt->GetGeoPar("PMTConeHeight") + 
            //pmt->GetGeoPar("PMTCurvatureRadius") -
            //pmt->GetGeoPar("PMTGlassQ");
        //}
      //}
      //else if (!strcmp(pmtArray.second->GetName(), "BTop"))
      //{
        //if (pmt->GetModel()->GetModelName().contains("ETL"))
        //{
          //pmtShft+=(
              //pmt->GetGeoPar("PMTCaseHeight")*0.5  +
              //pmt->GetGeoPar("PMTGlassConeHeight") +
              //pmt->GetGeoPar("PMTGlassZaxis")
              //);
        //}
      //}
      //double kk = +1;

      //if      (!strcmp(arrayCfg->GetName(),"BDownstream"))
      //{rotPMT   = fRotPMTBDwnStr; kk = +1;}
      //else if (!strcmp(arrayCfg->GetName(),"BTop"))
      //{rotPMT   = fRotPMTBTop; kk = -1;}
      //else if (!strcmp(arrayCfg->GetName(),"BBottom"))
      //{rotPMT   = fRotPMTBBottom; kk = +1;}
      //else if (!strcmp(arrayCfg->GetName(),"BLeft"))
      //{rotPMT   = fRotPMTBLeft; kk = -1;}
      //else if (!strcmp(arrayCfg->GetName(),"BRight"))
      //{rotPMT   = fRotPMTBRight; kk = +1;}

      //G4ThreeVector planePos(
          //pmtinfo.second->GetX(), 
          //pmtinfo.second->GetY(),
          //pmtinfo.second->GetZ()+kk*pmtShft
          //);
      //G4ThreeVector basePos(
          //arrayCfg->GetPMTArrayPosition()[0], 
          //arrayCfg->GetPMTArrayPosition()[1], 
          //arrayCfg->GetPMTArrayPosition()[2] 
          //);

      //planePos.rotateX(arrayCfg->GetRotationAngle()[0]);
      //planePos.rotateY(arrayCfg->GetRotationAngle()[1]);
      //planePos.rotateZ(arrayCfg->GetRotationAngle()[2]);

      //G4ThreeVector pmtPos = basePos + planePos; 
      ////G4cout << "PMT #" << pmtinfo.second->GetIdx() << G4endl;
      ////G4cout << "basePos  = " << basePos  << G4endl;
      ////G4cout << "planePos = " << planePos << G4endl;
      ////G4cout << "pre-rotation: " << planePos << G4endl;
      ////G4cout << "post-rotation: " << planePos << "\n"<< G4endl;
      ////getchar();

      //fPMTPV.push_back(  
          //pmt->GetModPV(
            //Form("PMT%i", iPmt), rotPMT, 
            //pmtPos,
            //fWorldLog, false, pmtinfo.second->GetIdx()
            //)
          //);
      //// set physical placement in pmt cfg
      //pmtinfo.second->SetPhysX(pmtPos[0]);
      //pmtinfo.second->SetPhysY(pmtPos[1]);
      //pmtinfo.second->SetPhysZ(pmtPos[2]);
      //iPmt++;
    //}

    ////break;

  //}
   //G4cout << "SLArDetectorConstruction::BuildAndPlacePMTs DONE" << G4endl;
  //return;
//}

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
