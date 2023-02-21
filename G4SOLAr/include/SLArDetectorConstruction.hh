/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArDetectorConstruction.hh
 * @created     mercoledì nov 16, 2022 09:42:24 CET
 */

#ifndef SLArDetectorConstruction_h
#define SLArDetectorConstruction_h 

#include "detector/TPC/SLArDetTPC.hh"
#include "detector/SuperCell/SLArDetSuperCell.hh"
#include "detector/ReadoutTile/SLArDetReadoutTile.hh"
#include "detector/ReadoutTile/SLArDetReadoutPlane.hh"

#include "SLArAnalysisManagerMsgr.hh"

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4MaterialPropertyVector.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4VisAttributes.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SLArCfgSuperCellArray;

/**
 * @brief SoLAr detector construction class
 *
 * This class constructs the SoLAr detector building its individual 
 * components (cryostat, target, readout system) and placing them 
 * in the world volume. 
 */
class SLArDetectorConstruction : public G4VUserDetectorConstruction
{


  friend class SLArAnalysisManagerMsgr;

  public:
    /**
     * @brief Constructor
     */
    SLArDetectorConstruction(G4String, G4String);
    /**
     * @brief Destructor
     */
    virtual ~SLArDetectorConstruction();

  public:
    /**
     * @brief Construct world and place detectors
     */
    virtual G4VPhysicalVolume* Construct();
    /**
     * @brief Construct Sensitive Detectors and cryostat scorers
     */
    virtual void ConstructSDandField();
    /**
     * @brief Construct virtual pixelization of the anode readout system
     */
    void ConstructAnodeMap(); 
    /**
     * @brief Return SLArDetectorConstruction::fTPC object
     */
    SLArDetTPC* GetDetTPC();
    /**
     * @brief Build SuperCell object and place the SuperCells according to the
     * given configuration
     */
    void BuildAndPlaceSuperCells();
    /**
     * @brief Build the ReadoutTile object and the place the MegaTiles 
     * according to the given configuration
     */
    void BuildAndPlaceReadoutTiles();
    /**
     * @brief Get the World's logical volume
     */
    G4LogicalVolume*                GetLogicWorld();
    std::vector<G4VPhysicalVolume*>&GetVecSuperCellPV();
    /**
     * @brief  Return the geometry configuration file
     */
    G4String                        GetGeometryCfgFile() {return fGeometryCfgFile;}
    /**
     * @brief  Return the material configuration file
     */
    G4String                        GetMaterialCfgFile() {return fMaterialDBFile;}
    void                            DumpSuperCellMap(G4String path = "");
    /**
     * @brief Construct scorers in the cryostat layers for neutron shielding studies
     */
    void                            ConstructCryostatScorer(); 

  private:

    //! Detector description initilization
    void Init();
    G4String fGeometryCfgFile; //!< Geometry configuration file
    G4String fMaterialDBFile;  //!< Material table file
    //! vector of visualization attributes
    std::vector<G4VisAttributes*>   fVisAttributes; 

    //! TPC detector object (cryostat + LAr target)
    SLArDetTPC* fTPC;

    SLArGeoInfo fWorldGeoPars;//!< World volume geometry parameters
    SLArDetSuperCell* fSuperCell; //!< SuperCell detector object
    SLArDetReadoutTile* fReadoutTile; //!< ReadoutTile detector object
    std::map<G4String, SLArDetReadoutPlane*> fReadoutMegaTile; 

    G4LogicalVolume* fWorldLog; //!< World logical volume
    std::vector<G4VPhysicalVolume*> fSuperCellsPV;
    G4String GetFirstChar(G4String line);
    
    //! Parse the description of the supercell detector system
    void InitPDS(const rapidjson::Value&); 
    //! Parse the description of the ReadoutTile detector system
    void InitPix(const rapidjson::Value&); 
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*SLArDetectorConstruction_h*/
