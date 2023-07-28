#include "SLArGENIEGeneratorAction.hh"


//***********************************************************************
//************************** CONSTRUCTORS *******************************

SLArGENIEGeneratorAction::SLArGENIEGeneratorAction() 
  : m_gfile("/pbs/home/j/jmcelwee/data/enubet_genie_seed.root"), m_gtree(0)
{

  //  TFile infile(m_gfile.c_str(),"READ");
  m_gfile.GetObject("enubetG",m_gtree);

  m_gtree->SetBranchAddress("EvtNum",&gVar.EvtNum);
  m_gtree->SetBranchAddress("StdHepN",&gVar.nPart);
  m_gtree->SetBranchAddress("StdHepPdg",&gVar.pdg);
  m_gtree->SetBranchAddress("StdHepP4",&gVar.p4);
  m_gtree->SetBranchAddress("StdHepX4",&gVar.x4);
  m_gtree->SetBranchAddress("EvtVtx",&gVar.vtx);

  m_gtree->Show(0);



}


SLArGENIEGeneratorAction::~SLArGENIEGeneratorAction()
{}

//***********************************************************************

//***********************************************************************
//************************** EVENT FUNCTIONS ****************************

void SLArGENIEGeneratorAction::GeneratePrimaries(G4Event *ev)
{

  int evtNum = ev->GetEventID();
  m_gtree->GetEntry(evtNum);
  //  m_gtree->Show(evtNum);

  for (int i=0; i<gVar.nPart; i++){
    //    std::cout << gVar.pdg[i] << std::endl;
  }

}

//***********************************************************************






/*
SLArGENIEGeneratorAction::SLArGENIEGeneratorAction(const G4String genie_file)
  : m_gfile(genie_file)
{
  TFile infile(m_gfile,"READ");
  infile.GetObject("enubetG",m_gtree);
}

void SLArGENIEGeneratorAction::TestFunction()
{
  m_gtree->Show(0);
  }*/
