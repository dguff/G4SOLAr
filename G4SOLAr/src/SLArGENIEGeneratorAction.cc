#include "SLArGENIEGeneratorAction.hh"


//***********************************************************************
//************************** CONSTRUCTORS *******************************

SLArGENIEGeneratorAction::SLArGENIEGeneratorAction() 
  : m_gfile("/pbs/home/j/jmcelwee/data/enubet_genie_seed.root")
{
  //  TFile infile(m_gfile,"READ");
  //  infile.GetObject("enubetG",m_gTree);
}


SLArGENIEGeneratorAction::~SLArGENIEGeneratorAction()
{}

//***********************************************************************

//***********************************************************************
//************************** EVENT FUNCTIONS ****************************

void SLArGENIEGeneratorAction::GeneratePrimaries(G4Event *ev)
{

  // int evtNum = ev->GetEventID();
  //  m_gTree->Show(0);

}

//***********************************************************************






/*
SLArGENIEGeneratorAction::SLArGENIEGeneratorAction(const G4String genie_file)
  : m_gfile(genie_file)
{
  TFile infile(m_gfile,"READ");
  infile.GetObject("enubetG",m_gTree);
}

void SLArGENIEGeneratorAction::TestFunction()
{
  m_gTree->Show(0);
  }*/
