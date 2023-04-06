/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArReadoutTileHit.hh
 * @created     Wed Aug 10, 2022 08:56:55 CEST
 */

#ifndef SLARREADOUTTILEHIT_HH

#define SLARREADOUTTILEHIT_HH

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"

class G4AttDef;
class G4AttValue;

/// ReadoutTile hit
///
/// It records:
/// - the ReadoutTile ID
/// - the particle time
/// - the particle local and global positions


class SLArReadoutTileHit : public G4VHit
{
public:
    SLArReadoutTileHit();
    SLArReadoutTileHit(G4double z);
    SLArReadoutTileHit(const SLArReadoutTileHit &right);
    virtual ~SLArReadoutTileHit();

    const SLArReadoutTileHit& operator=(const SLArReadoutTileHit &right);
    int operator==(const SLArReadoutTileHit &right) const;
    
    inline void *operator new(size_t);
    inline void operator delete(void *aHit);
    
    virtual void Draw();
    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;
    virtual void Print();

    void SetPhotonWavelength(G4double z) { fWavelength = z; }
    G4double GetPhotonWavelength() const { return fWavelength; }

    void SetTime(G4double t) { fTime = t; }
    G4double GetTime() const { return fTime / CLHEP::ns; }

    void SetLocalPos(G4ThreeVector xyz) { fLocalPos = xyz; }
    G4ThreeVector GetLocalPos() const { return fLocalPos; }

    void SetWorldPos(G4ThreeVector xyz) { fWorldPos = xyz; }
    G4ThreeVector GetWorldPos() const { return fWorldPos; }

    void      SetPhotonProcess(G4String prname);
    G4int     GetPhotonProcessId();
    G4String  GetPhotonProcessName();

    void SetAnodeIdx(G4int idx) { fAnodeIdx= idx; }
    G4int GetAnodeIdx() const { return fAnodeIdx; }
    void SetRowMegaTileIdx(G4int idx) { fRowMegaTileIdx= idx; }
    G4int GetRowMegaTileIdx() const { return fRowMegaTileIdx; }
    void SetMegaTileIdx(G4int idx) { fMegaTileIdx= idx; }
    G4int GetMegaTileIdx() const { return fMegaTileIdx; }
    void SetRowTileIdx(G4int idx) { fRowTileIdx= idx; }
    G4int GetRowTileIdx() const { return fRowTileIdx; }
    void SetTileIdx(G4int idx) { fTileIdx= idx; }
    G4int GetTileIdx() const { return fTileIdx; }
    void SetCellNr(G4int n) {fCellNr = n;}
    G4int GetCellNr() {return fCellNr;}
    void SetRowCellNr(G4int n) {fRowCellNr = n;}
    G4int GetRowCellNr() {return fRowCellNr;}


    G4String GetProcessName(int kType);
private:
    G4int         fAnodeIdx; 
    G4int         fRowMegaTileIdx;
    G4int         fMegaTileIdx;
    G4int         fRowTileIdx; 
    G4int         fTileIdx;
    G4int         fRowCellNr; 
    G4int         fCellNr; 
    G4double      fWavelength;
    G4double      fTime;
    G4int         fPhType;
    G4ThreeVector fLocalPos;
    G4ThreeVector fWorldPos;
};

typedef G4THitsCollection<SLArReadoutTileHit> SLArReadoutTileHitsCollection;

extern G4ThreadLocal G4Allocator<SLArReadoutTileHit>* SLArReadoutTileHitAllocator;

inline void* SLArReadoutTileHit::operator new(size_t)
{
    if (!SLArReadoutTileHitAllocator)
        SLArReadoutTileHitAllocator = new G4Allocator<SLArReadoutTileHit>;
    return (void*)SLArReadoutTileHitAllocator->MallocSingle();
}

inline void SLArReadoutTileHit::operator delete(void* aHit)
{
    SLArReadoutTileHitAllocator->FreeSingle((SLArReadoutTileHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /* end of include guard SLARREADOUTTILEHIT_HH */
