/**
 * @author      Giulia Conti (unimib), Daniele Guffanti (unimin & infn-mib)
 * @file        event_processing_test.C
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

const double pixel_pitch = 4.0;               // pixel pith in mm
const double larpix_integration_time = 600.0; // lartpix integration time (in ns)
const double v_drift = 1.582e-3;

const double noise_rms = 900;

struct solar_cluster_track
{
  int fEventNumber = 0;
  int fNClusters = 0;
  float fTotalCharge = 0;
  float fMaxClusterCharge = 0;
  float fTrueEventDir[3] = {0};
  float fRecoEventDir[3] = {0};
  float fCosTheta = 0;

  inline void reset()
  {
    fEventNumber = 0;
    fNClusters = 0;
    fTotalCharge = 0.;
    fMaxClusterCharge = 0.;
    fCosTheta = 0;
    for (int j = 0; j < 3; j++)
    {
      fTrueEventDir[j] = 0;
      fRecoEventDir[j] = 0;
    }
  }
};

std::vector<Color_t> color_vector = {kOrange + 7, kRed, kMagenta + 1, kViolet + 6, kBlue, kAzure + 1, kCyan - 3, kGreen + 1, kGreen + 3, kYellow - 6};

THnSparseF *BuildXYZHist(SLArCfgAnode *cfgAnode,
                         const double drift_len);

int process_event(SLArMCEvent *ev, SLArQEventReadout *qev, THnSparseF *xyz_hits, std::map<int, SLArCfgAnode *> &AnodeSysCfg, bool do_draw = false, solar_cluster_track *track_reco = nullptr);

void process_file(const TString file_path, bool single_shot = false)
{
  gStyle->SetPalette(kBlackBody);
  TColor::InvertPalette();

  //- - - - - - - - - - - - - - - - - - - - - - Open data file
  TFile *mc_file = new TFile(file_path);
  TTree *mc_tree = (TTree *)mc_file->Get("EventTree");

  //- - - - - - - - - - - - - - - - - - - - - - Access readout configuration
  std::map<int, SLArCfgAnode *> AnodeSysCfg;
  AnodeSysCfg.insert(std::make_pair(10, (SLArCfgAnode *)mc_file->Get("AnodeCfg50")));
  AnodeSysCfg.insert(std::make_pair(11, (SLArCfgAnode *)mc_file->Get("AnodeCfg51")));

  for (const auto &anodeCfg_ : AnodeSysCfg)
  {
    const auto cfgAnode = anodeCfg_.second;
    printf("Anode config: %i - %lu mega-tiles\n", cfgAnode->GetIdx(),
           cfgAnode->GetMap().size());
    printf("\tposition: [%g, %g, %g] mm\n",
           cfgAnode->GetPhysX(), cfgAnode->GetPhysY(), cfgAnode->GetPhysZ());
    printf("\tnormal: [%g, %g, %g]\n",
           cfgAnode->GetNormal().x(), cfgAnode->GetNormal().y(), cfgAnode->GetNormal().z());
    printf("\tEuler angles: [φ = %g, θ = %g, ψ = %g]\n",
           cfgAnode->GetPhi() * TMath::RadToDeg(),
           cfgAnode->GetTheta() * TMath::RadToDeg(),
           cfgAnode->GetPsi() * TMath::RadToDeg());
  }
  printf("\n");

  TString input = "";
  int iev = 0;
  const int tpc_id = 11;

  //- - - - - - - - - - - - - - - - - - - - - - Create 3D histogram for hits
  auto xyz_hits = BuildXYZHist(AnodeSysCfg[tpc_id], 1000);

  //- - - - - - - - - - - - - - - - - - - - - - Create SoLAr event structures
  SLArMCEvent *ev = 0;
  SLArQEventReadout *qev = new SLArQEventReadout();
  solar_cluster_track *track = new solar_cluster_track();

  mc_tree->SetBranchAddress("MCEvent", &ev);

  bool do_exit = false;

  if (single_shot)
  { // process just one event
    std::cout << "Enter the event number or enter 'quit' to exit: ";
    std::cin >> input;

    iev = input.Atoi();

    printf("Reading event %i\n", iev);
    mc_tree->GetEntry(iev);

    int status = process_event(ev, qev, xyz_hits, AnodeSysCfg, true);

    printf("event processed with status %i\n", status);

    return;
  }

  // if not "single-shot", create a file and a tree
  // where to store the result of the analysis
  TString output_file_name = file_path;
  output_file_name.Resize(output_file_name.Index(".root"));
  output_file_name += Form("_noise_%d_processed.root", static_cast<int>(noise_rms));
  TFile *file_output = new TFile(output_file_name, "recreate");

  TTree *output_tree = new TTree("processed_events", "SoLAr processed events");
  output_tree->Branch("ev_number", &track->fEventNumber);
  output_tree->Branch("tot_charge", &track->fTotalCharge);
  output_tree->Branch("n_clusters", &track->fNClusters);
  output_tree->Branch("cluster_charge", &track->fMaxClusterCharge);
  output_tree->Branch("true_dir", &track->fTrueEventDir, "nx/F:ny:nz");
  output_tree->Branch("reco_dir", &track->fRecoEventDir, "nx/F:ny:nz");
  output_tree->Branch("cos_theta", &track->fCosTheta);

  for (int iev = 0; iev < 200 /*iev<mc_tree->GetEntries()*/; iev++)
  //for (int iev = 0; iev < 100 /*iev<mc_tree->GetEntries()*/; iev++)
  {
    ev->Reset();
    qev->ResetEvent();
    xyz_hits->Reset();
    mc_tree->GetEntry(iev);

    int status = process_event(ev, qev, xyz_hits, AnodeSysCfg, false, track);

    // getchar();

    if (status == 0)
    {
      output_tree->Fill();
    }
  }

  output_tree->Write();
  // getchar();
  // timer->TurnOff();

  //}

  return;
}

