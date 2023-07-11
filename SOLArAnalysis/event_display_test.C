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


#include "event/SLArMCEvent.hh"
#include "config/SLArCfgAnode.hh"
#include "config/SLArCfgReadoutTile.hh"
#include "config/SLArCfgAssembly.hh"
#include "config/SLArCfgSuperCellArray.hh"

#include "SLArQEventReadout.hh"
#include "SLArQCluster.hh"

using namespace slarq;

const double pixel_pitch = 4.0; // pixel pith in mm
const double larpix_integration_time = 600.0; // lartpix integration time (in ns)
const double v_drift = 1.582e-3; 


THnSparseF* BuildXYZHist(SLArCfgAnode* cfgAnode, 
                         const double drift_len);
void read_and_display_event(SLArMCEvent* ev, SLArQEventReadout* qev, THnSparseF* xyz_hits, std::map<int,SLArCfgAnode*>& AnodeSysCfg);

void event_display_test(const TString file_path) 
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
    printf("\tEuler angles: [φ = %g, θ = %g, ψ = %g]\n", 
        cfgAnode->GetPhi()*TMath::RadToDeg(), 
        cfgAnode->GetTheta()*TMath::RadToDeg(), 
        cfgAnode->GetPsi()*TMath::RadToDeg());
  }
  printf("\n");
  


  TString input = ""; 
  int iev = 0;
  const int tpc_id = 11;

  //- - - - - - - - - - - - - - - - - - - - - - Access event
  auto xyz_hits = BuildXYZHist(AnodeSysCfg[tpc_id], 1000);

  SLArMCEvent* ev = 0; 
  SLArQEventReadout* qev = new SLArQEventReadout();

  mc_tree->SetBranchAddress("MCEvent", &ev);

  TTimer* timer = new TTimer("gSystem->ProcessEvents();", 500, false); 

  bool do_exit = false; 

  //while (do_exit == false) {
    std::cout << "Enter the event number or enter 'quit' to exit: ";
    std::cin >> input ; 

    //if (input == "quit") {do_exit = true; continue;}

    iev = input.Atoi();


    printf("Reading event %i\n", iev);
    mc_tree->GetEntry(iev);


    //timer->TurnOn(); 
    //timer->Reset(); 

    read_and_display_event(ev, qev, xyz_hits, AnodeSysCfg) ;

    //getchar(); 
    //timer->TurnOff(); 

  //}

  return;
}




void read_and_display_event(SLArMCEvent* ev, SLArQEventReadout* qev, THnSparseF* xyz_hits, std::map<int,SLArCfgAnode*> & AnodeSysCfg) {

  auto primaries = ev->GetPrimaries(); //vector 

  auto andMap = ev->GetEventAnode() ; // mappa con i  2 anodi     

  qev->ResetEvent();
  qev->SetEventNr(ev->GetEvNumber());

  TVector3 drift_direction(1, 0, 0); 

  // Get data only from the TPC we are interested in
  int tpc_id = 11; // chiave
  auto anode = ev->GetEventAnodeByTPCID(tpc_id); // valore

 
  double z_max = 0; 

  for (const auto &mt: anode->GetMegaTilesMap()) {  // loop su gruppi di tile
    //printf("\tMegatilte %i: %i hits\n", mt.first, mt.second->GetNChargeHits());
    if (mt.second->GetNChargeHits() == 0) continue;
    for (auto &t : mt.second->GetTileMap()) { // loop sulle tile
      //printf("\t\tTilte %i: %g hits\n", t.first, t.second->GetNPixelHits());
      if (t.second->GetPixelHits() == 0 ) continue;
      for (const auto &p : t.second->GetPixelEvents()) {
        auto electron_hits = p.second->GetHits();
        TVector3 pix_coord = AnodeSysCfg[tpc_id]->GetPixelCoordinates(
            {mt.first, t.first, p.first});
        //printf("pix_coord: [%g, %g, %g]\n", 
        //pix_coord.x(), pix_coord.y(), pix_coord.z());
        pix_coord -= pix_coord.Dot(drift_direction)*drift_direction;
        for (const auto &hit : electron_hits) {
          TVector3 x_drift = (AnodeSysCfg[tpc_id]->GetPhysX() -
              (hit->GetTime() * v_drift)) * drift_direction; 
          TVector3 x3d = x_drift + pix_coord; 
          double xyz_[3]; x3d.GetXYZ( xyz_ );
          auto ibin = xyz_hits->Fill( xyz_ ); 
        }
        if (p.second->GetNhits() > z_max) z_max = p.second->GetNhits(); // carica massima per colori
      }
      //getchar(); 
    }
  }

  qev->SourceHits3DHist( xyz_hits ); 

  std::vector<TString> projectionsList = {"y:x", "y:z", "z:x"};

  for (const auto projection : projectionsList) {

    auto strArray = projection.Tokenize(":"); 
    std::vector<TVector3> axesList;
    std::vector<int> axesIndexes; 

    for (const auto &obj : *strArray) {
      TObjString* str = (TObjString*)obj;
      TString strAxis = str->GetString(); 
      
      if      (strAxis == "x") {
        axesList.push_back( TVector3(1, 0, 0) ); 
        axesIndexes.push_back(0);
      }
      else if (strAxis == "y") {
        axesList.push_back( TVector3(0, 1, 0) ); 
        axesIndexes.push_back(1);
      }
      else if (strAxis == "z") {
        axesList.push_back( TVector3(0, 0, 1) ); 
        axesIndexes.push_back(2); 
      }
    }

    if (strArray->GetEntries() != 2) return;

    TCanvas* cProjection2D = new TCanvas("cProjection2D"+projection, projection, 
        0, 0, 800, 600); 
    cProjection2D->SetTicks(1, 1); 
    cProjection2D->cd(); 

    TH2D* h2 = qev->GetQHistN()->Projection(axesIndexes.at(0), axesIndexes.at(1)); 
    h2->SetName(Form("h2%s_ev%i", projection.Data(), ev->GetEvNumber())); 
    h2->Draw("colz"); 

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
        TGraph* g = new TGraph();
        Color_t col = kBlack; 
        TString name = ""; 

        if (!pdg_particle) {
          col = kBlack; 
          name = Form("g%s_%i_trk%i", projection.Data(), t->GetPDGID(), t->GetTrackID()); 
        }
        else {
          if      (pdg_particle == pdg->GetParticle(  22)) col = kYellow;    // γ
          else if (pdg_particle == pdg->GetParticle(  11)) col = kBlue-6;    // e-
          else if (pdg_particle == pdg->GetParticle( -11)) col = kRed-7;     // e+
          else if (pdg_particle == pdg->GetParticle(2212)) col = kRed;       // p
          else if (pdg_particle == pdg->GetParticle(2112)) col = kBlue;      // n
          else if (pdg_particle == pdg->GetParticle(-211)) col = kOrange+7;  // pi-
          else if (pdg_particle == pdg->GetParticle( 211)) col = kViolet-2;  // pi+
          else if (pdg_particle == pdg->GetParticle( 111)) col = kGreen;     // pi0
          else    col = kGray+2;
          name = Form("g%s_%s_trk_%i", 
              projection.Data(), pdg_particle->GetName(), t->GetTrackID()); 
        }

        for (const auto &pt : points) {
          if (pt.fCopy == tpc_id) g->AddPoint( 
              //TVector3(pt.fX, pt.fY, pt.fZ).Dot( AnodeSysCfg[tpc_id]->GetAxis0()), 
              //TVector3(pt.fX, pt.fY, pt.fZ).Dot( AnodeSysCfg[tpc_id]->GetAxis1()) );
            TVector3(pt.fX, pt.fY, pt.fZ).Dot( axesList.at(1) ), 
              TVector3(pt.fX, pt.fY, pt.fZ).Dot( axesList.at(0) ) );
        }
        g->SetName(name); 
        g->SetLineColor(col); 
        g->SetLineWidth(2);
        if (g->GetN() > 2) g->Draw("l"); 
      }
    }
    cProjection2D->Modified();
    cProjection2D->Update(); 
  }
  return;
}



