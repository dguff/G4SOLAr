/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArTankHit
 * @created     : lunedì ago 31, 2020 22:25:04 CEST
 */

#ifndef SLArTANKHIT_HH

#define SLArTANKHIT_HH

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4SystemOfUnits.hh"

class G4AttDef;
class G4AttValue;

/// Particle interaction inside the tank
///
/// It records:
///
/// - the energy deposit
/// - the particle time (at entrance)
/// - the particle global position (at entrance)


class SLArTankHit : public G4VHit
{
  public:
    SLArTankHit();
    SLArTankHit(const SLArTankHit &right);
    virtual ~SLArTankHit();

    const SLArTankHit& operator=(const SLArTankHit &right);
    int operator==(const SLArTankHit &right) const;

    inline void *operator new(size_t);
    inline void operator delete(void *aHit);

    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;
    virtual void Print();

    void SetDepositedEnergy(G4double z) { fEneDep = z; }
    G4double GetDepositedEnergy() const { return fEneDep; }

    void Add(double ed);

    void      SetParticleType(G4String prname);
    G4String  GetParticleType();

  private:
    G4double      fEneDep;
    G4int         fNsecondaries;
    //G4int         fCounterE; // electron-positron counter
    //G4int         fCounterP; // proton counter
    //G4int         fCounterG; // gamma counter
    //G4int         fCounterPI;// pion counter
    G4String      fParticleType;
};

typedef G4THitsCollection<SLArTankHit> SLArTankHitsCollection;

extern G4ThreadLocal G4Allocator<SLArTankHit>* SLArTankHitAllocator;

inline void* SLArTankHit::operator new(size_t)
{
  if (!SLArTankHitAllocator)
    SLArTankHitAllocator = new G4Allocator<SLArTankHit>;
  return (void*)SLArTankHitAllocator->MallocSingle();
}

inline void SLArTankHit::operator delete(void* aHit)
{
  SLArTankHitAllocator->FreeSingle((SLArTankHit*) aHit);
}




#endif /* end of include guard SLArTANKHIT_HH */

