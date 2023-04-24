/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArDetectorConstruction.cc
 * @created     Wed Nov 16, 2022 09:44:58 CET
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

#include "detector/Anode/SLArDetReadoutTile.hh"
#include "detector/Anode/SLArReadoutTileSD.hh"

#include "detector/SuperCell/SLArDetSuperCellArray.hh"
#include "detector/SuperCell/SLArSuperCellSD.hh"

#include "config/SLArCfgAnode.hh"
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
    fWorldGeoPars.ReadFromJSON(wrld["dimensions"].GetArray());
  } else {
    fWorldGeoPars.RegisterGeoPar("size_x", 5*CLHEP::m); 
    fWorldGeoPars.RegisterGeoPar("size_y", 8*CLHEP::m); 
    fWorldGeoPars.RegisterGeoPar("size_z",20*CLHEP::m); 
  }


  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Initialize TPC objects
  G4cerr << "SLArDetectorConstruction::Init TPC" << G4endl;
  InitTPC(d["TPC"]); 
  InitCathode(d["Cathode"]);
  ConstructTarget(); 

  fCryostat = new SLArDetCryostat(); 
  fCryostat->SetGeoPar( "target_x", fDetector->GetGeoPar("det_x") ); 
  fCryostat->SetGeoPar( "target_y", fDetector->GetGeoPar("det_y") ); 
  fCryostat->SetGeoPar( "target_z", fDetector->GetGeoPar("det_z") ); 

  if (d.HasMember("Cryostat")) {
    fCryostat->BuildCryostatStructure(d["Cryostat"]);
    G4cerr << "SLArDetectorConstruction::Init Cryostat DONE" << G4endl;
  }


  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Initialize Photodetectors
  if (d.HasMember("SuperCell") && d.HasMember("PhotoDetectionSystem")) {
    G4cout << "SLArDetectorConstruction::Init SuperCells" << G4endl;
    InitSuperCell( d["SuperCell"].GetObj() ); 
    G4cout << "SLArDetectorConstruction::Init SuperCells DONE" << G4endl;
    InitPDS(d["PhotoDetectionSystem"]); 
    G4cout << "SLArDetectorConstruction::Init PDS DONE" << G4endl;
  }

  
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // Initialize ReadoutTile
  if (d.HasMember("ReadoutTile") && d.HasMember("Anode")) {
    G4cout << "SLArDetectorConstruction::Init Pix..." << G4endl;
    InitReadoutTile(d["ReadoutTile"].GetObj()); 
    InitAnode(d["Anode"]); 
    G4cout << "SLArDetectorConstruction::Init Pix DONE" << G4endl;
  }

  std::fclose(geo_cfg_file);
}

void SLArDetectorConstruction::InitTPC(const rapidjson::Value& jtpc) {
  assert(jtpc.IsArray()); 

  //loop over TPC modules
  for (auto &tpc : jtpc.GetArray()) {
    SLArDetTPC* detTPC = new SLArDetTPC(); 
    detTPC->Init(tpc); 
    fTPC.insert(std::make_pair(detTPC->GetID(), detTPC)); 
  }

}

void SLArDetectorConstruction::InitCathode(const rapidjson::Value& jcathode) {
  assert(jcathode.IsArray()); 

  for (const auto &jcath : jcathode.GetArray()) {
    SLArDetCathode* detCathode = new SLArDetCathode(); 
    detCathode->Init(jcath); 
    fCathode.insert(std::make_pair(detCathode->GetID(), detCathode)); 
  }
} 

