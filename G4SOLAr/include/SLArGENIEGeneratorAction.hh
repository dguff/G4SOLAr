#ifndef SLARGENIEGENERATORACTION_HH
#define SLARGENIEGENERATORACTION_HH

#include <iostream>

#include "TFile.h"
#include "TTree.h"

#include "G4Event.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"

#include "SLArPGunGeneratorAction.hh"

struct GenieEvent{
  long int EvtNum;
  int nPart;
  int pdg[100];
  double p4[100][4];
  double x4[100][4];
  double vtx[4];
};



class SLArGENIEGeneratorAction : public G4VUserPrimaryGeneratorAction
{

public:
  SLArGENIEGeneratorAction();
  //  SLArGENIEGeneratorAction(const G4String genie_file);
  ~SLArGENIEGeneratorAction();

  virtual void GeneratePrimaries(G4Event* evnt);


protected:
  const G4String m_gfile;
  //  TTree *m_gTree =0;

};


#endif
