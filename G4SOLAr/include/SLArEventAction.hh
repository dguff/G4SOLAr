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
// $Id: SLArEventAction.hh 94486 2015-11-19 08:33:37Z gcosmo $
//
/// \file SLArEventAction.hh
/// \brief Definition of the SLArEventAction class

#ifndef SLArEventAction_h
#define SLArEventAction_h 1


#include "G4UserEventAction.hh"
#include "G4HCofThisEvent.hh"
#include "globals.hh"

#include <map>
#include <vector>

/// Event action

class SLArEventAction : public G4UserEventAction
{
public:
    SLArEventAction();
    virtual ~SLArEventAction();
    
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

    void IncPhotonCount_Scnt()  {fPhotonCount_Scnt++;}
    void IncPhotonCount_Cher()  {fPhotonCount_Cher++;}
    void IncPhotonCount_WLS ()  {fPhotonCount_WLS ++;}
    void IncAbsorption()        {fAbsorptionCount++;}
    void IncBoundaryAbsorption(){fBoundaryAbsorptionCount++;}
    void IncHitCount(G4int i=1) {fHitCount+=i;}

    G4int GetPhotonCount_Scnt()const    {return fPhotonCount_Scnt;}
    G4int GetPhotonCount_Cher()const    {return fPhotonCount_Cher;}
    G4int GetPhotonCount_WLS ()const    {return fPhotonCount_WLS ;}
    G4int GetHitCount()const            {return fHitCount;}
    G4double GetEDep()const             {return fTotEdep;}
    G4int GetAbsorptionCount()const     {return fAbsorptionCount;}
    G4int GetBoundaryAbsorptionCount()const {return fBoundaryAbsorptionCount;}

    void  RegisterNewTrackPID(int, int); 
    int   FindTopParentID(int); 
    
private:
    G4int fTileHCollID;
    G4int fTargetHCollID;

    G4int fHitCount;
    G4int fPhotonCount_Scnt;
    G4int fPhotonCount_Cher;
    G4int fPhotonCount_WLS ;
    G4int fAbsorptionCount ;
    G4int fBoundaryAbsorptionCount;
    G4double fTotEdep;

    std::map<int, int> fParentIDMap;

    void RecordEventReadoutTile (const G4Event* ev);
    void RecordEventTarget(const G4Event* ev);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
