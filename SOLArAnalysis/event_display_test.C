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

#include "event/SLArMCEvent.hh"
#include "config/SLArCfgAnode.hh"
#include "config/SLArCfgReadoutTile.hh"
#include "config/SLArCfgAssembly.hh"
#include "config/SLArCfgSuperCellArray.hh"

#include "SLArQEventReadout.hh"
#include "SLArQCluster.hh"

THnSparseF* BuildXYZHist(SLArCfgAnode* cfgAnode, 
                         std::array<int, 2> tile_index,
                         const double drift_len);
THnSparseF* Join(const std::vector<THnSparseF*>& hits_tile);

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
    printf("\tEuler angles: [φ = %g, θ = %g, ψ = %g]\n", 
        cfgAnode->GetPhi()*TMath::RadToDeg(), 
        cfgAnode->GetTheta()*TMath::RadToDeg(), 
        cfgAnode->GetPsi()*TMath::RadToDeg());
  }
  printf("\n");
  
  //- - - - - - - - - - - - - - - - - - - - - - Construct 3D hit structure
  const double pixel_pitch = 4.0; // pixel pith in mm
  const double larpix_integration_time = 600.0; // lartpix integration time (in ns)
  const double v_drift = 1.60e-3; 

  std::vector<THnSparseF*> h_hits_3d; 

  //- - - - - - - - - - - - - - - - - - - - - - Access event
  SLArMCEvent* ev = 0; 
  mc_tree->SetBranchAddress("MCEvent", &ev); 
  mc_tree->GetEntry(iev);
    
  TH1F* charged_time = new TH1F ("charged_time", "Time of arrival; Time [#mu s]; Counts", 1000, 0, 1000);
    
  auto primaries = ev->GetPrimaries(); //vector 

  auto andMap = ev->GetEventAnode() ; // mappa con i  2 anodi     

  slarq::SLArQEventReadout* qev = new slarq::SLArQEventReadout(iev, "qev", "charge pixel event");

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
      auto xyz_hits = BuildXYZHist(AnodeSysCfg[tpc_id], {mt.first, t.first}, 1000); 
      for (const auto &p : t.second->GetPixelEvents()) {
        auto electron_hits = p.second->GetHits();
        TVector3 pix_coord = AnodeSysCfg[tpc_id]->GetPixelCoordinates(
            {mt.first, t.first, p.first});
        //printf("pix_coord: [%g, %g, %g]\n", 
        //pix_coord.x(), pix_coord.y(), pix_coord.z());
        pix_coord -= pix_coord.Dot(drift_direction)*drift_direction;
        for (const auto &hit : electron_hits) {
          charged_time->Fill(hit->GetTime()*0.001);
          TVector3 x_drift = (1000 - (hit->GetTime() * v_drift)) * drift_direction; 
          TVector3 x3d = x_drift + pix_coord; 
          double xyz_[3]; x3d.GetXYZ( xyz_ );
          auto ibin = xyz_hits->Fill( xyz_ ); 
        }
        if (p.second->GetNhits() > z_max) z_max = p.second->GetNhits(); // carica massima per colori
      }
      h_hits_3d.push_back( xyz_hits ); 
      //getchar(); 
    }
  }

  qev->SourceHits3DHist( Join(h_hits_3d) ); 

  std::vector<TString> projectionsList = {"y:x", "y:z"};


  for (const auto projection : projectionsList) {

    auto strArray = projection.Tokenize(":"); 
    std::vector<TVector3> axesList;
    std::vector<int> axesIndexes; 

    for (const auto &obj : *strArray) {
      TObjString* str = (TObjString*)obj;
      TString strAxis = str->GetString(); 
      std::cout << strAxis.Data() << std::endl;
      
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

    TH2D* h2 = qev->GetQHistN()->Projection(axesIndexes.at(0), axesIndexes.at(1)); 
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
        TGraph g;
        Color_t col = kBlack; 
        TString name = ""; 

        if (!pdg_particle) {
          col = kBlack; 
          name = Form("g_%i_trk%i", t->GetPDGID(), t->GetTrackID()); 
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
          name = Form("g_%s_trk_%i", 
              pdg_particle->GetName(), t->GetTrackID()); 
        }

        for (const auto &pt : points) {
          if (pt.fCopy == tpc_id) g.AddPoint( 
              //TVector3(pt.fX, pt.fY, pt.fZ).Dot( AnodeSysCfg[tpc_id]->GetAxis0()), 
              //TVector3(pt.fX, pt.fY, pt.fZ).Dot( AnodeSysCfg[tpc_id]->GetAxis1()) );
            TVector3(pt.fX, pt.fY, pt.fZ).Dot( axesList.at(1) ), 
              TVector3(pt.fX, pt.fY, pt.fZ).Dot( axesList.at(0) ) );
        }
        g.SetName(name); 
        g.SetLineColor(col); 
        g.SetLineWidth(2);
        if (g.GetN() > 2) g.DrawClone("l"); 
      }
    }
  }
  return;
}



