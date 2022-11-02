//
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
/// \file SLAr/include/SLArDetectorConstruction.hh
/// \brief Definition of the SLArDetectorConstruction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef SLArDetectorConstruction_h
#define SLArDetectorConstruction_h 

#include "detector/Tank/SLArDetTank.hh"
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

struct PMTGeoInfo{
  public:
    G4double fX;
    G4double fY;
    G4int    fNum;

  PMTGeoInfo() {};

  PMTGeoInfo(G4double x, G4double y, G4int num)
  {
    fX   = x;
    fY   = y;
    fNum = num;
  }

};

class SLArDetectorConstruction : public G4VUserDetectorConstruction
{


  friend class SLArDetTankMsgr;
  friend class SLArAnalysisManagerMsgr;

  public:
    SLArDetectorConstruction(G4String, G4String);
    virtual ~SLArDetectorConstruction();

  public:
    virtual G4VPhysicalVolume*      Construct();
    virtual void                    ConstructSDandField();
    SLArDetTank*                    GetDetTank();
    void                            BuildAndPlaceSuperCells();
    void                            BuildAndPlaceReadoutTiles();
    G4LogicalVolume*                GetLogicWorld();
    std::vector<G4VPhysicalVolume*>&GetVecSuperCellPV();
    void                            DumpSuperCellMap(G4String path = "");

  private:
    void                            Init();
    G4String                        fGeometryCfgFile; 
    G4String                        fMaterialDBFile; 
    std::vector<G4VisAttributes*>   fVisAttributes;

    SLArDetTank*                    fTPC;

    SLArGeoInfo                       fWorldGeoPars;
    SLArDetSuperCell*                 fSuperCell;
    SLArDetReadoutTile*               fReadoutTile; 
    std::map<G4String, SLArDetReadoutPlane*> fReadoutMegaTile; 

    G4LogicalVolume*                fWorldLog;
    std::vector<G4VPhysicalVolume*> fSuperCellsPV;
    G4String                        GetFirstChar(G4String line);
    void InitPDS(const rapidjson::Value&); 
    void InitPix(const rapidjson::Value&); 
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*SLArDetectorConstruction_h*/
