/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : test_qbin
 * @created     : mercoledì nov 09, 2022 12:47:46 CET
 */

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TRandom3.h"

#include "config/SLArCfgSystemPix.hh"
#include "config/SLArCfgBaseSystem.hh"
#include "config/SLArCfgMegaTile.hh"
#include "config/SLArCfgReadoutTile.hh"
#include "event/SLArMCEvent.hh"
#include "event/SLArEventMegatile.hh"
#include "event/SLArEventTile.hh"

void test_qbin(const char* input_path, int iev = 0) 
{
  TFile* file = new TFile(input_path); 
  TTree* tree = (TTree*)file->Get("EventTree"); 
  SLArCfgSystemPix* anodeCfg = (SLArCfgSystemPix*)file->Get("PixSysConfig"); 

  SLArMCEvent* ev = nullptr; 
  tree->SetBranchAddress("MCEvent", &ev); 
  tree->GetEntry(iev); 
  double hmax = 0.;

  // read events
  auto anodeEv = ev->GetReadoutTileSystem(); 
  for (const auto& mtEv : anodeEv->GetMegaTilesMap()) {
    for (const auto& tEv : mtEv.second->GetTileMap()) {
      for (const auto& pix : tEv.second->GetPixelEvents()) {
        double nhits = pix.second->GetNhits(); 
        if (nhits > hmax) hmax = nhits;
        anodeCfg->GetMap()[mtEv.first]->GetMap()[tEv.first]->GetPixHistMap()->Fill(pix.first, pix.second->GetNhits()); 
      }
    }
  }


  // Draw pixel hit-map
  new TCanvas(); 
  TH1D* h = new TH1D("hFrame", "Anode readout;#it{z} [mm];#it{x} [mm]", 100, -3e3, +3e3); 
  h->GetYaxis()->SetRangeUser(-2e3, +2e3); 
  h->Draw("axis"); 

  for (const auto& mtCfg : anodeCfg->GetMap()) {
    mtCfg.second->GetTH2()->Draw("same"); 
    for (const auto& tCfg : mtCfg.second->GetMap()) {
      tCfg.second->GetPixHistMap()->GetZaxis()->SetRangeUser(0., 1.05*hmax); 
      tCfg.second->GetPixHistMap()->DrawClone("col same"); 
    }
  }

  // Draw electron tracks
  auto primaries = ev->GetPrimaries(); 
  for (const auto& pp : primaries) {
    auto trajectories = pp->GetTrajectories(); 
    for (const auto &tt : trajectories) {
      if (tt->GetInitKineticEne() > 0.1 && tt->GetPDGID() == 11) {
        TGraph* g = new TGraph(tt->GetPoints().size()); 
        int ip = 0; 
        for (const auto &tp : tt->GetPoints())  {
          g->SetPoint(ip, tp.fZ, tp.fY); ip++;
        }
        g->SetLineWidth(3); 
        g->DrawClone("lsame"); 
      }
    }
  }

  

  return;
}