THnSparseF* Join(const std::vector<THnSparseF*>& sparseVector) {
  // Get the number of dimensions from the first THnSparseF in the vector
  const Int_t nDim = sparseVector[0]->GetNdimensions();

  // Create a vector to hold the features of the merged axes
  std::vector<TAxis*> mergedAxes;

  // Loop over each dimension
  for (Int_t iDim = 0; iDim < nDim; ++iDim) {
    // Create a temporary vector to hold the bins for this dimension
    std::vector<Double_t> binEdges;

    // Loop over each THnSparseF in the vector
    for (const auto& sparse : sparseVector) {
      // Get the axis for this dimension
      TAxis* axis = sparse->GetAxis(iDim);
      // Add the bin edges for this axis to the temporary vector
      for (Int_t iBin = 0; iBin <= axis->GetNbins(); ++iBin) {
        binEdges.push_back(axis->GetBinLowEdge(iBin));
      }
    }

    // Sort the bin edges in ascending order
    std::sort(binEdges.begin(), binEdges.end());

    // Remove duplicate bin edges
    binEdges.erase(std::unique(binEdges.begin(), binEdges.end()), binEdges.end());

    // Create a new axis for this dimension in the merged histogram
    Int_t nBins = binEdges.size() - 1;
    Double_t* binArray = binEdges.data();
    TAxis* mergedAxis = new TAxis(nBins, binArray);
    mergedAxes.push_back(mergedAxis);
  }
  // Create a new THnSparseF object for merging
  int nbin_temp[nDim];
  double xmin_temp[nDim]; double xmax_temp[nDim]; 
  for (int idim=0; idim<nDim; idim++) {
    xmin_temp[idim] = mergedAxes.at(idim)->GetBinLowEdge(1); 
    xmax_temp[idim] = mergedAxes.at(idim)->GetXmax();
    nbin_temp[idim] = mergedAxes.at(idim)->GetNbins();
  }
  THnSparseF* mergedSparse = new THnSparseF("mergedSparse", "", nDim, 
      nbin_temp, xmin_temp, xmax_temp);

  //for (int idim=0; idim<nDim; idim++) {
    //mergedSparse->GetAxis(idim)->Set(mergedAxes.at(idim)->GetNbins(), 
        //mergedAxes.at(idim)->GetXbins()->GetArray()); 
  //}
  

  //printf("merged sparse:\n");
  //for (int idim=0; idim<nDim; idim++) {
    //printf("\t axis[%i]: %i | %g - %g - δ[0] = %g, δ[20] = %g\n", 
        //idim, mergedSparse->GetAxis(idim)->GetNbins(),
        //mergedSparse->GetAxis(idim)->GetXmin(), mergedSparse->GetAxis(0)->GetXmax(),
        //mergedSparse->GetAxis(idim)->GetBinCenter(2) - mergedSparse->GetAxis(idim)->GetBinCenter(1), 
        //mergedSparse->GetAxis(idim)->GetBinCenter(20) - mergedSparse->GetAxis(idim)->GetBinCenter(19)); 
  //}

  //// Loop over each THnSparseF in the vector
  for (const auto& sparse : sparseVector) {
    // Loop over each bin in the sparse histogram
    auto itr = sparse->CreateIter(true); 
    Long64_t ibin = 0; 
    while ( (ibin = itr->Next()) >= 0) {
      // Get the bin content and coordinates
      Double_t binContent = sparse->GetBinContent(ibin); 
      //if (binContent < 100) continue;
      int binIdxs[nDim];
      double binCoord[nDim];
      
      for (int idim = 0; idim < nDim; idim++) {
        binIdxs[idim] = itr->GetCoord(idim); 
        binCoord[idim] = sparse->GetAxis(idim)->GetBinCenter(binIdxs[idim]);
      }

      // Fill the corresponding bin in the merged sparse histogram
      //printf("bin coord [%g, %g, %g] -> %g\n", 
          //binCoord[0], binCoord[1], binCoord[2], binContent);
      mergedSparse->Fill(binCoord, binContent);
      //getchar(); 
    }
  }

  // Return the merged THnSparseF
  return mergedSparse;
}

THnSparseF* BuildXYZHist(SLArCfgAnode* cfgAnode,
                         std::array<int, 2> tile_index,
                         const double drift_len) {

  double hmin[3] = { 1e8}; 
  double hmax[3] = {-1e8}; 
  
  double xmin=1e6, xmax=-1e6, ymin=1e6, ymax=-1e6, zmin=1e6, zmax =-1e6; 

  auto cfgMegaTile = cfgAnode->GetBaseElement(tile_index.at(0));
  auto cfgTile = cfgMegaTile->GetBaseElement(tile_index.at(1)); 

  auto tile_bin = cfgTile->GetBinIdx(); 

  //printf("MT: %i, Tile %i  - binID %i\n", 
      //cfgMegaTile->GetIdx(), cfgTile->GetIdx(), tile_bin);

  for (const auto &bin_obj : *cfgAnode->GetAnodeMap(1)->GetBins()) {
    TH2PolyBin* bin = (TH2PolyBin*)bin_obj;
    if (bin->GetBinNumber() != tile_bin) continue;

    TGraph* g = (TGraph*)bin->GetPolygon(); 
    ymin = std::min( ymin, *std::min_element(g->GetY(), g->GetY()+4)); 
    ymax = std::max( ymax, *std::max_element(g->GetY(), g->GetY()+4)); 

    zmin = std::min( zmin, *std::min_element(g->GetX(), g->GetX()+4)); 
    zmax = std::max( zmax, *std::max_element(g->GetX(), g->GetX()+4)); 
  }

  ymin += cfgTile->GetPhysY(); ymax += cfgTile->GetPhysY(); 
  zmin += cfgTile->GetPhysZ(); zmax += cfgTile->GetPhysZ(); 

  const double pixel_pitch = 4.0; 
  const double v_drift = 1.589e-3; // in [mm/ns]
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

  TString hname = Form("hxyz_mt%i_t%i", cfgMegaTile->GetIdx(), cfgTile->GetIdx());
  THnSparseF* h_xyz = new THnSparseF(hname, hname, 3, nbin, hmin, hmax); 

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
