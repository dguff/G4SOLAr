/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArReadoutTileHit
 * @created     : mercoledì ago 10, 2022 08:58:05 CEST
 */

#include "detector/ReadoutTile/SLArReadoutTileHit.hh"

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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4Allocator<SLArReadoutTileHit>* SLArReadoutTileHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArReadoutTileHit::SLArReadoutTileHit()
: G4VHit(), fMegaTileIdx(0), fRowTileIdx(0), fTileIdx(0), 
  fWavelength(-1), fTime(0.), fPhType(-1),
  fLocalPos(0), fWorldPos(0) 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArReadoutTileHit::SLArReadoutTileHit(G4double z)
: G4VHit(), fMegaTileIdx(0), fRowTileIdx(0), fTileIdx(0), 
  fWavelength(z), fTime(0.), fPhType(-1),
  fLocalPos(0), fWorldPos(0) 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArReadoutTileHit::~SLArReadoutTileHit()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArReadoutTileHit::SLArReadoutTileHit(const SLArReadoutTileHit &right)
: G4VHit() {
    fWavelength = right.fWavelength;
    fWorldPos   = right.fWorldPos;
    fLocalPos   = right.fLocalPos;
    fTime       = right.fTime;
    fMegaTileIdx= right.fMegaTileIdx;
    fRowTileIdx = right.fRowTileIdx; 
    fTileIdx    = right.fTileIdx; 
    fPhType     = right.fPhType;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const SLArReadoutTileHit& SLArReadoutTileHit::operator=(const SLArReadoutTileHit &right)
{
    fWavelength   = right.fWavelength;
    fWorldPos     = right.fWorldPos;
    fLocalPos     = right.fLocalPos;
    fTime         = right.fTime;
    fMegaTileIdx  = right.fMegaTileIdx;
    fRowTileIdx   = right.fRowTileIdx; 
    fTileIdx      = right.fTileIdx; 
    fPhType       = right.fPhType;
    return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int SLArReadoutTileHit::operator==(const SLArReadoutTileHit &/*right*/) const
{
    return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArReadoutTileHit::Draw()
{
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if(pVVisManager)
    {
        G4Circle circle(fWorldPos);
        circle.SetScreenSize(2);
        circle.SetFillStyle(G4Circle::filled);
        G4Colour colour(1.,1.,0.);
        G4VisAttributes attribs(colour);
        circle.SetVisAttributes(attribs);
        pVVisManager->Draw(circle);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const std::map<G4String,G4AttDef>* SLArReadoutTileHit::GetAttDefs() const
{
    G4bool isNew;
    std::map<G4String,G4AttDef>* store
      = G4AttDefStore::GetInstance("SLArReadoutTileHit",isNew);

    if (isNew) {
        (*store)["HitType"] 
          = G4AttDef("HitType","Hit Type","Physics","","G4String");
        
        (*store)["Wavelength"] 
          = G4AttDef("Wavelength","Length","Physics","nm","G4double");
        
        (*store)["Time"] 
          = G4AttDef("Time","Time","Physics","ns","G4double");
        
        (*store)["Pos"] 
          = G4AttDef("Pos", "Position", "Physics","G4BestUnit","G4ThreeVector");
    
        (*store)["MegaTileIdx"] 
          = G4AttDef("MegatileIdx","Position","Physics","","G4int");

        (*store)["RowTileNr"] 
          = G4AttDef("RowTileIdx","Position","Physics","","G4int");

        (*store)["TileNr"] 
          = G4AttDef("TileIdx","Position","Physics","","G4int");

        (*store)["PhType"] 
          = G4AttDef("PhType","Ph process","Physics","","G4int");
    }
    return store;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<G4AttValue>* SLArReadoutTileHit::CreateAttValues() const
{
    std::vector<G4AttValue>* values = new std::vector<G4AttValue>;
    
    values
      ->push_back(G4AttValue("HitType","PMTHit",""));
    values
      ->push_back(G4AttValue("Wavelength",G4BestUnit(fWavelength,"Length"), ""));
    values
      ->push_back(G4AttValue("Time", G4BestUnit(fTime,"Time"), ""));
    values
      ->push_back(G4AttValue("Pos", G4BestUnit(fWorldPos,"Length"),""));
    values
      ->push_back(G4AttValue("MegaTileIdx", G4UIcommand::ConvertToString(fMegaTileIdx), ""));
    values
      ->push_back(G4AttValue("RowTileNr", G4UIcommand::ConvertToString(fRowTileIdx), ""));
    values
      ->push_back(G4AttValue("TileNr", G4UIcommand::ConvertToString(fTileIdx), ""));
    values
      ->push_back(G4AttValue("PhType", G4UIcommand::ConvertToString(fPhType), ""));
    
    return values;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArReadoutTileHit::Print()
{

    G4cout << " Tile: "<<fMegaTileIdx<<"/"<<fRowTileIdx<<"/"<<fTileIdx<< "\n"
           << GetPhotonProcessName()
           << " Ph wavelength" << fWavelength << "[nm]"
           << " : time "         << fTime/CLHEP::ns << " (nsec)"
           << " --- global (x,y) "<< G4BestUnit(fWorldPos.x(), "Length")
           << ", " << G4BestUnit(fWorldPos.y(), "Length") << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArReadoutTileHit::SetPhotonProcess(G4String prname)
{
  if       (G4StrUtil::contains(prname, "Cerenkov")) fPhType = 1;
  else if  (G4StrUtil::contains(prname, "Scint"   )) fPhType = 2;
  else if  (G4StrUtil::contains(prname, "WLS"     )) fPhType = 3;
  else                                   fPhType = 4;
}

G4int SLArReadoutTileHit::GetPhotonProcessId()
{
  return fPhType;
}

G4String SLArReadoutTileHit::GetPhotonProcessName()
{
  G4String prname;
  if      (fPhType == 1) prname = "Cerenkov";
  else if (fPhType == 2) prname = "Scint";
  else if (fPhType == 3) prname = "WLS";
  else                   prname = "Other";
  return prname;
}