int process_event(SLArMCEvent *ev, SLArQEventReadout *qev, THnSparseF *xyz_hits, std::map<int, SLArCfgAnode *> &AnodeSysCfg, bool do_draw, solar_cluster_track *track_reco)
{
  const auto primaries = ev->GetPrimaries(); // vector of primary particles
  const TVector3 true_dir(ev->GetDirection().data());
  std::vector<double> primary_vtx;
  // Get primary e- vertex
  for (const auto &primary : primaries)
  {
    if (primary->GetParticleName() == "e-")
    {
      primary_vtx = primary->GetVertex();
      break;
    }
  }
  TVector3 vtx(&primary_vtx.at(0));

  if (ev->GetEvNumber() % 1 == 0)
  {
    printf("\n---------------------------------------------------------\n");
    printf("processing event %i\n", ev->GetEvNumber());
    printf("vertex: %g, %g, %g\n", vtx.x(), vtx.y(), vtx.z());
  }

  if (fabs(vtx.y()) > 600 || fabs(vtx.z()) > 1000 || vtx.x() > 950)
  {
    if (do_draw)
      printf("Vertex out of scope\n");
    return 2;
  }

  const auto andMap = ev->GetEventAnode(); // mappa con i  2 anodi

  qev->SetEventNr(ev->GetEvNumber());
  if (track_reco)
    track_reco->reset();

  TVector3 drift_direction(1, 0, 0);

  // Get data only from the TPC we are interested in
  int tpc_id = 11;                               // chiave
  auto anode = ev->GetEventAnodeByTPCID(tpc_id); // valore

  // double z_max = 0;

  for (const auto &mt : anode->GetMegaTilesMap())
  { // loop su gruppi di tile
    // printf("\tMegatilte %i: %i hits\n", mt.first, mt.second->GetNChargeHits());
    if (mt.second->GetNChargeHits() == 0)
      continue;
    for (auto &t : mt.second->GetTileMap())
    { // loop sulle tile
      // printf("\t\tTilte %i: %g hits\n", t.first, t.second->GetNPixelHits());
      if (t.second->GetPixelHits() == 0)
        continue;
      for (const auto &p : t.second->GetPixelEvents())
      {
        auto electron_hits = p.second->GetHits();
        TVector3 pix_coord = AnodeSysCfg[tpc_id]->GetPixelCoordinates(
            {mt.first, t.first, p.first});
        // printf("pix_coord: [%g, %g, %g]\n",
        // pix_coord.x(), pix_coord.y(), pix_coord.z());
        pix_coord -= pix_coord.Dot(drift_direction) * drift_direction;
        for (const auto &hit : electron_hits)
        {
          TVector3 x_drift = (AnodeSysCfg[tpc_id]->GetPhysX() -
                              (hit->GetTime() * v_drift)) *
                             drift_direction;
          TVector3 x3d = x_drift + pix_coord;
          double xyz_[3];
          x3d.GetXYZ(xyz_);
          auto ibin = xyz_hits->Fill(xyz_);
        }
        // if (p.second->GetNhits() > z_max)
        // z_max = p.second->GetNhits(); // carica massima per colori
      }
      // getchar();
    }
  }

  if (noise_rms > 0)
  {
    auto itr = xyz_hits->CreateIter(false);
    Long64_t ibin = 0;

    while ((ibin = itr->Next()) >= 0)
    {
      float content = xyz_hits->GetBinContent(ibin);
      content += gRandom->Gaus(0, noise_rms);
      xyz_hits->SetBinContent(ibin, content);
    }
  }

  qev->SourceHits3DHist(xyz_hits);

  qev->Clustering();

  if (qev->GetClusters().empty())
  {
    if (do_draw)
      printf("No clusters found\n");
    return 3;
  }

  auto max_cl_points = qev->GetMaxCluster()->get_points();
  double d = 0;
  bool same_cluster = false;

  for (const auto cl_point : max_cl_points)
  {
    d = (cl_point.fPos - vtx).Mag();
    if (d < 8)
    {
      same_cluster = true;
      break;
    }
  }

  if (same_cluster == false)
  {
    if (do_draw)
      printf("Max cluster is not the same of the vertex\n");
    return 4; // Chiedere se è ok
  }

  SLArQEventAnalysis q_ev_analysis;

  std::vector<TString> projectionsList = {"y:x", "y:z", "z:x", "x:y", "z:y", "x:z"};
  std::vector<slarq::cluster_projection_info_t> projectionInfo;
  std::vector<slarq::cluster_projection_info_t> projectionGold; 
  std::vector<TCanvas *> canvas;

  int iproj = 0;
  for (const auto projection : projectionsList)
  {
    auto proj_info = q_ev_analysis.ProcessProjection(projection, qev, vtx);
    projectionInfo.push_back(proj_info);

    if (do_draw)
    {
      TString canv_name = Form("c_%s", projection.Data());
      TCanvas *cProjection = new TCanvas(canv_name, canv_name, 10 + 20 * iproj, 10 + 20 * iproj, 800, 600);
      q_ev_analysis.DrawProjection(qev, vtx, proj_info);

      cProjection->Modified();
      cProjection->Update();

      canvas.push_back(cProjection);
    }

    iproj++;
  }

  // - - - - - - - - - - - - - - Find the two best projections - - - - - - - - - - - -
  // NB: the best projections are the ones with higher non void bin number on the orizontal axis

  std::sort(projectionInfo.begin(), projectionInfo.end(),
            [](const slarq::cluster_projection_info_t lhs,
               const slarq::cluster_projection_info_t rhs)
            {
              double metric_lhs = 0;
              double metric_rhs = 0;
              if (lhs.fDirX == +1)
                metric_lhs = lhs.fLengthX[1];
              else if (lhs.fDirX == -1)
                metric_lhs = lhs.fLengthX[0];

              if (rhs.fDirX == +1)
                metric_rhs = rhs.fLengthX[1];
              else if (rhs.fDirX == -1)
                metric_rhs = rhs.fLengthX[0];

              return metric_lhs > metric_rhs;
            });

  iproj = 0;
  bool axis_set[3] = {false};
  TVector3 reco_dir;

  while (!(axis_set[0] && axis_set[1] && axis_set[2]) && iproj < projectionInfo.size())
  {
    const auto proj_info = projectionInfo.at(iproj);

    if (axis_set[proj_info.fAxisIdx[0]] == false ||
        axis_set[proj_info.fAxisIdx[1]] == false)
    {

      /*
       *for (int i=0; i<2; i++) {
       *  float q_proj = 0;
       *  (proj_info.fDirX == -1) ?
       *    q_proj = proj_info.fChargeX[0] : q_proj = proj_info.fChargeX[1];
       *  printf("Projection: axis[%i] = %i -> (%g, %g, %g) [q: %g]\n",
       *      i,
       *      proj_info.fAxisIdx[i],
       *      proj_info.fAxis[i].x(), proj_info.fAxis[i].y(), proj_info.fAxis[i].z(),
       *      q_proj);
       *}
       */

      double coord[3] = {0};

      coord[proj_info.fAxisIdx[1]] = 1.0;
      coord[proj_info.fAxisIdx[0]] = 1.0 * proj_info.fPar1;
      if (proj_info.fDirX == -1)
      {
        coord[proj_info.fAxisIdx[1]] *= -1;
        coord[proj_info.fAxisIdx[0]] *= -1;
      }

      if (axis_set[proj_info.fAxisIdx[0]] == false &&
          axis_set[proj_info.fAxisIdx[1]] == false)
      {
        reco_dir[proj_info.fAxisIdx[0]] = coord[proj_info.fAxisIdx[0]];
        reco_dir[proj_info.fAxisIdx[1]] = coord[proj_info.fAxisIdx[1]];

        axis_set[proj_info.fAxisIdx[0]] = true;
        axis_set[proj_info.fAxisIdx[1]] = true;

        projectionGold.push_back( proj_info ); 
      }
      else
      {
        float rescaling = 1.0;
        if (axis_set[proj_info.fAxisIdx[0]])
        {
          const auto proj_first = projectionGold.front(); 
          if (proj_info.fAxisIdx[0] == proj_first.fAxisIdx[0]) 
          {
            printf("second set: case 0\n");
            float tmp = reco_dir[ proj_first.fAxisIdx[0] ] / coord[proj_info.fAxisIdx[0]];

            reco_dir[proj_info.fAxisIdx[1]] = fabs(tmp) * proj_info.fDirX;
          }
          else if (proj_info.fAxisIdx[0] == proj_first.fAxisIdx[1])
          {
            printf("second set: case 1\n");
            float tmp = 1.0 / coord[proj_info.fAxisIdx[0]];

            reco_dir[proj_info.fAxisIdx[1]] = fabs(tmp) * proj_info.fDirX;
          }
          axis_set[proj_info.fAxisIdx[1]] = true;

          projectionGold.push_back( proj_info ); 
        }
        else if (axis_set[proj_info.fAxisIdx[1]])
        {
          const auto proj_first = projectionGold.front(); 
          if (proj_info.fAxisIdx[1] == proj_first.fAxisIdx[1]) {
            printf("second set: case 2\n");
            rescaling = 1.0;

            reco_dir[proj_info.fAxisIdx[0]] = coord[proj_info.fAxisIdx[0]];
          } 
          else if (proj_info.fAxisIdx[1] == proj_first.fAxisIdx[0]) 
          { 
            printf("second set: case 3\n");
            reco_dir[proj_info.fAxisIdx[0]] = reco_dir[proj_first.fAxisIdx[0]] * coord[proj_info.fAxisIdx[0]];
          }
          axis_set[proj_info.fAxisIdx[0]] = true;

          projectionGold.push_back( proj_info ); 
        }

        // printf("rescaling factor: %g\n", rescaling);
      }

      // printf("axis set: [%i, %i, %i]\n", axis_set[0], axis_set[1], axis_set[2]);

      // for (int j=0; j<2; j++) {
      // int iaxis = proj_info.fAxisIdx[j];
      // if (axis_set[iaxis] == false) {
      // reco_dir[iaxis] = coord[iaxis] * rescaling;
      // axis_set[iaxis] = true;
      // printf("setting coordinate [%i] to %g\n", iaxis, reco_dir[iaxis]);
      //}
      //}

      if (do_draw)
      {
        printf("projection %s : %s (%i : %i)\n", 
            proj_info.fAxisLabel[0].Data(), proj_info.fAxisLabel[1].Data(),
            proj_info.fAxisIdx[0], proj_info.fAxisIdx[1]);
        printf("\t- dirX: %i - dirY: %i\n", proj_info.fDirX, proj_info.fDirY);
        printf("\t- chgX0: %g - chgX1: %g\n", proj_info.fChargeX[0], proj_info.fChargeX[1]);
        printf("\t- lenX0: %g - lenX1: %g\n", proj_info.fLengthX[0], proj_info.fLengthX[1]);
        printf("\t- hitsX0: %g - hitsX1: %g\n", proj_info.fNHitsX[0], proj_info.fNHitsX[1]);
        printf("\t- fit: %.2f + (%g)*x\n", proj_info.fPar0, proj_info.fPar1);
        printf("\t- coord: (%g, %g, %g)\n", coord[0], coord[1], coord[2]);
        printf("\t- reco_dir: %g, %g, %g\n", 
            reco_dir.x(), reco_dir.y(), reco_dir.z());
      }
    }

    iproj++;
  }

  reco_dir = reco_dir.Unit();
  printf("reco_dir: %g, %g, %g\n",
         reco_dir.x(), reco_dir.y(), reco_dir.z());
  float cos_theta = true_dir.Dot(reco_dir);
  printf("cos θ = %g\n", true_dir.Dot(reco_dir));

  float theta = 1000;

  if (cos_theta < -1 || cos_theta > 1) {
        printf("cos_theta is outside the interval [-1,1]\n");
  }
  else
  {
    float theta_rad = std::acos(cos_theta);
    theta = theta_rad * 180 / M_PI;
  }

  if (track_reco)
  {
    track_reco->fEventNumber = ev->GetEvNumber();
    track_reco->fNClusters = qev->GetClusters().size();
    track_reco->fTotalCharge = qev->GetTotalCharge();
    track_reco->fMaxClusterCharge = qev->GetMaxCluster()->get_charge();
    reco_dir.GetXYZ( track_reco->fRecoEventDir ); 
    true_dir.GetXYZ( track_reco->fTrueEventDir ); 
    track_reco->fCosTheta = cos_theta;
  }

  if (do_draw == false)
  {
    for (auto &c : canvas)
    {
      c->Close();
    }
  }

  // getchar();

  return 0;
}

