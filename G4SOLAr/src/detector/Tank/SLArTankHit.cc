/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArTankHit
 * @created     : lunedì ago 31, 2020 22:25:26 CEST
 */

#include "detector/Tank/SLArTankHit.hh"

#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"


G4ThreadLocal G4Allocator<SLArTankHit>* SLArTankHitAllocator;


SLArTankHit::SLArTankHit()
: G4VHit(), fEneDep(0.), fNsecondaries(0), fParticleType("geantino")
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArTankHit::~SLArTankHit()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArTankHit::SLArTankHit(const SLArTankHit &right)
: G4VHit() {
    fEneDep       = right.fEneDep;
    fNsecondaries = right.fNsecondaries;
    fParticleType = right.fParticleType;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const SLArTankHit& SLArTankHit::operator=(const SLArTankHit &right)
{
    fEneDep       = right.fEneDep;
    fNsecondaries = right.fNsecondaries;
    fParticleType = right.fParticleType;
    return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int SLArTankHit::operator==(const SLArTankHit &/*right*/) const
{
    return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const std::map<G4String,G4AttDef>* SLArTankHit::GetAttDefs() const
{
    G4bool isNew;
    std::map<G4String,G4AttDef>* store
      = G4AttDefStore::GetInstance("SLArTankHit",isNew);

    if (isNew) {
        (*store)["Particle"] 
          = G4AttDef("Particle","Particle","Physics","","G4String");
        
        (*store)["EneDep"] 
          = G4AttDef("EneDep","Energy Deposit","Physics","G4BestUnit","G4double");
    }
    return store;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<G4AttValue>* SLArTankHit::CreateAttValues() const
{
    std::vector<G4AttValue>* values = new std::vector<G4AttValue>;
    
    values
      ->push_back(G4AttValue("EneDep",G4BestUnit(fEneDep,"Energy"), ""));
    values
      ->push_back(G4AttValue("Particle", fParticleType, ""));
    
    return values;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArTankHit::Print()
{
  printf("Tank hit\n");
  G4cout << G4BestUnit(fEneDep, "Energy") << " energy deposit" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArTankHit::SetParticleType(G4String prname)
{
  fParticleType = prname;
}

void SLArTankHit::Add(double ed)
{
  fEneDep += ed;
}

G4String SLArTankHit::GetParticleType()
{
  return fParticleType;
}