THnSparseF* BuildXYZHist(SLArCfgAnode* cfgAnode,
                         const double drift_len) {

  double hmin[3] = { 1e8}; 
  double hmax[3] = {-1e8}; 
  
  double xmin=1e6, xmax=-1e6, ymin=1e6, ymax=-1e6, zmin=1e6, zmax =-1e6; 

  for (const auto &bin_obj : *cfgAnode->GetAnodeMap(0)->GetBins()) {
    TH2PolyBin* bin = (TH2PolyBin*)bin_obj;

    TGraph* g = (TGraph*)bin->GetPolygon(); 
    ymin = std::min( ymin, *std::min_element(g->GetY(), g->GetY()+4)); 
    ymax = std::max( ymax, *std::max_element(g->GetY(), g->GetY()+4)); 

    zmin = std::min( zmin, *std::min_element(g->GetX(), g->GetX()+4)); 
    zmax = std::max( zmax, *std::max_element(g->GetX(), g->GetX()+4)); 
  }

  const double pixel_pitch = 4.0; 
  const double v_drift = 1.582e-3; // in [mm/ns]
  const double larpix_integration_time = 600.0; 
  const double hit_drift_window = larpix_integration_time * v_drift; 
  int n_bin_drift= (int)(drift_len /  hit_drift_window); 
  xmin = 0;
  xmax = hit_drift_window * n_bin_drift; 
  hmin[0] = xmin; hmax[0] = xmax; 
  hmin[1] = ymin; hmax[1] = ymax; 
  hmin[2] = zmin; hmax[2] = zmax; 

  int nbin[3] = {0};
  for (int i=0; i<3; i++) {
    if (i>0){
      nbin[i] = (hmax[i]-hmin[i])/pixel_pitch;
    } 
    else {
      nbin[i] = n_bin_drift; 
    }
  }

  //printf("Creating THnSparse in range %i[%g, %g] - %i[%g, %g] - %i[%g, %g]\n", 
      //nbin[0], hmin[0], hmax[0], nbin[1], hmin[1], hmax[1], nbin[2], hmin[2], hmax[2]);

  TString hname = "hxyz";
  THnSparseF* h_xyz = new THnSparseF(hname, hname, 3, nbin, hmin, hmax); 

  h_xyz->GetAxis(0)->SetTitle("x [mm]"); 
  h_xyz->GetAxis(1)->SetTitle("y [mm]"); 
  h_xyz->GetAxis(2)->SetTitle("z [mm]"); 

  return h_xyz;
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
