/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArDetectorConstruction.cc
 * @created     mercoledì nov 16, 2022 09:44:58 CET
 */

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

#include "config/SLArCfgSystemPix.hh"
#include "config/SLArCfgBaseSystem.hh"
#include "config/SLArCfgSuperCell.hh"
#include "config/SLArCfgReadoutTile.hh"
#include "config/SLArCfgMegaTile.hh"

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

/**
 * @details Standard constructor of the SLArDetectorConstruction class, 
 * setting the geometry configuration file and the material description table
 *
 * @param geometry_cfg_file Geometry configuration file 
 * @param material_db_file Material description table
 */
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

/**
 * @details Detector initilization. 
 * Parse the geometry configuration of the world and of all the other 
 * detector components. Configuration of the cryostat and LAr target is 
 * passed to fTPC together with the materials table. 
 * The configuration of the readout system is treated by the InitPix 
 * and InitPDS functions. 
 */
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
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
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


  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
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

/**
 * @details Construct the fSuperCell object and parse the supercell 
 * geometry from the pds object described in the geometry configuration
 * file. 
 * After this first step, the method creates a SuperCell system configuration 
 * object and registers all the SuperCell arrays defined in the pds["modules"]
 * section and finally the configuration is registered by the analysis manager
 *
 * @param pds supercell system description
 */
void SLArDetectorConstruction::InitPDS(const rapidjson::Value& pds) {
  fSuperCell = new SLArDetSuperCell(); 
  assert(pds.HasMember("dimensions")); 
  fSuperCell->GetGeoInfo()->ReadFromJSON(pds["dimensions"]); 
  fSuperCell->BuildMaterial(fMaterialDBFile); 

  printf("Building SuperCell configuration object...\n");
  SLArCfgSystemSuperCell* pdsCfg = new SLArCfgSystemSuperCell("PDSCfg"); 
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

      pdsCfg->RegisterElement(sc_array);
    }
  }


  SLArAnalysisManager::Instance()->LoadPDSCfg(pdsCfg);

}

/**
 * @details Parse the description of the pixelated anode readout system. 
 * Build the fReadoutTile object, setup the anode readout configuration
 * according to the description provided in pixsys["modules"] and finally 
 * source the configuration to the analysis manager. 
 *
 * @param pixsys Pixelated anode readout description
 */
void SLArDetectorConstruction::InitPix(const rapidjson::Value& pixsys) {
  SLArCfgSystemPix* pixCfg = new SLArCfgSystemPix("PixCfg");

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
        pixCfg->RegisterElement(mtileCfg);
      } // end of positions loop
    } // end of Megatile models loop
    SLArAnalysisManager::Instance()->LoadPixCfg(pixCfg); 
  } // endif pixsys.HasMember("modules")
}

/**
 * @details Construct the world volume, build and place the 
 * SLArDetectorConstruction::fTPC object. 
 * After this first step, the method calls BuildAndPlaceSuperCells() and
 * BuildAndPlaceReadoutTiles() to place the SuperCell and the Readout Tile
 * detector system. 
 *
 * @return 
 */
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

/**
 * @details Create Sensitive Detector objects for the readout systems 
 * (SLArDetectorConstruction::fReadoutTile, SLArDetectorConstruction::fSuperCell), 
 * for the LAr TPC active volume. The method then calls 
 * SLArDetectorConstruction::ConstructCryostatScorer() to set a fraction of the
 * cryostat wall as Sensitive Detectors to implement some simple scorer used for 
 * background shielding studies.
 */
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

/**
 * @details Construct some scorers to evaluate the cryostat shielding performance. 
 * The method assigns a G4PSTermination scorer
 * to the borated polyethilene layers to count the nr of neutrons stopped in 
 * those volumes. Similarly, a G4PSFlatSurfaceCurrent is assigned to the cryostat
 * outer and inner walls to check the number of neutrons entering the cryostat and
 * the TPC active volume. 
 *
 * TODO: replace the hardcoded CopyIDs of the interested cryostat layers with 
 * a more flaxible solution. 
 */
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


/**
 * @details Build the SLArDetectorConstruction::fSuperCell object and 
 * the photon detector's logical skin surface. 
 * Then place the individual SuperCell according to the configuration 
 * stored in the analysis manager. 
 */