void SLArDetectorConstruction::InitSuperCell(const rapidjson::Value& jsupercell) {
  fSuperCell = new SLArDetSuperCell(); 
  assert(jsupercell.HasMember("dimensions")); 
  fSuperCell->GetGeoInfo()->ReadFromJSON(jsupercell["dimensions"].GetArray()); 
  return;
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
void SLArDetectorConstruction::InitPDS(const rapidjson::Value& jconf) {

  assert(jconf.IsArray()); 

  for (const auto &jarray : jconf.GetArray()) {
    SLArDetSuperCellArray* detSCArray = new SLArDetSuperCellArray(); 
    detSCArray->Init(jarray); 
    fSCArray.insert( std::make_pair(detSCArray->GetID(), detSCArray) ); 
  }
  
  return;
}

/*
 *  printf("Building SuperCell configuration object...\n");
 *  SLArCfgSystemSuperCell* pdsCfg = new SLArCfgSystemSuperCell("PDSCfg"); 
 *  if (pds.HasMember("modules")) {
 *    printf("is modules an array?\n");
 *    assert(pds["modules"].IsArray());
 *    printf("yes\n"); 
 *    for (const auto &mdl : pds["modules"].GetArray()) {
 *      printf("[%i] Module %s\n", mdl["id"].GetInt(), mdl["name"].GetString());
 *
 *      SLArCfgSuperCellArray* sc_array = 
 *        new SLArCfgSuperCellArray(mdl["name"].GetString(), mdl["id"].GetInt());
 *      sc_array->SetIdx(mdl["id"].GetInt()); 
 *
 *      if (mdl.HasMember("positions")) {
 *        assert(mdl["positions"].IsArray());
 *        for (const auto &isc : mdl["positions"].GetArray()) {
 *          SLArCfgSuperCell* scCfg = new SLArCfgSuperCell(); 
 *          const char* cunit = isc["unit"].GetString(); 
 *          auto xyz = isc["xyz"].GetArray(); 
 *          auto rot = isc["rot"].GetArray(); 
 *
 *          scCfg->SetIdx(isc["copy"].GetInt()); 
 *
 *          scCfg->SetX(xyz[0].GetDouble()*G4UIcommand::ValueOf(cunit)); 
 *          scCfg->SetY(xyz[1].GetDouble()*G4UIcommand::ValueOf(cunit)); 
 *          scCfg->SetZ(xyz[2].GetDouble()*G4UIcommand::ValueOf(cunit)); 
 *
 *          scCfg->SetPhi  (rot[0].GetDouble()*TMath::DegToRad()); 
 *          scCfg->SetTheta(rot[1].GetDouble()*TMath::DegToRad()); 
 *          scCfg->SetPsi  (rot[2].GetDouble()*TMath::DegToRad()); 
 *
 *          scCfg->Set2DSize_X(fSuperCell->GetGeoPar("cell_z")); 
 *          scCfg->Set2DSize_Y(fSuperCell->GetGeoPar("cell_x")); 
 *
 *          if (isc.HasMember("norm")) {
 *            assert(isc["norm"].IsArray()); 
 *            scCfg->SetNormal(
 *                  isc["norm"].GetArray()[0].GetDouble(),
 *                  isc["norm"].GetArray()[1].GetDouble(),
 *                  isc["norm"].GetArray()[2].GetDouble()
 *                ); 
 *          }
 *
 *          sc_array->RegisterElement(scCfg); 
 *        }
 *      }
 *
 *      pdsCfg->RegisterElement(sc_array);
 *    }
 *  }
 *
 *
 *  SLArAnalysisManager::Instance()->LoadPDSCfg(pdsCfg);
 *
 *}
 *
 */
/**
 * @details Parse the description of the pixelated anode readout system. 
 * Build the fReadoutTile object, setup the anode readout configuration
 * according to the description provided in pixsys["modules"] and finally 
 * source the configuration to the analysis manager. 
 *
 * @param pixsys Pixelated anode readout description
 */
void SLArDetectorConstruction::InitReadoutTile(const rapidjson::Value& pixsys) {
  fReadoutTile = new SLArDetReadoutTile();
  
  assert(pixsys.HasMember("dimensions")); 
  assert(pixsys.HasMember("components")); 
  assert(pixsys.HasMember("unit_cell")); 

  fReadoutTile->GetGeoInfo()->ReadFromJSON(pixsys["dimensions"].GetArray()); 
  fReadoutTile->BuildComponentsDefinition(pixsys["components"]); 
  fReadoutTile->BuildUnitCellStructure(pixsys["unit_cell"]); 
  fReadoutTile->BuildMaterial(fMaterialDBFile);

  if (pixsys.HasMember("tile_assembly")) {
    assert(pixsys["tile_assembly"].IsArray()); 

    for (const auto &mtile : pixsys["tile_assembly"].GetArray()) {
      // Setup megatile
      SLArDetReadoutTileAssembly* megatile = new SLArDetReadoutTileAssembly(); 
      assert(mtile.HasMember("dimensions")); 
      megatile->GetGeoInfo()->ReadFromJSON(mtile["dimensions"].GetArray()); 
      megatile->BuildMaterial(fMaterialDBFile); 
      fReadoutMegaTile.insert(std::make_pair(mtile["name"].GetString(),megatile)); 
    } // end of Megatile models loop
  } // endif pixsys.HasMember("tile_assembly")
}

void SLArDetectorConstruction::InitAnode(const rapidjson::Value& jconf) {
  assert(jconf.IsArray()); 

  for (const auto &janode : jconf.GetArray()) {
    SLArDetAnodeAssembly* detAnode = new SLArDetAnodeAssembly(); 
    detAnode->Init(janode); 
    fAnodes.insert( std::make_pair(detAnode->GetID(), detAnode) ); 
  }

}

void SLArDetectorConstruction::ConstructTarget() {
  G4ThreeVector target_min   (0, 0, 0); 
  G4ThreeVector target_max   (0, 0, 0); 

  for (const auto &tpc_ : fTPC) {
    auto tpc = tpc_.second; 
    G4ThreeVector local_center; 
    G4ThreeVector local_dim;  
    local_center.setX(tpc->GetGeoPar("tpc_pos_x")); 
    local_center.setY(tpc->GetGeoPar("tpc_pos_y")); 
    local_center.setZ(tpc->GetGeoPar("tpc_pos_z"));
    local_dim   .setX(tpc->GetGeoPar("tpc_x")); 
    local_dim   .setY(tpc->GetGeoPar("tpc_y")); 
    local_dim   .setZ(tpc->GetGeoPar("tpc_z"));

    G4ThreeVector local_min = local_center - 0.5*local_dim; 
    G4ThreeVector local_max = local_center + 0.5*local_dim; 

    G4cout << local_center << G4endl; 
    G4cout << local_min << G4endl; 
    G4cout << local_max << G4endl; 

    for (int idim = 0; idim <3; idim++) {
      if (local_min[idim] < target_min[idim]) target_min[idim] = local_min[idim]; 
      if (local_max[idim] > target_max[idim]) target_max[idim] = local_max[idim]; 

      //printf("target_max: [%g, %g, %g], target_min: [%g, %g, %g]\n", 
          //target_max[0], target_max[1], target_max[2], 
          //target_min[0], target_min[1], target_min[2]); 
    }
  }

  G4ThreeVector target_center = 0.5*(target_min + target_max); 
  G4double target_dim[3] = {0.}; 
  for (int idim = 0; idim < 3; idim++) {
    target_dim[idim] = target_max[idim] - target_min[idim]; 
  }

  fDetector = new SLArBaseDetModule(); 
  fDetector->SetGeoPar("det_pos_x", target_center.x()); 
  fDetector->SetGeoPar("det_pos_y", target_center.y()); 
  fDetector->SetGeoPar("det_pos_z", target_center.z()); 
  
  fDetector->SetGeoPar("det_x", target_dim[0]); 
  fDetector->SetGeoPar("det_y", target_dim[1]); 
  fDetector->SetGeoPar("det_z", target_dim[2]); 

  printf("LAr target: pos (%g, %g, %g) mm - size %g x %g x %g\n", 
      target_center.x(), target_center.y(), target_center.z(), 
      target_dim[0], target_dim[1], target_dim[2]); 

  fDetector->SetSolidVolume( new G4Box("target_solid", 
      0.5*target_dim[0], 0.5*target_dim[1], 0.5*target_dim[2]) ); 
  SLArMaterial* matTarget = new SLArMaterial("LAr"); 
  matTarget->BuildMaterialFromDB(fMaterialDBFile); 
  fDetector->SetLogicVolume( new G4LogicalVolume(fDetector->GetModSV(), 
      matTarget->GetMaterial(), "lar_target_lv") ); 
  fDetector->GetModLV()->SetVisAttributes( G4VisAttributes(false) ); 

}

void SLArDetectorConstruction::ConstructCryostat() {

  fCryostat->BuildMaterials(fMaterialDBFile); 
  fCryostat->BuildCryostat(); 

  fCryostat->GetModPV("cryostat_pv", 0, 
      fDetector->GetModPV()->GetTranslation(), 
      fWorldLog, 0) ; 

  fCryostat->SetVisAttributes(); 

}

void SLArDetectorConstruction::ConstructCathode() {
  for (auto &cathode : fCathode) {
    cathode.second->BuildMaterial(fMaterialDBFile); 
    cathode.second->BuildCathode(); 
    auto geoinfo = cathode.second->GetGeoInfo(); 
    cathode.second->GetModPV(
        "cathode_pv_"+std::to_string(cathode.first), 0, 
        G4ThreeVector(geoinfo->GetGeoPar("pos_x"), 
          geoinfo->GetGeoPar("pos_y"), 
          geoinfo->GetGeoPar("pos_z")), 
        fDetector->GetModLV(), 0, cathode.first); 
  }
}

/**
 * @details Construct the world volume, build and place the 
 * SLArDetectorConstruction::fTPC object. 
 * After this first step, the method calls BuildAndPlaceSuperCells() and
 * BuildAndPlaceAnode() to place the SuperCell and the Readout Tile
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

  // 2. Build and place the LAr target
  G4cerr << "\nSLArDetectorConstruction: Building the Detector Volume" << G4endl;
  fDetector->SetModPV( new G4PVPlacement(0, 
        G4ThreeVector(fDetector->GetGeoPar("det_pos_x"), 
          fDetector->GetGeoPar("det_pos_y"), 
          fDetector->GetGeoPar("det_pos_z")), 
        fDetector->GetModLV(), "target_lar_pv", fWorldLog, 0, 9) ); 
  
  // 3. Build and place the Cryostat
  G4cerr << "\nSLArDetectorConstruction: Building the Cryostat" << G4endl;
  fCryostat->SetWorldMaterial(matWorld); 
  ConstructCryostat(); 

  G4cerr << "\nSLArDetectorConstruction: Building the Cathode" << G4endl;
  ConstructCathode(); 

  G4cerr << "\nSLArDetectorConstruction: Building the TPCs" << G4endl;
  for (auto &tpc : fTPC) {
    tpc.second->BuildMaterial(fMaterialDBFile); 
    tpc.second->BuildTPC();
    tpc.second->GetModPV("TPC"+std::to_string(tpc.first), 0,
        tpc.second->GetTPCcenter(), 
        fDetector->GetModLV(), false, tpc.first);
    tpc.second->SetVisAttributes(); 
  }

  // 3. Build and place the "conventional" Photon Detection System 
  if (fSuperCell) BuildAndPlaceSuperCells();

  // 4. Build and place the "pixel-based" readout system 
  BuildAndPlaceAnode(); 

  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //Visualization attributes
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
        fReadoutTile->GetSiPMActive()->GetModLV(), sipmSD );
  }

  //Set SuperCell SD
  if (fSuperCell) {
    G4VSensitiveDetector* superCellSD
      = new SLArSuperCellSD(SDname="/supercell"); 
    SDman->AddNewDetector(superCellSD); 
    SetSensitiveDetector(
        fSuperCell->GetCoating()->GetModLV(), superCellSD );
  }

  // Set LAr-volume SD
  G4int iTPC = 0; 
  for (const auto tpc : fTPC) {
    auto tpcSD = 
      new SLArLArSD("/TPC/LArTPC"+std::to_string(tpc.first), tpc.first);
    SDman->AddNewDetector(tpcSD);
    SetSensitiveDetector(tpc.second->GetModLV(), tpcSD);
    iTPC++; 
  }


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
      fCryostat->GetCryostatStructure()[iBPltCryostatLayerID[i]]->fModule->GetModLV(); 
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
      fCryostat->GetCryostatStructure()[iCryostatWallLayerID[i]]->fModule->GetModLV(); 
    auto lv = lv_mother->GetDaughter(0)->GetLogicalVolume();

    SetSensitiveDetector(lv, CryostatWall[i]); 
  }

}

SLArDetTPC* SLArDetectorConstruction::GetDetTPC(int copyid) 
{
  int count = fTPC.count(copyid); 
  if (!count) {
    printf("SLArDetectorConstruction::GetDetTPC(%i) ERROR:", copyid); 
    printf("Unable to find TPC with copy id %i\n", copyid); 
    return nullptr;
  }

  return fTPC.find(copyid)->second;
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
  fSuperCell->BuildMaterial(fMaterialDBFile);
  fSuperCell->BuildSuperCell();
  fSuperCell->BuildLogicalSkinSurface(); 

  // Get PMTSystem Configuration
  SLArAnalysisManager* SLArAnaMgr = SLArAnalysisManager::Instance();
  SLArCfgSystemSuperCell*  pdsCfg = new SLArCfgSystemSuperCell("PDSCfg"); 

  printf("-- Building SuperCell arrays\n");
  for (auto &array_ : fSCArray) {
    auto scarray = array_.second; 
    auto scarray_id = array_.first; 
    scarray->BuildMaterial(fMaterialDBFile); 
    printf("---- Building SC array volume\n");
    scarray->BuildSuperCellArray( fSuperCell );
    auto pos = scarray->GetPosition(); 
    auto rot = scarray->GetRotation();

    auto tpc = fTPC.find(scarray->GetTPCID())->second; 
    auto glb_pos = tpc->GetTPCcenter() + pos; 
    scarray->SetGlobalPos( glb_pos ); 

    printf("---- Placing SC Array %i in TPC %i\n", scarray_id, tpc->GetID());
    scarray->GetModPV("anode"+std::to_string(scarray_id), 
        rot, pos, tpc->GetModLV(), 0, scarray_id); 

    auto array_cfg = scarray->BuildSuperCellArrayCfg(); 
    array_cfg->SetX( pos.x() ); array_cfg->SetPhysX( glb_pos.x() );
    array_cfg->SetY( pos.y() ); array_cfg->SetPhysY( glb_pos.y() );
    array_cfg->SetZ( pos.z() ); array_cfg->SetPhysZ( glb_pos.z() );
    pdsCfg->RegisterElement( array_cfg ); 
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
void SLArDetectorConstruction::BuildAndPlaceAnode() {

  printf("SLArDetectorConstruction::BuildAndPlaceAnode()...\n");
  printf("-- Building readout tile\n");
  fReadoutTile->BuildReadoutTile(); 
  fReadoutTile->SetVisAttributes();
  fReadoutTile->BuildLogicalSkinSurface(); 

  printf("-- Building readout tile assemblies\n");
  for (auto &mt : fReadoutMegaTile) {
    mt.second->BuildReadoutPlane(fReadoutTile); 
  }

  printf("-- Building anode assemblies\n");
  auto ana_mgr = SLArAnalysisManager::Instance();

  for (auto &anode_ : fAnodes) {
    auto anode = anode_.second; 
    auto anode_id = anode_.first; 
    anode->BuildMaterial(fMaterialDBFile); 
    printf("---- Building anode volume\n");
    anode->BuildAnodeAssembly( 
        fReadoutMegaTile.find(anode->GetTileAssemblyModel())->second );
    auto pos = anode->GetPosition(); 
    auto rot = anode->GetRotation();

    auto tpc = fTPC.find(anode->GetTPCID())->second; 
    auto glb_pos = tpc->GetTPCcenter() + pos; 

    printf("---- Placing Anode %i in TPC %i\n", anode_id, tpc->GetID());
    anode->GetModPV("anode"+std::to_string(anode_id), 
        rot, pos, tpc->GetModLV(), 0, anode_id); 

    auto anode_cfg = anode->BuildAnodeConfig(); 
    anode_cfg->SetX( pos.x() ); anode_cfg->SetPhysX( glb_pos.x() ); 
    anode_cfg->SetY( pos.y() ); anode_cfg->SetPhysY( glb_pos.y() ); 
    anode_cfg->SetZ( pos.z() ); anode_cfg->SetPhysZ( glb_pos.z() ); 
    ana_mgr->LoadAnodeCfg(anode_cfg); 
  }

  ConstructAnodeMap(); 
}

void SLArDetectorConstruction::ConstructAnodeMap() {
  printf("SLArDetectorConstruction::ConstructAnodeMap()\n");
  auto ana_mgr = SLArAnalysisManager::Instance(); 

  for (auto &anodeCfg_ : ana_mgr->GetAnodeCfg()) {
    auto anodeCfg = anodeCfg_.second; 
    // access the first megatile to extract the map of the tiles 
    // (which is replicated for all the megatiles in the anode). 
    int megatile_nr = anodeCfg->GetMap().size(); 
    printf("%s has %i elements registered\n", anodeCfg->GetName(), megatile_nr); 
    for (const auto& mt : anodeCfg->GetMap()) {
      printf("%s\n", mt.second->GetName()); 
    }

    auto mtileCfg = anodeCfg->GetMap().begin()->second; 

    if (!mtileCfg) printf("mtileCfg is null!\n");

    auto hMapMegaTile = anodeCfg->BuildPolyBinHist(); 
    printf("mapMegaTile\n");
    auto hMapTile     = mtileCfg->BuildPolyBinHist(
        SLArCfgAssembly<SLArCfgReadoutTile>::ESubModuleReferenceFrame::kRelative); 
    printf("mapTile\n");
    G4RotationMatrix* mtile_rot = new G4RotationMatrix(
        mtileCfg->GetPhi(), 
        mtileCfg->GetTheta(), 
        mtileCfg->GetPsi());
    G4RotationMatrix* mtile_rot_inv = new G4RotationMatrix(*mtile_rot); 
    mtile_rot_inv->invert(); // FIXME: Why do I need to use the inverse rotation????? 

    auto hMapPixel = fReadoutTile->BuildTileChgPixelMap(
        G4ThreeVector(anodeCfg->GetAxis0().x(), anodeCfg->GetAxis0().y(), anodeCfg->GetAxis0().z()), 
        G4ThreeVector(anodeCfg->GetAxis1().x(), anodeCfg->GetAxis1().y(), anodeCfg->GetAxis1().z()), 
        nullptr, mtile_rot_inv);
    printf("mapPixel\n");

    anodeCfg->RegisterMap(0, hMapMegaTile); 
    anodeCfg->RegisterMap(1, hMapTile); 
    anodeCfg->RegisterMap(2, hMapPixel); 

    delete mtile_rot;
    delete mtile_rot_inv; 
  }

  printf("SLArDetectorConstruction::ConstructAnodeMap() DONE \n");
  return; 
}

