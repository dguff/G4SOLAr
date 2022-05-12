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

//#include "detector/PMT/SLArDetPMT.hh"
//#include "detector/LAPPD/SLArDetLAPPD.hh"
//#include "detector/Hodoscope/SLArDetHodoscope.hh"
#include "detector/SLArDetectorSize.hh"
#include "detector/Tank/SLArDetTank.hh"

#include "SLArAnalysisManagerMsgr.hh"

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4MaterialPropertyVector.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4VisAttributes.hh"
//#include "G4GDMLParser.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SLArCfgPMTArray;

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
  //friend class SLArDetPMTMsgr;
  friend class SLArAnalysisManagerMsgr;

  public:
    SLArDetectorConstruction();
    virtual ~SLArDetectorConstruction();

  public:
    virtual G4VPhysicalVolume*      Construct();
    virtual void                    ConstructSDandField();
    SLArDetTank*                    GetDetTank();
    //SLArDetPMT*                       GetDetPMT(const char* mod);
    //std::map<G4String,SLArDetPMT*>&   GetDetPMTs();
    //std::map<G4String,SLArDetHodoscope*>&   
                                    //GetDetHodoscopes();
    //SLArDetLAPPD*                     GetDetLAPPD();
    //void  [>deprecated<]            LoadPMTMap(G4String path);
    //void                            BuildAndPlacePMTs();
    //void                            BuildPMTModel(const char* mod);
    //void                            BuildAndPlaceLAPPD();
    //void                            BuildAndPlaceHodoscope();
    //void                            BuildAndPlaceLAPPD(
                                      //G4RotationMatrix*    rot, 
                                      //const G4ThreeVector& pos, 
                                      //G4String            name, 
                                      //G4int             copyNo);
    G4LogicalVolume*                GetLogicWorld();
    std::vector<G4VPhysicalVolume*>&GetVecPMT();
    void                            DumpPMTMap(G4String path = "");
    int                             RemovePMTs ();
    int                             RemoveHodoModules();

  private:
    void                            Init();
    G4String                        fPMTMapPath;
    //G4GDMLParser                    fParser;
    std::vector<G4VisAttributes*>   fVisAttributes;

    SLArDetTank*                      fTank;
    SLArDetTankMsgr*                  fTankMsgr;

    //std::map<G4String, SLArDetHodoscope*>
                                    //fHodoscopes;

    //std::map<G4String,SLArDetPMT*>    fPMTs;
    G4RotationMatrix*               fRotPMTBDwnStr;  
    G4RotationMatrix*               fRotPMTBTop   ;  
    G4RotationMatrix*               fRotPMTBBottom;  
    G4RotationMatrix*               fRotPMTBLeft  ;  
    G4RotationMatrix*               fRotPMTBRight ;  
    //SLArDetPMTMsgr*                   fPMTMsgr;
    // TODO: clean up code from fPMTMap
    //       use SLArSystemConfigPMT instead
    //std::vector<PMTGeoInfo>         fPMTMap; // deprecated
                                          

    //bool                            fIsLAPPD;
    //SLArDetLAPPD*                   fLAPPD;
    //SLArDetLAPPDMsgr*               fLAPPDMsgr;
    //void                            SetLAPPD(bool kLAPPD);
    //bool                            IsLAPPD();

    G4LogicalVolume*                fWorldLog;
    //std::vector<G4VPhysicalVolume*> fPMTPV;
    //std::vector<G4VPhysicalVolume*> fHodoModulePV;
    G4String                        GetFirstChar(G4String line);


  protected:
    SLArDetectorSize::SLArGEOcoll fColl;    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*SLArDetectorConstruction_h*/