void SLArDetectorConstruction::BuildAndPlaceSuperCells()
{
  fSuperCell->BuildSuperCell();
  fSuperCell->BuildLogicalSkinSurface(); 

  // Get PMTSystem Configuration
  SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
  SLArCfgSystemSuperCell*  pdsCfg = SLArAnaMgr->GetPDSCfg();

  for (auto &pdsArray : pdsCfg->GetMap())
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

/**
 * @details Build the SLArDetectorConstruction::fReadoutTile object and 
 * construct the logical skin surface of the active photon detector volume.
 *
 */
void SLArDetectorConstruction::BuildAndPlaceReadoutTiles() {

  fReadoutTile->BuildReadoutTile(); 
  fReadoutTile->SetVisAttributes();

  fReadoutTile->BuildLogicalSkinSurface(); 

  SLArCfgSystemPix* pixCfg = SLArAnalysisManager::Instance()->GetPixCfg(); 
  
  printf("SLArDetectorConstruction::BuildAndPlaceReadoutTiles\n"); 
  printf("Registered readout tiles models:\n"); 
  for (const auto &models : fReadoutMegaTile) {
    printf("- %s\n", models.first.c_str());
  }
  printf("Nr of readout tiles modules: %lu\n", pixCfg->GetMap().size()); 

  for (auto &mtileCfg_ : pixCfg->GetMap()) {
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
      G4RotationMatrix* mtile_rot_inv = new G4RotationMatrix(*mtile_rot); 
      mtile_rot_inv->invert(); // FIXME: Why do I need to use the inverse rotation????? 

      auto pv = megatile->GetModPV(mtileCfg->GetName(), 
          mtile_rot, mtile_pos, fTPC->GetTarget()->GetModLV(), 
          false, mtileCfg->GetIdx());
      auto lv  = megatile->GetModLV();
      auto row = megatile->GetTileRow();

      EAxis     kTRowAxis = kZAxis, kTileAxis     = kXAxis;
      G4int nTRowReplicas = 0     , nTileReplicas = 0;
      G4double    wdtTRow = 0.    , wdtTile       = 0.;
      G4double    oftTRow = 0.    , oftTile       = 0.;
      G4bool      cnsTRow = kFALSE, cnsTile       = kFALSE;
      G4ThreeVector pos0TRow      ; G4ThreeVector pos0Tile; 
      G4ThreeVector vaxisTRow     ; G4ThreeVector vaxisTile; 

      auto mtile_parameterised = (G4PVParameterised*)lv->GetDaughter(0); 
      if (mtile_parameterised->IsParameterised()) {
          auto parametrization = 
            (SLArDetReadoutPlane::SLArMTileParametrization*)mtile_parameterised->GetParameterisation();
        mtile_parameterised->GetReplicationData(kTRowAxis, nTRowReplicas, wdtTRow, oftTRow, cnsTRow);
        kTRowAxis = parametrization->GetReplicationAxis(); 
        vaxisTRow = parametrization->GetReplicationAxisVector(); 
        wdtTRow   = parametrization->GetSpacing(); 
        pos0TRow  = parametrization->GetStartPos(); 
#ifdef SLAR_DEBUG
        printf("%s is replicated %i times along axis %i (wdt=%g mm, offset= %g mm)\n", 
            pv->GetName().c_str(), nTRowReplicas, kTRowAxis, wdtTRow, oftTRow); 
        //getchar(); 

#endif
      } else {
        printf("%s is not replicated! Why??\n", pv->GetName().c_str()); 
        getchar(); 
      }


      auto mrow_pv = (G4PVParameterised*)row->GetModPV();
      if (mrow_pv->IsParameterised()) {
        auto parametrization = (SLArDetReadoutPlane::SLArMTileParametrization*)mrow_pv->GetParameterisation();
        mrow_pv->GetReplicationData(kTileAxis, nTileReplicas, wdtTile, oftTile, cnsTile);
        wdtTile   = parametrization->GetSpacing(); 
        kTileAxis = parametrization->GetReplicationAxis(); 
        vaxisTile = parametrization->GetReplicationAxisVector(); 
        pos0Tile  = parametrization->GetStartPos(); 
#ifdef SLAR_DEBUG
        printf("%s is replicated %i times along axis %i (wdt=%g mm, offset=%g mm)\n", 
            mrow_pv->GetName().c_str(), nTileReplicas, kTileAxis, 
            wdtTile, oftTile); 
        //getchar(); 
#endif
      } else {
        printf("%s is not replicated! Why??\n", mrow_pv->GetName().c_str()); 
        getchar(); 
      }

      for (int ii =0; ii<nTRowReplicas; ii++) {
        G4ThreeVector pos_row  = pos0TRow + wdtTRow*ii*vaxisTRow; 

        for (int jj=0; jj<nTileReplicas; jj++) {
          G4ThreeVector pos_tile = pos_row + pos0Tile + wdtTile*jj*vaxisTile; 

          SLArCfgReadoutTile* tile_cfg = new SLArCfgReadoutTile((ii+1)*100 + jj);
          G4ThreeVector pos_tile_ = pos_tile; 
          tile_cfg->SetZ(pos_tile.z()); 
          tile_cfg->SetY(pos_tile.y());
          tile_cfg->SetX(pos_tile.x());
          
          G4ThreeVector phys_pos = mtile_pos 
            + pos_tile_.transform(*mtile_rot_inv); 
          
          tile_cfg->SetPhysX( (phys_pos).x() ); 
          tile_cfg->SetPhysY( (phys_pos).y() ); 
          tile_cfg->SetPhysZ( (phys_pos).z() );  

//#ifdef SLAR_DEBUG
          // TODO: check positioning is ok
          // seems ok, but better check it twice
          // printf("%s megatile pos: [%.2f, %.2f, %.2f]: cell %i pos [%.2f, %.2f, %.2f]: phys pos [%.2f, %.2f, %.2f]\n", 
              //mtileCfg->GetName(), tile_pos.x(), tile_pos.y(), tile_pos.z(), 
              //cell_cfg->GetIdx(), cell_pos.x(), cell_pos.y(), cell_pos.z(),
              //phys_pos.x(), phys_pos.y(), phys_pos.z()
              //);
//#endif

          tile_cfg->Set2DSize_X(fReadoutTile->GetGeoPar("tile_z")); 
          tile_cfg->Set2DSize_Y(fReadoutTile->GetGeoPar("tile_x")); 

          // construct pixels in the readout tile configuration
          auto tilesens_lv = fReadoutTile->GetModLV()->GetDaughter(1)->GetLogicalVolume(); 
          auto tilesens_pv = tilesens_lv->GetDaughter(0);  // cell_plane_pv
          G4int n_row = 0; G4int n_cell = 0; 
          G4double crow_x = 0; G4double cell_z = 0.; 
          G4ThreeVector crow_pos0;  G4ThreeVector crow_vaxis;
          G4ThreeVector cell_pos0;  G4ThreeVector cell_vaxis; 


          if (tilesens_pv->IsParameterised()) {
            auto trow_parametrization = (SLArDetReadoutTile::SLArRTileParametrization*)
              tilesens_pv->GetParameterisation(); 
            EAxis axis_ = kUndefined; G4double offset_ = 0.; G4bool cons_;
            tilesens_pv->GetReplicationData(axis_, n_row, crow_x, offset_, cons_); 
            crow_x = trow_parametrization->GetSpacing(); 
            crow_vaxis = trow_parametrization->GetReplicationAxisVector(); 
            crow_pos0 = trow_parametrization->GetStartPos(); 

            // getting cell_row_pv
            auto crow_pv = tilesens_pv->GetLogicalVolume()->GetDaughter(0); 
            if (crow_pv->IsParameterised()) {
              auto crow_parametrization = (SLArDetReadoutTile::SLArRTileParametrization*)
                crow_pv->GetParameterisation(); 
              EAxis caxis_ = kUndefined; G4double coffset_ = 0.; G4bool ccons_;
              crow_pv->GetReplicationData(caxis_, n_cell, cell_z, coffset_, ccons_); 
              cell_z = crow_parametrization->GetSpacing(); 
              cell_vaxis = crow_parametrization->GetReplicationAxisVector(); 
              cell_pos0 = crow_parametrization->GetStartPos();

            } else {
              printf("%s is not a parameterised volume\n", crow_pv->GetName().c_str());
            }
            
            //now build the pixel map
            for (G4int ix=0; ix<n_row; ix++) {
              G4ThreeVector row_pos = crow_pos0 + ix*crow_x*crow_vaxis;
              for (G4int iz=0; iz<n_cell; iz++) {
                G4ThreeVector cell_pos = row_pos + cell_pos0 + iz*cell_z*cell_vaxis; 


                //printf("tile_pos: [%.2f, %.2f, %.2f]\n", pos_tile.x(), pos_tile.y(), pos_tile.z());
                //printf("cell_pos: [%.2f, %.2f, %.2f]\n", cell_pos.x(), cell_pos.y(), cell_pos.z());
                auto cell_structure = fReadoutTile->GetUnitCellPixelMap(); 
                //printf("----------------------------------------------------------------\n");
                for (const auto &cc : cell_structure) {
                    std::vector<SLArCfgReadoutTile::xypoint> xypoints; 
                    //G4ThreeVector pix_pos = cc.fPos + cell_pos + pos_tile; 
                    //G4ThreeVector pix_pos_= pix_pos; pix_pos_.transform(*mtile_rot_inv); 
                    //printf("pix_pos  : (%.2f, %.2f, %.2f)\n", pix_pos .x(), pix_pos .y(), pix_pos .z());
                    //printf("pix_pos_r: (%.2f, %.2f, %.2f)\n", pix_pos_.x(), pix_pos_.y(), pix_pos_.z());
                    //printf("phi_x: %g, theta_x: %g, phi_y = %g, theta_y = %g, phi_z= %g, theta_z = %g\n", 
                        //TMath::RadToDeg()*mtile_rot_inv->phiX(), TMath::RadToDeg()*mtile_rot_inv->thetaX(), 
                        //TMath::RadToDeg()*mtile_rot_inv->phiY(), TMath::RadToDeg()*mtile_rot_inv->thetaY(), 
                        //TMath::RadToDeg()*mtile_rot_inv->phiZ(), TMath::RadToDeg()*mtile_rot_inv->thetaZ());
                    //G4ThreeVector vpoint[5]; 
                    //vpoint[0] = pix_pos + G4ThreeVector(-0.5*pix_x, 0, -0.5*pix_z);
                    //vpoint[1] = pix_pos + G4ThreeVector(-0.5*pix_x, 0,  0.5*pix_z);
                    //vpoint[2] = pix_pos + G4ThreeVector( 0.5*pix_x, 0,  0.5*pix_z);
                    //vpoint[3] = pix_pos + G4ThreeVector( 0.5*pix_x, 0, -0.5*pix_z);
                    //vpoint[4] = pix_pos + G4ThreeVector(-0.5*pix_x, 0, -0.5*pix_z);
                    
                    //for (int ipix = 0; ipix<5; ipix++) {
                      //G4ThreeVector pix_phys = mtile_pos + vpoint[ipix].transform(*mtile_rot_inv);
                      ////printf("pix[%i]: %.2f, %.2f, %.2f mm\n", ipix, pix_phys.x(), pix_phys.y(), pix_phys.z());
                      //SLArCfgReadoutTile::xypoint p = { pix_phys.z(), pix_phys.y() }; 
                      //xypoints.push_back( p ); 
                    //}

                    for (const auto &edge : cc.fEdges) {
                      G4ThreeVector edge_pos = edge + cell_pos + pos_tile; 
                      G4ThreeVector edge_phys = mtile_pos + edge_pos.transform(*mtile_rot_inv); 
                      SLArCfgReadoutTile::xypoint p = {edge_phys.z(), edge_phys.y()}; 
                      xypoints.push_back( p ); 
                    }

                    tile_cfg->AddPixelToHistMap(xypoints); 
                  //printf("----------------------------------------------------------------\n");
                }
              }
            }

          } else {
            printf("%s is not a parameterised volume\n", tilesens_pv->GetName().c_str());
          }

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

  pixCfg->BuildPolyBinHist(); 
}

