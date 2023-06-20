/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        ext_neutrons
 * @created     lunedì apr 24, 2023 14:56:31 CEST
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

void ext_neutrons(const char* filename) 
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

  double origin_pos[3]; 
  double final_pos[3]; 
  double origin_energy;
  double lar_edep;
  double weight; 
  int    origin_vol; 
  int    final_vol; 
  int    pdgID; 
  int    trkID; 
  int    parentID; 
  int    iEv; 
  TString destroyer; 
  TString creator; 

  TTree* tree = new TTree("externals", "external events"); 
  tree->Branch("iEv", &iEv); 
  tree->Branch("pdgID", &pdgID); 
  tree->Branch("trkID", &trkID); 
  tree->Branch("parentID", &parentID); 
  tree->Branch("origin_pos", origin_pos, "g_origin_pos[3]/D"); 
  tree->Branch("final_pos", final_pos, "g_final_pos[3]/D"); 
  tree->Branch("origin_energy", &origin_energy); 
  tree->Branch("lar_edep", &lar_edep); 
  tree->Branch("origin_vol", &origin_vol); 
  tree->Branch("final_vol", &final_vol); 
  tree->Branch("weight", &weight); 
  tree->Branch("creator", &creator); 
  tree->Branch("destroyer", &destroyer); 


  
  //- - - - - - - - - - - - - - - - - - - - - - Access event
  SLArMCEvent* ev = 0; 
  mc_tree->SetBranchAddress("MCEvent", &ev); 



  TH1D* hEnergyNeutron = new TH1D("hEnergyNeutron", 
      "Externals Energy;Energy [MeV];Entries", 100, 0, 20);
  TH1D* hEnergyGamma = new TH1D("hEnergyGamma", 
      "Externals Energy;Energy [MeV];Entries", 100, 0, 20);
  TH3D* h3OriginPoint = new TH3D("h3OriginPoint", "primary origin;x [mm];z [mm];y [mm]",
      70, -3.3e3, 3.3e3, 90, -3.5e3, 3.5e3, 70, -3e3, 3e3); 
  TH3D* h3EndPoint = new TH3D("h3EndPoint", "Neutron end point;x [mm];z [mm];y [mm]",
      70, -3.3e3, 3.3e3, 90, -3.5e3, 3.5e3, 70, -3e3, 3e3); 

  TH3D* h3GammaOriginPoint = new TH3D("h3GammaOriginPoint", "Gamma origin;x [mm];z [mm];y [mm]",
      70, -3.3e3, 3.3e3, 90, -3.5e3, 3.5e3, 70, -3e3, 3e3); 
  TH3D* h3GammaEndPoint = new TH3D("h3GammaEndPoint", "Gamma end point;x [mm];z [mm];y [mm]",
      70, -3.3e3, 3.3e3, 90, -3.5e3, 3.5e3, 70, -3e3, 3e3); 
  TH1D* hStopVolumeNeutron = new TH1D("hStopVolumeNeutron", 
      "Stop volume copy number - #gamma;Volume copy number;Entries",1000,0,1000);
  TH1D* hStopVolumeGamma   = new TH1D("hStopVolumeGamma", 
      "Stop volume copy number - n;Volume copy number;Entries",1000,0,1000);
  TH1D* hNeutronEnergyLAr  = new TH1D("hEnergyNeutronLAr", 
      "Neutron Energy (LAr);Energy [MeV];Entries", 100, 0, 20); 
  TH1D* hGammaEnergyLAr  = new TH1D("hEnergyGammaLAr", 
      "Gamma Energy (LAr);Energy [MeV];Entries", 100, 0, 20); 


  for (int i=0; i<mc_tree->GetEntries(); i++) {

    if (i%1000 == 0) printf("processing ev %i\n", i); 
    mc_tree->GetEntry(i); 

    iEv = ev->GetEvNumber(); 
    
    const auto primaries = ev->GetPrimaries(); 
    const int primary_id = primaries.front()->GetTrackID(); 
    const auto tracks = primaries.front()->GetTrajectories(); 

    hEnergyNeutron->Fill( primaries.front()->GetEnergy() ); 

    const auto primary_start = tracks.front()->GetPoints().front(); 
    const auto primary_stop  = tracks.front()->GetPoints().back(); 

    h3OriginPoint->Fill( primary_start.fX, primary_start.fZ, primary_start.fY );
    h3EndPoint->Fill( primary_stop.fX, primary_stop.fZ, primary_stop.fY );

    for (const auto &trk : tracks) {
      pdgID = trk->GetPDGID(); 
      trkID = trk->GetTrackID(); 
      weight = trk->GetWeight(); 
      parentID = trk->GetParentID(); 
      origin_energy = trk->GetInitKineticEne(); 
      creator = trk->GetCreatorProcess(); 
      destroyer = trk->GetEndProcess(); 

      const auto parent = get_parent_track(parentID, tracks); 
      if (parent && pdgID == 22) {
        if (parent->GetEndProcess().Contains("nCapture") || 
            parent->GetEndProcess().Contains("Inelastic")) {
          //printf("Update creator process info: %s -> %s\n", 
              //creator.Data(), parent->GetEndProcess().Data());
          creator = parent->GetEndProcess(); 
        }
      }

      const auto start_pos = trk->GetPoints().front(); 
      const auto stop_pos = trk->GetPoints().back(); 
      origin_pos[0] = start_pos.fX; final_pos[0] = stop_pos.fX; 
      origin_pos[1] = start_pos.fY; final_pos[1] = stop_pos.fY; 
      origin_pos[2] = start_pos.fZ; final_pos[2] = stop_pos.fZ; 
      
      origin_vol    = start_pos.fCopy; 
      final_vol     = stop_pos.fCopy; 

      const auto t = trk->GetPoints(); 
      int copy = t.front().fCopy; 
      size_t ipoint = 0; 
      while ( copy != 9 && copy != 10 && copy != 11 && ipoint < t.size()-1 ) {
        ipoint++; 
        copy = t.at(ipoint).fCopy; 
      }
      (ipoint < t.size() - 2) ? lar_edep = t.at(ipoint).fKEnergy : lar_edep = 0.; 

      tree->Fill(); 
    }
  }

  tree->Write(); 
  hEnergyNeutron->Write();
  h3OriginPoint->Write(); 
  h3EndPoint->Write(); 



  return;
}