THnSparseF *BuildXYZHist(SLArCfgAnode *cfgAnode,
                         const double drift_len)
{

  double hmin[3] = {1e8};
  double hmax[3] = {-1e8};

  double xmin = 1e6, xmax = -1e6, ymin = 1e6, ymax = -1e6, zmin = 1e6, zmax = -1e6;

  for (const auto &bin_obj : *cfgAnode->GetAnodeMap(0)->GetBins())
  {
    TH2PolyBin *bin = (TH2PolyBin *)bin_obj;

    TGraph *g = (TGraph *)bin->GetPolygon();
    ymin = std::min(ymin, *std::min_element(g->GetY(), g->GetY() + 4));
    ymax = std::max(ymax, *std::max_element(g->GetY(), g->GetY() + 4));

    zmin = std::min(zmin, *std::min_element(g->GetX(), g->GetX() + 4));
    zmax = std::max(zmax, *std::max_element(g->GetX(), g->GetX() + 4));
  }

  const double pixel_pitch = 4.0;
  const double v_drift = 1.582e-3; // in [mm/ns]
  const double larpix_integration_time = 600.0;
  const double hit_drift_window = larpix_integration_time * v_drift;
  int n_bin_drift = (int)(drift_len / hit_drift_window);
  xmin = 0;
  xmax = hit_drift_window * n_bin_drift;
  hmin[0] = xmin;
  hmax[0] = xmax;
  hmin[1] = ymin;
  hmax[1] = ymax;
  hmin[2] = zmin;
  hmax[2] = zmax;

  int nbin[3] = {0};
  for (int i = 0; i < 3; i++)
  {
    if (i > 0)
    {
      nbin[i] = (hmax[i] - hmin[i]) / pixel_pitch;
    }
    else
    {
      nbin[i] = n_bin_drift;
    }
  }

  // printf("Creating THnSparse in range %i[%g, %g] - %i[%g, %g] - %i[%g, %g]\n",
  // nbin[0], hmin[0], hmax[0], nbin[1], hmin[1], hmax[1], nbin[2], hmin[2], hmax[2]);

  TString hname = "hxyz";
  THnSparseF *h_xyz = new THnSparseF(hname, hname, 3, nbin, hmin, hmax);

  h_xyz->GetAxis(0)->SetTitle("x [mm]");
  h_xyz->GetAxis(1)->SetTitle("y [mm]");
  h_xyz->GetAxis(2)->SetTitle("z [mm]");

  return h_xyz;
}

