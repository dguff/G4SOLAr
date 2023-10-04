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
#include "TDatabasePDG.h"

#include "event/SLArMCEvent.hh"
#include "config/SLArCfgAnode.hh"
#include "config/SLArCfgAssembly.hh"
#include "config/SLArCfgSuperCellArray.hh"

void refactor_test_sc(const TString file_path, const int iev) 
{
  gStyle->SetPalette(kBlackBody); 
  TColor::InvertPalette(); 
  TFile* mc_file = new TFile(file_path); 
  TTree* mc_tree = (TTree*)mc_file->Get("EventTree"); 

  //- - - - - - - - - - - - - - - - - - - - - - configuration
  auto PDSSysConfig = (SLArCfgBaseSystem<SLArCfgSuperCellArray>*)mc_file->Get("PDSSysConfig"); 
  std::map<int, SLArCfgAnode*>  AnodeSysCfg;
  AnodeSysCfg.insert( std::make_pair(10, (SLArCfgAnode*)mc_file->Get("AnodeCfg50") ) );
  AnodeSysCfg.insert( std::make_pair(11, (SLArCfgAnode*)mc_file->Get("AnodeCfg51") ) );

/*
 *  std::map<int, TH2Poly*> h2SCArray; 
 *
 *  for (auto &cfgSCArray_ : PDSSysConfig->GetMap()) {
 *    const auto cfgSCArray = cfgSCArray_.second;
 *    printf("SC cfg config: %i - %lu super-cell\n", cfgSCArray_.first, 
 *        cfgSCArray->GetMap().size());
 *    printf("\tposition: [%g, %g, %g] mm\n", 
 *        cfgSCArray->GetPhysX(), cfgSCArray->GetPhysY(), cfgSCArray->GetPhysZ()); 
 *    printf("\tnormal: [%g, %g, %g]\n", 
 *        cfgSCArray->GetNormal().x(), cfgSCArray->GetNormal().y(), cfgSCArray->GetNormal().z() );
 *    printf("\teuler angles: [φ = %g, θ = %g, ψ = %g]\n", 
 *        cfgSCArray->GetPhi()*TMath::RadToDeg(), 
 *        cfgSCArray->GetTheta()*TMath::RadToDeg(), 
 *        cfgSCArray->GetPsi()*TMath::RadToDeg());
 *    cfgSCArray->BuildGShape(); 
 *    auto h2 = cfgSCArray->BuildPolyBinHist(SLArCfgSuperCellArray::kWorld, 25, 25);  
 *    h2SCArray.insert( std::make_pair(cfgSCArray->GetIdx(), h2) ); 
 *  }
 *  printf("\n");
 *
 *  for (const auto& anodeCfg_ : AnodeSysCfg) {
 *    const auto cfgAnode = anodeCfg_.second;
 *    printf("Anode config: %i - %lu mega-tiles\n", cfgAnode->GetIdx(), 
 *        cfgAnode->GetMap().size());
 *    printf("\tposition: [%g, %g, %g] mm\n", 
 *        cfgAnode->GetPhysX(), cfgAnode->GetPhysY(), cfgAnode->GetPhysZ()); 
 *    printf("\tnormal: [%g, %g, %g]\n", 
 *        cfgAnode->GetNormal().x(), cfgAnode->GetNormal().y(), cfgAnode->GetNormal().z() );
 *    printf("\teuler angles: [φ = %g, θ = %g, ψ = %g]\n", 
 *        cfgAnode->GetPhi()*TMath::RadToDeg(), 
 *        cfgAnode->GetTheta()*TMath::RadToDeg(), 
 *        cfgAnode->GetPsi()*TMath::RadToDeg());
 *  }
 *  printf("\n");
 *  
 *  TH2D* h2_30 = new TH2D("sc_top_30", "30 sc top", 50, -1.5e3, 1.5e3, 50, -1200, 1200); 
 *  h2_30->Draw("axis");
 *  h2SCArray.find(30)->second->Draw("col same"); 
 */
  TH1D* hTime = new TH1D("hPhTime", "Photon hit time;Time [ns];Entries", 1000, 0, 5e3); 

  TH1D* hBacktracker = new TH1D("hBacktracker", "backtracker: trkID", 1000, 0, 1000 );

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
  mc_tree->GetEntry(iev); 

  auto primaries = ev->GetPrimaries(); 

  auto andMap = ev->GetEventAnode(); 

  for (const auto &anode_ : andMap) {
    auto anode = anode_.second; 
    int tpc_id = anode_.first; 
    auto hAnode = AnodeSysCfg[tpc_id]->GetAnodeMap(0); 
    std::vector<TH2Poly*> h2mt; h2mt.reserve(50); 
    std::vector<TH2Poly*> h2pix; h2pix.reserve(500); 

    double z_max = 0; 
    printf("ANODE %i:\n", anode->GetID());
    for (const auto &mt: anode->GetMegaTilesMap()) {
      auto h2mt_ = AnodeSysCfg[tpc_id]->ConstructPixHistMap(1, std::vector<int>{mt.first}); 
      h2mt.push_back( h2mt_ ); 
      //printf("\tMegatilte %i: %i hits\n", mt.first, mt.second->GetNChargeHits());
      if (mt.second->GetNChargeHits() == 0) continue;
      for (auto &t : mt.second->GetTileMap()) {
        //printf("\t\tTilte %i: %g hits\n", t.first, t.second->GetNPixelHits());
        if (t.second->GetPixelHits() == 0 ) continue;
        auto h2t_ = AnodeSysCfg[tpc_id]->ConstructPixHistMap(2, std::vector<int>{mt.first, t.first}); 
        for (const auto &p : t.second->GetPixelEvents()) {
          //printf("\t\t\tPixel %i has %i hits\n", p.first, p.second->GetNhits());
          h2t_->SetBinContent( p.first, p.second->GetNhits() );
          p.second->PrintHits();
          if (p.second->GetNhits() > z_max) z_max = p.second->GetNhits(); 
        }
        h2pix.push_back( h2t_ ); 
      }
    }

    for (const auto &mt: anode->GetMegaTilesMap()) {
      if (mt.second->GetNPhotonHits() == 0) continue;
      for (auto &t : mt.second->GetTileMap()) {
        if (t.second->GetHits().empty()) continue;
        for (const auto &p : t.second->GetHits()) {
          hTime->Fill( p.first, p.second );  
        }

        if (t.second->GetBacktrackerRecordSize() > 0) {
          for (const auto &backtracker : t.second->GetBacktrackerRecordCollection()) {
            auto records = backtracker.second.GetConstRecords();
            auto recordTrkID = records.at(0);
            for (const auto &trkID : recordTrkID.GetConstCounter()) {
              hBacktracker->Fill( trkID.first, trkID.second );
            }
          }
        }
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

    auto pdg = TDatabasePDG::Instance(); 

    for (const auto &p : primaries) {
      printf("----------------------------------------\n");
      printf("PRIMARY vertex: %s - K0 = %2f - t = %.2f - vtx [%.1f, %.1f, %.1f]\n", 
          p->GetParticleName().Data(), p->GetEnergy(), p->GetTime(), 
          p->GetVertex()[0], p->GetVertex()[1], p->GetVertex()[2]);
      auto trajectories = p->GetTrajectories(); 
      for (const auto &t : trajectories) {
        auto points = t->GetPoints(); 
        auto pdg_particle = pdg->GetParticle(t->GetPDGID()); 
        //printf("%s [%i]: t = %.2f, K = %.2f - n_scint = %g, n_elec = %g\n", 
            //t->GetParticleName().Data(), t->GetTrackID(), 
            //t->GetTime(),
            //t->GetInitKineticEne(), 
            //t->GetTotalNph(), t->GetTotalNel());
        if (t->GetInitKineticEne() < 0.01) continue;
        TGraph g; 
        Color_t col = kBlack; 
        TString name = ""; 

        if (!pdg_particle) {
          col = kBlack; 
          name = Form("g_%i_trk%i", t->GetPDGID(), t->GetTrackID()); 
        }
        else {
          if (pdg_particle == pdg->GetParticle(22)) col = kYellow;
          else if (pdg_particle == pdg->GetParticle( 11)) col = kBlue-6;
          else if (pdg_particle == pdg->GetParticle(-11)) col = kRed-7;
          else if (pdg_particle == pdg->GetParticle(2212)) col = kRed; 
          else if (pdg_particle == pdg->GetParticle(2112)) col = kBlue;
          else if (pdg_particle == pdg->GetParticle(-211)) col = kOrange+7; 
          else if (pdg_particle == pdg->GetParticle( 211)) col = kViolet-2; 
          else if (pdg_particle == pdg->GetParticle( 111)) col = kGreen; 
          else    col = kGray+2;
          name = Form("g_%s_trk_%i", pdg_particle->GetName(), t->GetTrackID()); 
        }
        
        for (const auto &pt : points) {
          if (pt.fCopy == tpc_id) g.AddPoint( 
              TVector3(pt.fX, pt.fY, pt.fZ).Dot( AnodeSysCfg[tpc_id]->GetAxis0()), 
              TVector3(pt.fX, pt.fY, pt.fZ).Dot( AnodeSysCfg[tpc_id]->GetAxis1()) );
        }

        g.SetName(name); 
        g.SetLineColor(col); 
        g.SetLineWidth(2);
        if (g.GetN() > 2) g.DrawClone("l");
      }
    }
  }

  auto pdsMap = ev->GetEventSuperCellArray(); 
  for (const auto &scArray : pdsMap) {
    int n_sc = 0; 

    printf("Array: %i - %i hits\n", scArray.first, scArray.second->GetNhits());
    for (const auto &sc : scArray.second->GetSuperCellMap()) {
      auto n = sc.second->GetNhits(); 
      //printf("SC %i recorded %i hits\n", sc.second->GetIdx(), n); 
      n_sc += n;

      for (const auto &h : sc.second->GetHits()) {
        hTime->Fill( h.first, h.second ); 
      }
    }

    //printf("SC counting: %i\n", n_sc);
  }

  TCanvas* cTime = new TCanvas(); 
  hTime->Draw("hist");

  return;
}

