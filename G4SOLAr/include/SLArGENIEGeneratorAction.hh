#ifndef SLARGENIEGENERATORACTION_HH
#define SLARGENIEGENERATORACTION_HH

#include <iostream>
#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"

#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"



struct GenieEvent{
  Long64_t EvtNum;
  int nPart;
  int pdg[100];
  int status[100];
  double p4[100][4];
  double x4[100][4];
  double vtx[4];
};



class SLArGENIEGeneratorAction : public G4VUserPrimaryGeneratorAction
{
private:
  
protected:
  TTree *m_gtree {};
  GenieEvent gVar;

  G4int m_GENIEInitEvnt {};

public:
  SLArGENIEGeneratorAction();
  SLArGENIEGeneratorAction(const G4String genie_file);
  ~SLArGENIEGeneratorAction();

  void SetGENIEEvntExt(G4int evntID);  
  void Initialize(G4String inGENIE);


  virtual void GeneratePrimaries(G4Event* evnt);


};


#endif
