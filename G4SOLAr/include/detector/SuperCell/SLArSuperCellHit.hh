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
// $Id: SLArSuperCellHit.hh 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file SLArSuperCellHit.hh
/// \brief Definition of the SLArSuperCellHit class

#ifndef SLArSuperCellHit_h
#define SLArSuperCellHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"

class G4AttDef;
class G4AttValue;

/// SuperCell hit
///
/// It records:
/// - the SuperCell ID
/// - the particle time
/// - the particle local and global positions


class SLArSuperCellHit : public G4VHit
{
public:
    SLArSuperCellHit();
    SLArSuperCellHit(G4double z);
    SLArSuperCellHit(const SLArSuperCellHit &right);
    virtual ~SLArSuperCellHit();

    const SLArSuperCellHit& operator=(const SLArSuperCellHit &right);
    int operator==(const SLArSuperCellHit &right) const;
    
    inline void *operator new(size_t);
    inline void operator delete(void *aHit);
    
    virtual void Draw();
    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;
    virtual void Print();

    void SetPhotonEnergy(G4double z) { fEnergy = z; }
    G4double GetPhotonEnergy() const { return fEnergy; }

    void SetTime(G4double t) { fTime = t; }
    G4double GetTime() const { return fTime / CLHEP::ns; }

    void SetLocalPos(G4ThreeVector xyz) { fLocalPos = xyz; }
    G4ThreeVector GetLocalPos() const { return fLocalPos; }

    void SetWorldPos(G4ThreeVector xyz) { fWorldPos = xyz; }
    G4ThreeVector GetWorldPos() const { return fWorldPos; }

    void      SetPhotonProcess(G4String prname);
    G4int     GetPhotonProcessId();
    G4String  GetPhotonProcessName();

    void SetPMTIdx(G4int idx) { fPMTIdx= idx; }
    G4int GetPMTIdx() const { return fPMTIdx; }

    G4String GetProcessName(int kType);
private:
    G4int         fPMTIdx;
    G4double      fEnergy;
    G4double      fTime;
    G4int         fPhType;
    G4ThreeVector fLocalPos;
    G4ThreeVector fWorldPos;
};

typedef G4THitsCollection<SLArSuperCellHit> SLArSuperCellHitsCollection;

extern G4ThreadLocal G4Allocator<SLArSuperCellHit>* SLArSuperCellHitAllocator;

inline void* SLArSuperCellHit::operator new(size_t)
{
    if (!SLArSuperCellHitAllocator)
        SLArSuperCellHitAllocator = new G4Allocator<SLArSuperCellHit>;
    return (void*)SLArSuperCellHitAllocator->MallocSingle();
}

inline void SLArSuperCellHit::operator delete(void* aHit)
{
    SLArSuperCellHitAllocator->FreeSingle((SLArSuperCellHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