void hit_vs_distance(const TString file_path)
{

  TFile *mc_file = new TFile(file_path);
  TTree *mc_tree = (TTree *)mc_file->Get("EventTree");

  //- - - - - - - - - - - - - - - - - - - - - - Access event
  SLArMCEvent *ev = 0;
  mc_tree->SetBranchAddress("MCEvent", &ev);

  TH1F *h1_nhit = new TH1F("h1_nhit", "Hits; Electron number; Counts", 1000, 1e5, 1e6);
  TH2F *h2_nhit_x = new TH2F("h2_nhit_x", "Hits vs anod distance", 100, 0, 1000, 100, 1e5, 1e6);
  // Mettere i nomi degli assi in h2

  std::map<int, SLArCfgAnode *> AnodeSysCfg;
  AnodeSysCfg.insert(std::make_pair(10, (SLArCfgAnode *)mc_file->Get("AnodeCfg50")));
  AnodeSysCfg.insert(std::make_pair(11, (SLArCfgAnode *)mc_file->Get("AnodeCfg51")));

  for (int iev = 0; iev < 200; /*mc_tree->GetEntries();*/ iev++)
  {
    mc_tree->GetEntry(iev);

    auto primaries = ev->GetPrimaries(); // vector
    auto andMap = ev->GetEventAnode();   // mappa con i  2 anodi

    double electron_hits = 0;
    double electron_dist = 0;

    auto anode = andMap.find(11)->second;              // valore
    int tpc_id = 11;                                   // chiave
    auto hAnode = AnodeSysCfg[tpc_id]->GetAnodeMap(0); // megatile
    std::vector<TH2Poly *> h2mt;
    h2mt.reserve(50); // vettore di istogrammi di megatile
    std::vector<TH2Poly *> h2pix;
    h2pix.reserve(500); // vettore di istogrammi di pixel

    // double z_max = 0;

    for (const auto &mt : anode->GetMegaTilesMap())
    { // loop su gruppi di tile
      // printf("\tMegatilte %i: %i hits\n", mt.first, mt.second->GetNChargeHits());
      if (mt.second->GetNChargeHits() == 0)
        continue;
      for (auto &t : mt.second->GetTileMap())
      { // loop sulle tile
        // printf("\t\tTilte %i: %g hits\n", t.first, t.second->GetNPixelHits());
        if (t.second->GetPixelHits() == 0)
          continue;

        for (const auto &p : t.second->GetPixelEvents())
        {
          // printf("\t\t\tPixel %i has %i hits\n", p.first, p.second->GetNhits());
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

  TCanvas *CNhit = new TCanvas("CNhit", "Distribution of electron number", 0, 0, 1000, 600);
  CNhit->Divide(2, 1);
  CNhit->cd(1);
  h1_nhit->Draw();
  CNhit->cd(2);
  h2_nhit_x->Draw("col");
}
