/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        refactor_test_sc.C
 * @created     Mon Mar 27, 2023 11:19:53 CEST
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

#include "event/SLArMCEvent.hh"
#include "config/SLArCfgAnode.hh"
#include "config/SLArCfgAssembly.hh"
#include "config/SLArCfgSuperCellArray.hh"

void refactor_test_sc() 
{
  gStyle->SetPalette(kBlackBody); 
  TColor::InvertPalette(); 
  TFile* mc_file = new TFile("../install/electrons.root"); 
  TTree* mc_tree = (TTree*)mc_file->Get("EventTree"); 

  //- - - - - - - - - - - - - - - - - - - - - - Access readout configuration
  auto PDSSysConfig = (SLArCfgBaseSystem<SLArCfgSuperCellArray>*)mc_file->Get("PDSSysConfig"); 
  std::map<int, SLArCfgAnode*>  AnodeSysCfg;
  AnodeSysCfg.insert( std::make_pair(10, (SLArCfgAnode*)mc_file->Get("AnodeCfg50") ) );
  AnodeSysCfg.insert( std::make_pair(11, (SLArCfgAnode*)mc_file->Get("AnodeCfg51") ) );

  std::map<int, TH2Poly*> h2SCArray; 

  for (auto &cfgSCArray_ : PDSSysConfig->GetMap()) {
    const auto cfgSCArray = cfgSCArray_.second;
    printf("SC cfg config: %i - %lu super-cell\n", cfgSCArray_.first, 
        cfgSCArray->GetMap().size());
    printf("\tposition: [%g, %g, %g] mm\n", 
        cfgSCArray->GetPhysX(), cfgSCArray->GetPhysY(), cfgSCArray->GetPhysZ()); 
    printf("\tnormal: [%g, %g, %g]\n", 
        cfgSCArray->GetNormal().x(), cfgSCArray->GetNormal().y(), cfgSCArray->GetNormal().z() );
    printf("\teuler angles: [φ = %g, θ = %g, ψ = %g]\n", 
        cfgSCArray->GetPhi()*TMath::RadToDeg(), 
        cfgSCArray->GetTheta()*TMath::RadToDeg(), 
        cfgSCArray->GetPsi()*TMath::RadToDeg());
    cfgSCArray->BuildGShape(); 
    auto h2 = cfgSCArray->BuildPolyBinHist(SLArCfgSuperCellArray::kWorld, 25, 25);  
    h2SCArray.insert( std::make_pair(cfgSCArray->GetIdx(), h2) ); 
  }
  printf("\n");

  for (const auto& anodeCfg_ : AnodeSysCfg) {
    const auto cfgAnode = anodeCfg_.second;
    printf("Anode config: %i - %lu mega-tiles\n", cfgAnode->GetIdx(), 
        cfgAnode->GetMap().size());
    printf("\tposition: [%g, %g, %g] mm\n", 
        cfgAnode->GetPhysX(), cfgAnode->GetPhysY(), cfgAnode->GetPhysZ()); 
    printf("\tnormal: [%g, %g, %g]\n", 
        cfgAnode->GetNormal().x(), cfgAnode->GetNormal().y(), cfgAnode->GetNormal().z() );
    printf("\teuler angles: [φ = %g, θ = %g, ψ = %g]\n", 
        cfgAnode->GetPhi()*TMath::RadToDeg(), 
        cfgAnode->GetTheta()*TMath::RadToDeg(), 
        cfgAnode->GetPsi()*TMath::RadToDeg());
  }
  printf("\n");
  
  TH2D* h2_30 = new TH2D("sc_top_30", "30 sc top", 50, -1.5e3, 1.5e3, 50, -1200, 1200); 
  h2_30->Draw("axis");
  h2SCArray.find(30)->second->Draw("col same"); 

  //- - - - - - - - - - - - - - - - - - - - - - Access event
  SLArMCEvent* ev = 0; 
  mc_tree->SetBranchAddress("MCEvent", &ev); 
  mc_tree->GetEntry(0); 

  auto primaries = ev->GetPrimaries(); 

  auto andMap = ev->GetEventAnode(); 

  for (const auto &anode_ : andMap) {
    auto anode = anode_.second; 
    int tpc_id = anode_.first; 
    auto hAnode = AnodeSysCfg[tpc_id]->GetAnodeMap(0); 
    std::vector<TH2Poly*> h2mt; h2mt.reserve(50); 
    std::vector<TH2Poly*> h2pix; h2pix.reserve(500); 

    double z_max = 0; 

    for (const auto &mt: anode->GetMegaTilesMap()) {
      auto h2mt_ = AnodeSysCfg[tpc_id]->ConstructPixHistMap(1, std::vector<int>{mt.first}); 
      h2mt.push_back( h2mt_ ); 
      if (mt.second->GetNChargeHits() == 0) continue;
      for (auto &t : mt.second->GetTileMap()) {
        if (t.second->GetNPixelHits() == 0) continue;
        auto h2t_ = AnodeSysCfg[tpc_id]->ConstructPixHistMap(2, std::vector<int>{mt.first, t.first}); 
        for (const auto &p : t.second->GetPixelEvents()) {
          h2t_->SetBinContent( p.first, p.second->GetNhits() );
          if (p.second->GetNhits() > z_max) z_max = p.second->GetNhits(); 
        }
        h2pix.push_back( h2t_ ); 
      }
    }

    TCanvas* c = new TCanvas(Form("cTPC%i", tpc_id), Form("TPC %i", tpc_id), 0, 0, 800, 500); 
    c->SetTicks(1, 1); 
    hAnode->Draw(); 
    for (const auto &hmt : h2mt) hmt->Draw("same"); 
    for (auto &ht : h2pix) {
      ht->GetZaxis()->SetRangeUser(0, z_max*1.05); 
      ht->Draw("col same"); 
    }

    for (const auto &p : primaries) {
      auto trajectories = p->GetTrajectories(); 
      for (const auto &t : trajectories) {
        auto points = t->GetPoints(); 
        TGraph g; 
        for (const auto &pt : points) {
          if (pt.fCopy == tpc_id) g.AddPoint( 
              TVector3(pt.fX, pt.fY, pt.fZ).Dot( AnodeSysCfg[tpc_id]->GetAxis0()), 
              TVector3(pt.fX, pt.fY, pt.fZ).Dot( AnodeSysCfg[tpc_id]->GetAxis1()) );
        }

        if (g.GetN() > 0) {
          if (t->GetParticleName() == "e-") g.SetLineColor(kBlack); 
          else if (t->GetParticleName() == "gamma") g.SetLineColor(kYellow);
          else g.SetLineColor(kGray+1);
          g.SetLineWidth(2); 
          g.DrawClone("l");
        }
      }
    }



  }

  auto pdsMap = ev->GetEventSuperCellArray(); 
  for (const auto &scArray : pdsMap) {
    int n_sc = 0; 

    printf("Array: %i - %i hits\n", scArray.first, scArray.second->GetNhits());
    for (const auto &sc : scArray.second->GetSuperCellMap()) {
      auto n = sc.second->GetNhits(); 
      printf("SC %i recorded %i hits\n", sc.second->GetIdx(), n); 
      n_sc += n;
    }

    printf("SC counting: %i\n", n_sc);
  }
  return;
}

