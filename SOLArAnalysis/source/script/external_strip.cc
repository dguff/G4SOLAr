/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        external_strip.cc
 * @created     Mon Apr 24, 2023 14:56:31 CEST
 */

#include <iostream>
#include <iterator>
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TDatabasePDG.h"
#include <ROOT/RDataFrame.hxx>

#include "event/SLArMCEvent.hh"
#include "event/SLArMCPrimaryInfo.hh"
#include "config/SLArCfgAnode.hh"
#include "config/SLArCfgAssembly.hh"
#include "config/SLArCfgSuperCellArray.hh"

#include "SLArAnalysisLibSetup.hpp"
#include "progressbar.hpp"

const SLArEventTrajectory* get_parent_track(const int parentID, const std::vector<SLArEventTrajectory*> trajectories) {
  SLArEventTrajectory* parentTrack = nullptr; 
  for (size_t k=0; k<trajectories.size(); k++) {
    if (parentID == trajectories.at(k)->GetTrackID()) {
      parentTrack = trajectories.at(k); 
      break;
    }
  }
  return parentTrack;
}

struct TTrkData_t {
  TTrkData_t() {}; 
  void reset() {
    fOrigin_pos = {0, 0, 0}; 
    fFinal_pos = {0, 0, 0}; 
    fOrigin_energy = 0;
    fLAr_origin_energy = 0; 
    fLAr_edep = 0;
    fWeight = 0; 
    fOrigin_vol = 0; 
    fFinal_vol = 0; 
    fPdgID = 0; 
    fTrkID = 0; 
    fParentID = 0; 
    fIEv = 0; 
    fTerminator = "null";          
    fCreator = "null"; 
  }

  TVector3 fOrigin_pos; 
  TVector3 fFinal_pos; 
  double  fOrigin_energy;
  double  fLAr_origin_energy; 
  double  fLAr_edep;
  double  fWeight; 
  int     fOrigin_vol; 
  int     fFinal_vol; 
  int     fPdgID; 
  int     fTrkID; 
  int     fParentID; 
  int     fIEv; 
  TString fTerminator; 
  TString fCreator; 
}; 


void external_strip(const char* filename) 
{
  
  gStyle->SetPalette(kBlackBody); 
  TColor::InvertPalette(); 
  TFile* mc_file = new TFile(filename); 
  TTree* mc_tree = (TTree*)mc_file->Get("EventTree"); 

  //- - - - - - - - - - - - - - - - - - - - - - Setup output
  TString output_name = filename; 
  output_name.Resize(output_name.Index(".root")); 
  output_name += "_output.root"; 
  TFile* output = new TFile(output_name, "recreate");

  TTrkData_t data;

  TTree* tree = new TTree("externals", "external events"); 
  tree->Branch("iEv"               , &data.fIEv);
  tree->Branch("pdgID"             , &data.fPdgID);
  tree->Branch("trkID"             , &data.fTrkID);
  tree->Branch("parentID"          , &data.fParentID);
  tree->Branch("origin_pos"        , &data.fOrigin_pos);
  tree->Branch("final_pos"         , &data.fFinal_pos);
  tree->Branch("origin_energy"     , &data.fOrigin_energy);
  tree->Branch("lar_origin_energy" , &data.fLAr_origin_energy);
  tree->Branch("lar_edep"          , &data.fLAr_edep);
  tree->Branch("origin_vol"        , &data.fOrigin_vol);
  tree->Branch("final_vol"         , &data.fFinal_vol);
  tree->Branch("weight"            , &data.fWeight);
  tree->Branch("creator"           , &data.fCreator);
  tree->Branch("terminator"        , &data.fTerminator);

  //- - - - - - - - - - - - - - - - - - - - - - Define event processing
  auto process_event = [&](SLArMCEvent* ev) {

    const auto primaries = ev->GetPrimaries(); 
    const auto tracks = primaries.front()->GetTrajectories(); 

    for (const auto &trk : tracks) {
      data.reset(); 

      data.fIEv = ev->GetEvNumber(); 
      data.fPdgID = trk->GetPDGID(); 
      data.fTrkID = trk->GetTrackID(); 
      data.fWeight = trk->GetWeight(); 
      data.fParentID = trk->GetParentID(); 
      data.fOrigin_energy = trk->GetInitKineticEne(); 
      data.fCreator = trk->GetCreatorProcess(); 
      data.fTerminator = trk->GetEndProcess(); 

      const auto parent = get_parent_track(data.fParentID, tracks); 
      if (parent && data.fPdgID == 22) {
        if (parent->GetEndProcess().Contains("nCapture") || 
            parent->GetEndProcess().Contains("Inelastic")) {
          data.fCreator = parent->GetEndProcess(); 
        }
      }

      const auto start_pos = trk->GetPoints().front(); 
      const auto stop_pos = trk->GetPoints().back(); 
      data.fOrigin_pos[0] = start_pos.fX; data.fFinal_pos[0] = stop_pos.fX; 
      data.fOrigin_pos[1] = start_pos.fY; data.fFinal_pos[1] = stop_pos.fY; 
      data.fOrigin_pos[2] = start_pos.fZ; data.fFinal_pos[2] = stop_pos.fZ; 

      data.fOrigin_vol    = start_pos.fCopy; 
      data.fFinal_vol     = stop_pos.fCopy; 

      auto t = trk->GetPoints(); 
      
      for (std::vector<trj_point>::iterator pt_itr = t.begin(); pt_itr < t.end(); pt_itr++) {
        const auto pt = *pt_itr;
        if (pt.fLAr) {
          if (data.fLAr_origin_energy == 0) {
            data.fLAr_origin_energy = pt.fKEnergy;
          } 
          if (pt_itr != t.begin()){
            data.fLAr_edep += pt.fEdep; 
          }
        } 
      }

      tree->Fill(); 
    }
  };

  //- - - - - - - - - - - - - - - - - - - - - - Access event
  SLArMCEvent* ev = nullptr; 
  mc_tree->SetBranchAddress("MCEvent", &ev); 
  progressbar prog(mc_tree->GetEntries()); 
  prog.set_done_char("-"); 
  for (int i=0; i<mc_tree->GetEntries(); i++) {
    prog.update(); 
    mc_tree->GetEntry(i); 
    process_event(ev); 
  }

  tree->Write(); 

  output->Close(); 

  return;
}

int main(int argc, char *argv[])
{
  setup_slar_lib(); 
 
  printf("processing %s\n", argv[1]);

  external_strip(argv[1]); 
  //printf("I have created event %i\n", ev->GetEvNumber());
  //TFile test("test.root", "recreate"); 
  //SLArMCEvent* ev = new SLArMCEvent(); 
  //ev->SetEvNumber(10); 
  //ev->Write("myEvent"); 
  //test.Close(); 

  return 0;
}

