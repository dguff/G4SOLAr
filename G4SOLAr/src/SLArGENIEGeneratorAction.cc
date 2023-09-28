#include "SLArGENIEGeneratorAction.hh"


//***********************************************************************
//************************** CONSTRUCTORS *******************************

void SLArGENIEGeneratorAction::Initialize(G4String inGENIE)
{
  TFile *GENIEInput = TFile::Open(inGENIE.c_str());

  m_gtree = (TTree*) GENIEInput->Get("enubetG");

  m_gtree->SetBranchAddress("EvtNum",&gVar.EvtNum);
  m_gtree->SetBranchAddress("StdHepN",&gVar.nPart);
  m_gtree->SetBranchAddress("StdHepPdg",&gVar.pdg);
  m_gtree->SetBranchAddress("StdHepStatus",&gVar.status);
  m_gtree->SetBranchAddress("StdHepP4",&gVar.p4);
  m_gtree->SetBranchAddress("StdHepX4",&gVar.x4);
  m_gtree->SetBranchAddress("EvtVtx",&gVar.vtx);
}

SLArGENIEGeneratorAction::SLArGENIEGeneratorAction() 
  : m_gtree(0)
{}


SLArGENIEGeneratorAction::SLArGENIEGeneratorAction(const G4String genie_file)
  : m_gtree(0)
{
  Initialize(genie_file);
}


SLArGENIEGeneratorAction::~SLArGENIEGeneratorAction()
{}

//***********************************************************************

//***********************************************************************
//************************** EVENT FUNCTIONS ****************************

void SLArGENIEGeneratorAction::GeneratePrimaries(G4Event *ev)
{
  // No idea about the units, need to think about what we need
  
  int evtNum = ev->GetEventID() + m_GENIEInitEvnt;
  m_gtree->GetEntry(evtNum);
  std::cout << "   GENIE TTree event selection: " << evtNum << std::endl;

  size_t particle_idx = 0; // Think this can be done in a better way

  G4ThreeVector vtx(0.,0.,0.);
  vtx.set(gVar.vtx[0]*1E-3, gVar.vtx[1]*1E-3, gVar.vtx[2]*1E-3);
  std::vector<G4PrimaryVertex*> primary_vertices;

  for (int i=0; i<gVar.nPart; i++){

    if (gVar.status[i] == 1){ // 0 - incoming; 1 - outgoing; x - virtual
    G4PrimaryParticle *particle = new G4PrimaryParticle(gVar.pdg[i],
							gVar.p4[i][0]*1E3,
							gVar.p4[i][1]*1E3,
							gVar.p4[i][2]*1E3,
							gVar.p4[i][3]*1E3);
    auto vertex = new G4PrimaryVertex(vtx, 0.); // Not sure which is better here
    //    auto vertex = new G4PrimaryVertex(vtx, gVar.vtx[3]); 
    
    vertex->SetPrimary(particle);
    primary_vertices.push_back(vertex);

    particle_idx++;
    }
  }

  for (const auto& vertex : primary_vertices)
    ev->AddPrimaryVertex(vertex);

}

//***********************************************************************


//***********************************************************************
//***********************************************************************

void SLArGENIEGeneratorAction::SetGENIEEvntExt(G4int evntID)
{
  m_GENIEInitEvnt = evntID;
}



//***********************************************************************
