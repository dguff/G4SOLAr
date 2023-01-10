/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : test_qbin
 * @created     : mercoledì nov 09, 2022 12:47:46 CET
 */

#include <iostream>
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TDatabasePDG.h"

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
  std::vector<TH2Poly*> pixMaps;
  pixMaps.reserve(200); 

  auto anodeEv = ev->GetReadoutTileSystem(); 

  for (const auto& mtEv : anodeEv->GetMegaTilesMap()) {
    auto mt_hits = mtEv.second->GetNChargeHits(); 
    if (mt_hits == 0) continue;

    for (const auto& tEv : mtEv.second->GetTileMap()) {
      auto t_hits = tEv.second->GetPixelHits();
      if (t_hits == 0) continue;

      TH2Poly* h2Map = anodeCfg->ConstructPixHistMap(mtEv.first, tEv.first);

      for (const auto& pix : tEv.second->GetPixelEvents()) {
        double nhits = pix.second->GetNhits(); 
        if (nhits > hmax) hmax = nhits;
        h2Map->SetBinContent(pix.first, nhits); 
        //anodeCfg->GetMap()[mtEv.first]->GetMap()[tEv.first]->GetPixHistMap()->SetBinContent(pix.first, nhits); 
      }
      printf("charge map pointer %p\n", static_cast<void*>(h2Map));
      printf("Hist name: %s\n", h2Map->GetName());

      pixMaps.push_back(h2Map); 
    }
  }

  // Draw pixel hit-map
  TH2D* h = new TH2D("hFrame", "Anode readout;#it{z} [mm];#it{x} [mm]", 
      100, -6e3, +6e3, 100, -3e3, 3e3); 

  TCanvas* cP = new TCanvas("cP"); 
  cP->cd(); h->Draw("axis"); 

  bool is_first = true; 
  for (size_t j=0; j<pixMaps.size(); j++) {
    auto qtile = pixMaps.at(j); 
    printf("qtile ptr = %p\n", static_cast<void*>(qtile));
    qtile->GetZaxis()->SetRangeUser(0, 1.05*hmax); 
    if (is_first) {qtile->Draw("colz0 same"); is_first = false;}
    else qtile->Draw("col0 same"); 
  }

  // Draw electron tracks
  auto pdgDB = TDatabasePDG::Instance();
  double q = 0;
  auto primaries = ev->GetPrimaries(); 
  for (const auto& pp : primaries) {
    auto trajectories = pp->GetTrajectories(); 
    for (const auto &tt : trajectories) {
      auto particle = pdgDB->GetParticle(tt->GetPDGID()); 
      if (particle) q = particle->Charge(); 
      else          q = 1; 
      if (tt->GetInitKineticEne() > 0.1 && q != 0) {
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

