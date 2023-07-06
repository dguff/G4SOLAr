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

void event_display_test(const TString file_path, const int iev) 
{
  gStyle->SetPalette(kBlackBody); 
  TColor::InvertPalette(); 
  TFile* mc_file = new TFile(file_path); 
  TTree* mc_tree = (TTree*)mc_file->Get("EventTree");

  //- - - - - - - - - - - - - - - - - - - - - - Access readout configuration
 
  std::map<int, SLArCfgAnode*>  AnodeSysCfg;
  AnodeSysCfg.insert( std::make_pair(10, (SLArCfgAnode*)mc_file->Get("AnodeCfg50") ) );
  AnodeSysCfg.insert( std::make_pair(11, (SLArCfgAnode*)mc_file->Get("AnodeCfg51") ) );

  std::map<int, TH2Poly*> h2SCArray; 

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
  

  //- - - - - - - - - - - - - - - - - - - - - - Access event
  SLArMCEvent* ev = 0; 
  mc_tree->SetBranchAddress("MCEvent", &ev); 
  mc_tree->GetEntry(iev);
    
  TH1F* charged_time = new TH1F ("charged_time", "Time of arrival; Time [#mu s]; Counts", 1000, 0, 1000);
    
  auto primaries = ev->GetPrimaries(); //vector 

  auto andMap = ev->GetEventAnode() ; // mappa con i  2 anodi     

  for (const auto &anode_ : andMap) {
    auto anode = anode_.second; // valore
    int tpc_id = anode_.first; // chiave
    auto hAnode = AnodeSysCfg[tpc_id]->GetAnodeMap(0); // megatile 
    std::vector<TH2Poly*> h2mt; h2mt.reserve(50);    // vettore di istogrammi di megatile
    std::vector<TH2Poly*> h2pix; h2pix.reserve(500); // vettore di istogrammi di pixel

    double z_max = 0; 
    printf("ANODE %i:\n", anode->GetID());

    for (const auto &mt: anode->GetMegaTilesMap()) {  // loop su gruppi di tile
      auto h2mt_ = AnodeSysCfg[tpc_id]->ConstructPixHistMap(1, std::vector<int>{mt.first}); // costruisce istogramma megatile (con dentro le tile) 
      h2mt.push_back( h2mt_ ); 
      //printf("\tMegatilte %i: %i hits\n", mt.first, mt.second->GetNChargeHits());
      if (mt.second->GetNChargeHits() == 0) continue;
      for (auto &t : mt.second->GetTileMap()) { // loop sulle tile
        //printf("\t\tTilte %i: %g hits\n", t.first, t.second->GetNPixelHits());
        if (t.second->GetPixelHits() == 0 ) continue;
        auto h2t_ = AnodeSysCfg[tpc_id]->ConstructPixHistMap(2, std::vector<int>{mt.first, t.first}); // creo l'istogramma per i pixel
        for (const auto &p : t.second->GetPixelEvents()) {
          //printf("\t\t\tPixel %i has %i hits\n", p.first, p.second->GetNhits());
          h2t_->SetBinContent( p.first, p.second->GetNhits() ); // riempio istogramma dei pixel
            auto electron_hits = p.second->GetHits();
            for (const auto &hit : electron_hits) {
                charged_time->Fill(hit->GetTime()*0.001);
            }
          if (p.second->GetNhits() > z_max) z_max = p.second->GetNhits(); // carica massima per colori
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

    TCanvas * CTime = new TCanvas ("CTime", "Arrival time distribution", 0, 0, 800, 600);
    charged_time->Draw();

  return;
}



void hit_vs_distance (const TString file_path){

  TFile* mc_file = new TFile(file_path);
  TTree* mc_tree = (TTree*)mc_file->Get("EventTree");
    
  //- - - - - - - - - - - - - - - - - - - - - - Access event
  SLArMCEvent* ev = 0;
  mc_tree->SetBranchAddress("MCEvent", &ev);

  TH1F * h1_nhit = new TH1F ("h1_nhit", "Hits; Electron number; Counts",  1000, 1e5, 1e6);
  TH2F * h2_nhit_x= new TH2F ("h2_nhit_x", "Hits vs anod distance", 100, 0, 1000, 100, 1e5, 1e6);
    // Mettere i nomi degli assi in h2


  std::map<int, SLArCfgAnode*>  AnodeSysCfg;
  AnodeSysCfg.insert( std::make_pair(10, (SLArCfgAnode*)mc_file->Get("AnodeCfg50") ) );
  AnodeSysCfg.insert( std::make_pair(11, (SLArCfgAnode*)mc_file->Get("AnodeCfg51") ) );


  for (int iev=0; iev< mc_tree->GetEntries(); iev++){
    mc_tree->GetEntry(iev);

    auto primaries = ev->GetPrimaries(); //vector 
    auto andMap = ev->GetEventAnode() ; // mappa con i  2 anodi     

    double electron_hits = 0;
    double electron_dist = 0;

  
    auto anode = andMap.find(11)->second; // valore
    int tpc_id = 11; // chiave
    auto hAnode = AnodeSysCfg[tpc_id]->GetAnodeMap(0); // megatile 
    std::vector<TH2Poly*> h2mt; h2mt.reserve(50);    // vettore di istogrammi di megatile
    std::vector<TH2Poly*> h2pix; h2pix.reserve(500); // vettore di istogrammi di pixel

    // double z_max = 0;

    for (const auto &mt: anode->GetMegaTilesMap()) {  // loop su gruppi di tile
      //printf("\tMegatilte %i: %i hits\n", mt.first, mt.second->GetNChargeHits());
      if (mt.second->GetNChargeHits() == 0) continue;
      for (auto &t : mt.second->GetTileMap()) { // loop sulle tile
        //printf("\t\tTilte %i: %g hits\n", t.first, t.second->GetNPixelHits());
        if (t.second->GetPixelHits() == 0 ) continue;
    
        for (const auto &p : t.second->GetPixelEvents()) {
          //printf("\t\t\tPixel %i has %i hits\n", p.first, p.second->GetNhits());
            electron_hits += p.second->GetHits().size();
            /* for (const auto &hit : electron_hits) {
                charged_time->Fill(hit->GetTime()*0.001);
            } */
          // if (p.second->GetNhits() > z_max) z_max = p.second->GetNhits(); 
        }
      }
    }


    h1_nhit->Fill(electron_hits);
    h2_nhit_x->Fill(primaries.at(0)->GetVertex().at(0), electron_hits); // Da riempire con distanza x
        
  }
  
  
  TCanvas * CNhit = new TCanvas ("CNhit", "Distribution of electron number", 0, 0, 1000, 600);
  CNhit->Divide(2, 1);
  CNhit->cd(1);
  h1_nhit->Draw();
  CNhit->cd(2);
  h2_nhit_x->Draw("col");

}
