/**
 * @author      Giulia Conti (unimib), Daniele Guffanti (unimin & infn-mib)
 * @file        multiple_scattering_analysis.cc
 * @created     Fri Jul 21, 2023
 */

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TObjString.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TDatabasePDG.h"
#include "THnBase.h"
#include "THnSparse.h"
#include "TTimer.h"
#include "TMarker.h"

#include "event/SLArMCEvent.hh"
#include "config/SLArCfgAnode.hh"
#include "config/SLArCfgReadoutTile.hh"
#include "config/SLArCfgAssembly.hh"
#include "config/SLArCfgSuperCellArray.hh"

#include "SLArQEventReadout.hh"
#include "SLArQCluster.hh"
#include "SLArQEventAnalysis.hh"

using namespace slarq;

void multiple_scattering_analysis(const TString file_path)
{
  //- - - - - - - - - - - - - - - - - - - - - - Open data file
  TFile *mc_file = new TFile(file_path);
  TTree *mc_tree = (TTree *)mc_file->Get("EventTree");

  SLArMCEvent *ev = 0;
  mc_tree->SetBranchAddress("MCEvent", &ev);

  TH2F *h2_dist = new TH2F("h2_dist", "Distance vs z", 100, 0, 50, 100, -50, 50);
  TCanvas *h2Dist = new TCanvas("h2Dist", "h2Dist", 0, 0, 800, 600);

  auto pdg = TDatabasePDG::Instance();

  for (int iev = 0; iev < 100 /*iev<mc_tree->GetEntries()*/; iev++)
  {
    ev->Reset();
    mc_tree->GetEntry(iev);

    const auto primaries = ev->GetPrimaries(); // vector of primary particles
    const TVector3 true_dir(ev->GetDirection().data());
    std::vector<double> primary_vtx;

    // Get primary e- vertex
    for (const auto &primary : primaries)
    {
      if (primary->GetParticleName() == "e-")
      {
        primary_vtx = primary->GetVertex();
        TVector3 vtx(&primary_vtx.at(0));
        auto trajectories = primary->GetTrajectories();
        for (const auto &t : trajectories)
        {

          auto pdg_particle = pdg->GetParticle(t->GetPDGID());

          if (pdg_particle == pdg->GetParticle(11))
          {
            auto points = t->GetPoints();

            for (const auto &point : points)
            {
              float z = point.fZ;
              TVector3 point_tmp (point.fX, point.fY, point.fZ);
              TVector3 dir_point = vtx + (z * true_dir); 
              double dist = (point_tmp - dir_point).Mag();

              h2_dist->Fill(z - vtx.Z(), dist);
            }
          }
          
        }
      }
    }
  }

  h2_dist->Draw("colz");
}