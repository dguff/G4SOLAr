/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        radiological.C
 * @created     Sun Apr 30, 2023 17:40:16 CEST
 */

#include <iostream>
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

#include "event/SLArMCEvent.hh"
#include "config/SLArCfgAnode.hh"
#include "config/SLArCfgAssembly.hh"
#include "config/SLArCfgSuperCellArray.hh"

void radiological(const char* filename) 
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

  double origin_k;
  double lar_edep;
  double lar_edep_temp;
  int    origin_vol; 
  int    final_vol; 
  int    pdgID; 
  int    trkID; 
  int    parentID; 
  int    iEv; 

  TTree* tree = new TTree("externals", "external events"); 
  tree->Branch("iEv", &iEv); 
  tree->Branch("pdgID", &pdgID); 
  tree->Branch("trkID", &trkID); 
  tree->Branch("parentID", &parentID); 
  tree->Branch("origin_k", &origin_k); 
  tree->Branch("lar_edep", &lar_edep); 
  tree->Branch("lar_edep_temp", &lar_edep_temp); 
  tree->Branch("origin_vol", &origin_vol); 
  tree->Branch("final_vol", &final_vol); 

  //- - - - - - - - - - - - - - - - - - - - - - Access event
  SLArMCEvent* ev = 0; 
  mc_tree->SetBranchAddress("MCEvent", &ev); 

  for (int i=0; i<mc_tree->GetEntries(); i++) {

    if (i%1000 == 0) printf("processing ev %i\n", i); 
    mc_tree->GetEntry(i); 

    iEv = ev->GetEvNumber(); 
    
    const auto primaries = ev->GetPrimaries(); 

    for (const auto &primary : primaries) {

      const auto tracks = primary->GetTrajectories(); 
      lar_edep = primary->GetTotalLArEdep();  
      pdgID = primary->GetID(); 
      trkID = primary->GetTrackID(); 
      origin_k = primary->GetEnergy(); 

      lar_edep_temp = 0; 

        for (const auto &trk : tracks) {
          const double pdgID = trk->GetPDGID(); 
          const int _trkID = trk->GetTrackID(); 

          if (_trkID == trkID) {
            parentID = trk->GetParentID(); 
            const auto start_pos = trk->GetPoints().front(); 
            const auto stop_pos = trk->GetPoints().back(); 
            origin_vol = start_pos.fCopy; 
            final_vol  = stop_pos .fCopy; 
          }
          const auto t = trk->GetPoints(); 
          int copy = t.front().fCopy; 
          size_t ipoint = 0; 

          for (const auto &pt : t ) {
            if (ipoint > 0 && (copy == 10 || copy == 11)) {
              const double edep = pt.fEdep; 
              lar_edep_temp += edep;
            }
            ipoint++; 
          }
        }

        tree->Fill(); 
    }
  }

  tree->Write(); 


  return;
}

