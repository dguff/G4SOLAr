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
// $Id: SLArSuperCellHit.cc 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file SLArSuperCellHit.cc
/// \brief Implementation of the SLArSuperCellHit class

#include "detector/SuperCell/SLArSuperCellHit.hh"

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

G4ThreadLocal G4Allocator<SLArSuperCellHit>* SLArSuperCellHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArSuperCellHit::SLArSuperCellHit()
: G4VHit(), fSuperCellNo(0), fSuperCellRowNo(0), fSuperCellArrayNo(0), 
  fEnergy(-1), fWavelength(-1), fTime(0.), fPhType(-1), fPhProducerID(-1),
  fLocalPos(0), fWorldPos(0) 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArSuperCellHit::SLArSuperCellHit(G4double z)
: G4VHit(), fSuperCellNo(0), fSuperCellRowNo(0), fSuperCellArrayNo(0), 
  fEnergy(z), fWavelength(-1), fTime(0.), fPhType(-1), fPhProducerID(-1),
  fLocalPos(0), fWorldPos(0) 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArSuperCellHit::~SLArSuperCellHit()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SLArSuperCellHit::SLArSuperCellHit(const SLArSuperCellHit &right)
: G4VHit() {
    fEnergy       = right.fEnergy;
    fWorldPos     = right.fWorldPos;
    fLocalPos     = right.fLocalPos;
    fTime         = right.fTime;
    fSuperCellNo = right.fSuperCellNo;
    fSuperCellRowNo = right.fSuperCellRowNo;
    fSuperCellArrayNo = right.fSuperCellArrayNo;
    fPhType       = right.fPhType;
    fPhProducerID = right.fPhProducerID;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const SLArSuperCellHit& SLArSuperCellHit::operator=(const SLArSuperCellHit &right)
{
    fEnergy       = right.fEnergy;
    fWavelength   = right.fWavelength;
    fWorldPos     = right.fWorldPos;
    fLocalPos     = right.fLocalPos;
    fTime         = right.fTime;
    fSuperCellNo  = right.fSuperCellNo;
    fSuperCellRowNo = right.fSuperCellRowNo;
    fSuperCellArrayNo = right.fSuperCellArrayNo;
    fPhType       = right.fPhType;
    fPhProducerID = right.fPhProducerID;
    return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int SLArSuperCellHit::operator==(const SLArSuperCellHit &/*right*/) const
{
    return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArSuperCellHit::Draw()
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

const std::map<G4String,G4AttDef>* SLArSuperCellHit::GetAttDefs() const
{
    G4bool isNew;
    std::map<G4String,G4AttDef>* store
      = G4AttDefStore::GetInstance("SLArSuperCellHit",isNew);

    if (isNew) {
        (*store)["HitType"] 
          = G4AttDef("HitType","Hit Type","Physics","","G4String");
        
        (*store)["Energy"] 
          = G4AttDef("Energy","Energy","Physics","G4BestUnit","G4double");

        (*store)["Wavelength"] 
          = G4AttDef("Wavelength","Length","Physics","nm","G4double");        

        (*store)["Time"] 
          = G4AttDef("Time","Time","Physics","ns","G4double");
        
        (*store)["Pos"] 
          = G4AttDef("Pos", "Position", "Physics","G4BestUnit","G4ThreeVector");
    
        (*store)["CellNo"] 
          = G4AttDef("CellNo","Position","Physics","","G4int");
        (*store)["RowNo"] 
          = G4AttDef("RowNo","Position","Physics","","G4int");
        (*store)["ArrayNo"] 
          = G4AttDef("ArrayNo","Position","Physics","","G4int");

        (*store)["PhType"] 
          = G4AttDef("PhType","Ph process","Physics","","G4int");

        (*store)["PhProducerID"] 
          = G4AttDef("PhProducerID","Ph producer trk id","Physics","","G4int");
    }
    return store;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<G4AttValue>* SLArSuperCellHit::CreateAttValues() const
{
    std::vector<G4AttValue>* values = new std::vector<G4AttValue>;
    
    values
      ->push_back(G4AttValue("HitType","SuperCellHit",""));
    values
      ->push_back(G4AttValue("Energy",G4BestUnit(fEnergy,"Energy"), ""));
    values
      ->push_back(G4AttValue("Wavelength", std::to_string(fWavelength), ""));
    values
      ->push_back(G4AttValue("Time", G4BestUnit(fTime,"Time"), ""));
    values
      ->push_back(G4AttValue("Pos", G4BestUnit(fWorldPos,"Length"),""));
    values
      ->push_back(G4AttValue("CellNo", G4UIcommand::ConvertToString(fSuperCellNo), ""));
    values
      ->push_back(G4AttValue("RowNo", G4UIcommand::ConvertToString(fSuperCellRowNo), ""));
    values
      ->push_back(G4AttValue("ArrayNo", G4UIcommand::ConvertToString(fSuperCellArrayNo), ""));
    values
      ->push_back(G4AttValue("PhType", G4UIcommand::ConvertToString(fPhType), ""));
    values
      ->push_back(G4AttValue("PhProducerID", G4UIcommand::ConvertToString(fPhProducerID), ""));    
    return values;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArSuperCellHit::Print()
{

    G4cout << " SuperCell: " << fSuperCellNo << "-" << fSuperCellRowNo << "-" 
           << fSuperCellArrayNo << "\n"
           << GetPhotonProcessName()
           << " Ph wavelength " << fWavelength << " [nm]"
           << " : time "         << fTime/CLHEP::ns << " (ns)"
           << " --- global (x,y) "<< G4BestUnit(fWorldPos.x(), "Length")
           << ", " << G4BestUnit(fWorldPos.y(), "Length") << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SLArSuperCellHit::SetPhotonProcess(G4String prname)
{
  if       (G4StrUtil::contains(prname,"Cerenkov")) fPhType = 1;
  else if  (G4StrUtil::contains(prname,"Scint"   )) fPhType = 2;
  else if  (G4StrUtil::contains(prname,"WLS"     )) fPhType = 3;
  else     fPhType = 4;
}

G4int SLArSuperCellHit::GetPhotonProcessId() const
{
  return fPhType;
}

G4String SLArSuperCellHit::GetPhotonProcessName() const
{
  G4String prname;
  if      (fPhType == 1) prname = "Cerenkov";
  else if (fPhType == 2) prname = "Scint";
  else if (fPhType == 3) prname = "WLS";
  else                   prname = "Other";
  return prname;
}
