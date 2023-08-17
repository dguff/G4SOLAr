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
protected:
  TFile m_gfile {};
  TTree *m_gtree {};
  GenieEvent gVar;

public:
  SLArGENIEGeneratorAction();
  //  SLArGENIEGeneratorAction(const G4String genie_file);
  ~SLArGENIEGeneratorAction();

  virtual void GeneratePrimaries(G4Event* evnt);


};


#endif
