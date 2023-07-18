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
#include "TMarker.h"

#include "event/SLArMCEvent.hh"
#include "config/SLArCfgAnode.hh"
#include "config/SLArCfgReadoutTile.hh"
#include "config/SLArCfgAssembly.hh"
#include "config/SLArCfgSuperCellArray.hh"

#include "SLArQEventReadout.hh"
#include "SLArQCluster.hh"

using namespace slarq;

const double pixel_pitch = 4.0;               // pixel pith in mm
const double larpix_integration_time = 600.0; // lartpix integration time (in ns)
const double v_drift = 1.582e-3;

std::vector<Color_t> color_vector = {kOrange + 7, kRed, kMagenta + 1, kViolet + 6, kBlue, kAzure + 1, kCyan - 3, kGreen + 1, kGreen + 3, kYellow - 6};

THnSparseF *BuildXYZHist(SLArCfgAnode *cfgAnode,
                         const double drift_len);
void read_and_display_event(SLArMCEvent *ev, SLArQEventReadout *qev, THnSparseF *xyz_hits, std::map<int, SLArCfgAnode *> &AnodeSysCfg);

double line(double *x, double *par)
{
  return par[0] * x[0] + par[1];
}

void event_display_test(const TString file_path)
{
  gStyle->SetPalette(kBlackBody);
  TColor::InvertPalette();
  TFile *mc_file = new TFile(file_path);
  TTree *mc_tree = (TTree *)mc_file->Get("EventTree");

  //- - - - - - - - - - - - - - - - - - - - - - Access readout configuration
  std::map<int, SLArCfgAnode *> AnodeSysCfg;
  AnodeSysCfg.insert(std::make_pair(10, (SLArCfgAnode *)mc_file->Get("AnodeCfg50")));
  AnodeSysCfg.insert(std::make_pair(11, (SLArCfgAnode *)mc_file->Get("AnodeCfg51")));

  std::map<int, TH2Poly *> h2SCArray;

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

  //- - - - - - - - - - - - - - - - - - - - - - Access event
  auto xyz_hits = BuildXYZHist(AnodeSysCfg[tpc_id], 1000);

  SLArMCEvent *ev = 0;
  SLArQEventReadout *qev = new SLArQEventReadout();

  mc_tree->SetBranchAddress("MCEvent", &ev);

  TTimer *timer = new TTimer("gSystem->ProcessEvents();", 500, false);

  bool do_exit = false;

  // while (do_exit == false) {
  std::cout << "Enter the event number or enter 'quit' to exit: ";
  std::cin >> input;

  // if (input == "quit") {do_exit = true; continue;}

  iev = input.Atoi();

  printf("Reading event %i\n", iev);
  mc_tree->GetEntry(iev);

  // timer->TurnOn();
  // timer->Reset();

  read_and_display_event(ev, qev, xyz_hits, AnodeSysCfg);

  // getchar();
  // timer->TurnOff();

  //}

  return;
}

void read_and_display_event(SLArMCEvent *ev, SLArQEventReadout *qev, THnSparseF *xyz_hits, std::map<int, SLArCfgAnode *> &AnodeSysCfg)
{

  auto primaries = ev->GetPrimaries(); // vector

  auto andMap = ev->GetEventAnode(); // mappa con i  2 anodi

  qev->ResetEvent();
  qev->SetEventNr(ev->GetEvNumber());

  TVector3 drift_direction(1, 0, 0);

  // Get data only from the TPC we are interested in
  int tpc_id = 11;                               // chiave
  auto anode = ev->GetEventAnodeByTPCID(tpc_id); // valore

  double z_max = 0;

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
        if (p.second->GetNhits() > z_max)
          z_max = p.second->GetNhits(); // carica massima per colori
      }
      // getchar();
    }
  }

  qev->SourceHits3DHist(xyz_hits);

  qev->Clustering();

  if (qev->GetTotalCharge() == 0)
  {
    printf("There isn't any event in this TPC.\n");
    return;
  }

  // Define a vector that will contain the number of non void bin (of the bigger cluster) on the orizontal axis of each projection
  std::vector<int> N_non_void_bin_oriz_axis;
  std::vector<int> charge_oriz_axis;

  std::vector<TString> projectionsList = {"y:x", "y:z", "z:x", "x:y", "z:y", "x:z"};

  for (const auto projection : projectionsList)
  {

    auto strArray = projection.Tokenize(":");
    std::vector<TVector3> axesList;
    std::vector<int> axesIndexes;

    for (const auto &obj : *strArray)
    {
      TObjString *str = (TObjString *)obj;
      TString strAxis = str->GetString();

      if (strAxis == "x")
      {
        axesList.push_back(TVector3(1, 0, 0));
        axesIndexes.push_back(0);
      }
      else if (strAxis == "y")
      {
        axesList.push_back(TVector3(0, 1, 0));
        axesIndexes.push_back(1);
      }
      else if (strAxis == "z")
      {
        axesList.push_back(TVector3(0, 0, 1));
        axesIndexes.push_back(2);
      }
    }

    if (strArray->GetEntries() != 2)
      return;

    TCanvas *cProjection2D = new TCanvas("cProjection2D" + projection, projection,
                                         0, 0, 800, 600);
    cProjection2D->SetTicks(1, 1);
    cProjection2D->cd();

    TH2D *h2 = qev->GetQHistN()->Projection(axesIndexes.at(0), axesIndexes.at(1));
    h2->SetName(Form("h2%s_ev%i", projection.Data(), ev->GetEvNumber()));
    h2->Draw("colz");

    // - - - - - - - - - - - - Define vector with vertex coordinates - - - - - - - - - - - -
    TVector3 vertex = (&(primaries.at(0)->GetVertex()).at(0));
    TMarker *m_vertex = new TMarker(vertex.Dot(axesList.at(1)), vertex.Dot(axesList.at(0)), 20);
    // printf("Vertice: %f, %f, %f\n", vertex.at(0), vertex.at(1), vertex.at(2)) ;
    m_vertex->Draw();

    //// - - - - - - - - - - - - If: only for hist with clusters - - - - - - - - - - - - - - - -

    //// NB: idealmente sarebbe meglio sistemare direttamente la funzione GetMaxCluster in modo che se non ci sono cluster possa comunque funzionare. Ma intanto lo teniamo così.
    // if (qev->GetTotalCharge() > 0)
    //{
    //   Obtain the cluster with the higher total charge (tcluster defined in SLArQCluster.hh)
    auto max_cluster = qev->GetMaxCluster();

    // Define a graph with the points of the max cluster and draw them
    TGraph *g_cluster = new TGraph();

    for (const auto &point : max_cluster->get_points())
    {
      g_cluster->AddPoint(point.fPos.Dot(axesList.at(1)), point.fPos.Dot(axesList.at(0)));
    }

    g_cluster->SetName(Form("g_cluster_%s_%lu", projection.Data(), max_cluster->get_id()));
    g_cluster->SetMarkerColor(kBlack);
    g_cluster->SetLineWidth(2);
    g_cluster->SetMarkerStyle(108);
    g_cluster->Draw("p");

    // Create an hist as the projection (on the axis considered on this run of the loop) of the 3D hist created from the max cluster
    TH2D *h2_max_cl = qev->GetMaxClusterHn()->Projection(axesIndexes.at(0), axesIndexes.at(1));
    h2_max_cl->SetName(Form("h2_max_cluster_%s_ev%i", projection.Data(), ev->GetEvNumber()));

    // Obtain the indeces of the bin where the vertex is situated
    int id_x_vertex = h2_max_cl->GetXaxis()->FindBin(m_vertex->GetX()); // Indice bin asse x
    int id_y_vertex = h2_max_cl->GetYaxis()->FindBin(m_vertex->GetY()); // Indice bin asse x

    int charge_x_min = 0; // Togliere se decido di usare i non_void_bin. Altrimenti viceversa.
    int charge_x_maj = 0;
    int charge_y_min = 0;
    int charge_y_maj = 0;

    double non_void_bin_x_min = 0;
    double non_void_bin_x_maj = 0;
    double non_void_bin_y_min = 0;
    double non_void_bin_y_maj = 0;

    // Decide the width of bins interval to consider in the next section
    int N_x = -1;
    if (h2_max_cl->GetXaxis()->GetBinWidth(1) / 4 > 0.99)
      N_x = 1;
    else
      N_x = 2;

    // printf("Divisione bin delle x: %f\n", h2_max_cl->GetXaxis()->GetBinWidth(1) / 4);
    // printf("Ampiezza bin delle x: %f\n", h2_max_cl->GetXaxis()->GetBinWidth(1));
    // printf("Numero di bin considerati per le x: %i\n", N_x);

    int N_y = -1;
    if (h2_max_cl->GetYaxis()->GetBinWidth(1) / 4 > 0.99)
      N_y = 1;
    else
      N_y = 2;
    // printf("Divisione bin delle y: %f\n", h2_max_cl->GetYaxis()->GetBinWidth(1) / 4);
    // printf("Ampiezza bin delle y: %f\n", h2_max_cl->GetYaxis()->GetBinWidth(1));
    // printf("Numero di bin considerati per le y: %i\n", N_y);

    // Loop to obtain information on the bins around the vertex, in order to decide approximately the direction of the particle (right/left and up/down)
    //  We consider\ the bins along the orizontal and along the vertical coordinate of the vertex, plus the bins up/down and right/left (within the interval set in the previous section)
    for (int ix = 1; ix < h2_max_cl->GetNbinsX() + 1; ix++)
    {
      for (int iy = id_y_vertex - N_y; iy <= id_y_vertex + N_y; iy++)
      {
        if (ix < id_x_vertex)
        {
          charge_x_min += h2_max_cl->GetBinContent(ix, iy);
          if (h2_max_cl->GetBinContent(ix, iy) > 0)
            non_void_bin_x_min += 1 * h2_max_cl->GetXaxis()->GetBinWidth(1);
        }
        if (ix > id_x_vertex)
        {
          charge_x_maj += h2_max_cl->GetBinContent(ix, iy);
          if (h2_max_cl->GetBinContent(ix, iy) > 0)
            non_void_bin_x_maj += 1 * h2_max_cl->GetXaxis()->GetBinWidth(1);
        }
      }
    }

    for (int ix = id_x_vertex - N_x; ix <= id_x_vertex + N_x; ix++)
    {
      for (int iy = 1; iy < h2_max_cl->GetNbinsY() + 1; iy++)
      {
        if (iy < id_y_vertex)
        {
          charge_y_min += h2_max_cl->GetBinContent(ix, iy);
          if (h2_max_cl->GetBinContent(ix, iy) > 0)
            non_void_bin_y_min += 1 * h2_max_cl->GetYaxis()->GetBinWidth(1);
        }
        if (iy > id_y_vertex)
        {
          charge_y_maj += h2_max_cl->GetBinContent(ix, iy);
          if (h2_max_cl->GetBinContent(ix, iy) > 0)
            non_void_bin_y_maj += 1 * h2_max_cl->GetYaxis()->GetBinWidth(1);
        }
      }
    }

    printf("non_void_bin_x_min: %f\n", non_void_bin_x_min);
    printf("non_void_bin_x_maj: %f\n", non_void_bin_x_maj);
    printf("non_void_bin_y_min: %f\n", non_void_bin_y_min);
    printf("non_void_bin_y_maj: %f\n", non_void_bin_y_maj);

    // Save the number of non void bin along the orizontal axis for this projection
    N_non_void_bin_oriz_axis.push_back(non_void_bin_x_maj + non_void_bin_x_min);

    int dir_x = 100;

    if (non_void_bin_x_min > non_void_bin_x_maj)
      dir_x = -1;

    else if (non_void_bin_x_min < non_void_bin_x_maj)
      dir_x = 1;

    else // Bisognerebbe guardare il caso in cui entrambe sono uguali a 0 --> tracce verticali
      dir_x = 0;

    int dir_y = 100;

    if (non_void_bin_y_min > non_void_bin_y_maj)
      dir_y = -1;

    else if (non_void_bin_y_min < non_void_bin_y_maj)
      dir_y = 1;

    else
      dir_y = 0;

    printf("Direzione lungo x: %i\n", dir_x);
    printf("Direzione lungo y: %i\n", dir_y);

    // Define the graph where each point is the weighted mean of the y bins on the bin content, given the x
    TGraphErrors *g_max_cl = new TGraphErrors;

    double num = 0;
    int total_bin_content_x = 0;
    double ey = 0;

    // Loop to calculate the weighted mean
    for (int ix = 1; ix < h2_max_cl->GetNbinsX() + 1; ix++)
    {
      float x_bin = h2_max_cl->GetXaxis()->GetBinCenter(ix);

      for (int iy = 1; iy < h2_max_cl->GetNbinsY() + 1; iy++)
      {
        float y_bin = h2_max_cl->GetYaxis()->GetBinCenter(iy);
        // printf("num: %f", y_bin);
        total_bin_content_x += h2_max_cl->GetBinContent(ix, iy);
        num += h2_max_cl->GetBinContent(ix, iy) * y_bin;
      }
      // printf("Total bin content: %d", total_bin_content_x);

      // Define the y error. In a preliminary approximation is chosen constant, considering a uniform distribution of the events on the bin
      ey = h2_max_cl->GetYaxis()->GetBinWidth(1) / sqrt(12);

      if (total_bin_content_x != 0) //
      {
        g_max_cl->AddPoint(x_bin, (num / total_bin_content_x));
        g_max_cl->SetPointError(g_max_cl->GetN() - 1, 0, ey);
      }

      num = 0;
      total_bin_content_x = 0;
    }
    printf("Errore sulle y: %f\n", ey);

    // printf("Bins number: %d\n", h2_max_cl->GetNbinsX());
    // printf("Bins number: %d\n", h2_max_cl->GetNbinsY());

    g_max_cl->SetName("g_max_cl");
    g_max_cl->SetLineColor(kBlack);
    g_max_cl->SetLineWidth(2);
    g_max_cl->Draw("pl");

    // - - - - - - - - - - - - - - Fit - - - - - - - - - - - - - -
    int N_point_fit = 2;
    int N_par = 2;

    // printf("Point 0: %f\n", g_max_cl->GetPointX(0));
    // printf("Point max: %f\n", g_max_cl->GetPointX(g_max_cl->GetN() - 1));

    // if (dir_x > 0)
    // {
    //   TF1 *fline = new TF1("fline", line, h2_max_cl->GetXaxis()->GetBinCenter(id_x_vertex), h2_max_cl->GetXaxis()->GetBinCenter(id_x_vertex) + h2_max_cl->GetXaxis()->GetBinWidth(1) * (N_point_fit +0.99) , N_par);
    //   TFitResultPtr fit_g_max_cl = g_max_cl->Fit(fline, "S", "", h2_max_cl->GetXaxis()->GetBinCenter(id_x_vertex), h2_max_cl->GetXaxis()->GetBinCenter(id_x_vertex) + h2_max_cl->GetXaxis()->GetBinWidth(1) * (N_point_fit +0.99));
    // }
    // else
    // {
    //   TF1 *fline = new TF1("fline", line, h2_max_cl->GetXaxis()->GetBinCenter(id_x_vertex) - h2_max_cl->GetXaxis()->GetBinWidth(1) * (N_point_fit +0.99), h2_max_cl->GetXaxis()->GetBinCenter(id_x_vertex), N_par);
    //   TFitResultPtr fit_g_max_cl = g_max_cl->Fit(fline, "S", "", h2_max_cl->GetXaxis()->GetBinCenter(id_x_vertex) - h2_max_cl->GetXaxis()->GetBinWidth(1) * (N_point_fit +0.99), h2_max_cl->GetXaxis()->GetBinCenter(id_x_vertex));
    // }

    if (dir_x > 0)
    {
      TF1 *fline = new TF1("fline", line, m_vertex->GetX(), m_vertex->GetX() + h2_max_cl->GetXaxis()->GetBinWidth(1) * (N_point_fit + 0.99), N_par);
      TFitResultPtr fit_g_max_cl = g_max_cl->Fit(fline, "S", "", m_vertex->GetX(), m_vertex->GetX() + h2_max_cl->GetXaxis()->GetBinWidth(1) * (N_point_fit + 0.99));
    }
    else if (dir_x < 0)
    {
      TF1 *fline = new TF1("fline", line, m_vertex->GetX() - h2_max_cl->GetXaxis()->GetBinWidth(1) * (N_point_fit + 0.99), m_vertex->GetX(), N_par);
      TFitResultPtr fit_g_max_cl = g_max_cl->Fit(fline, "S", "", m_vertex->GetX() - h2_max_cl->GetXaxis()->GetBinWidth(1) * (N_point_fit + 0.99), m_vertex->GetX());
    }
    else
    {
      TF1 *fline = new TF1("fline", line, m_vertex->GetX() - h2_max_cl->GetXaxis()->GetBinWidth(1) * (N_point_fit + 0.99), m_vertex->GetX() + h2_max_cl->GetXaxis()->GetBinWidth(1) * (N_point_fit + 0.99), N_par);
      TFitResultPtr fit_g_max_cl = g_max_cl->Fit(fline, "S", "", m_vertex->GetX() - h2_max_cl->GetXaxis()->GetBinWidth(1) * (N_point_fit + 0.99), m_vertex->GetX() + h2_max_cl->GetXaxis()->GetBinWidth(1) * (N_point_fit + 0.99));
    }

    // Devo segnarmi i parametri da qualche parte (momentaneamente con dei vector? poi qualcosa di più strutturato)
    //}

    // - - - - - - - - - - - - Print the primaries trajectories - - - - - - - - - - - -
    auto pdg = TDatabasePDG::Instance();

    for (const auto &p : primaries)
    {
      printf("----------------------------------------\n");
      printf("PRIMARY vertex: %s - K0 = %2f - t = %.2f - vtx [%.1f, %.1f, %.1f]\n",
             p->GetParticleName().Data(), p->GetEnergy(), p->GetTime(),
             p->GetVertex()[0], p->GetVertex()[1], p->GetVertex()[2]);
      auto trajectories = p->GetTrajectories();
      for (const auto &t : trajectories)
      {
        auto points = t->GetPoints();
        auto pdg_particle = pdg->GetParticle(t->GetPDGID());
        // printf("%s [%i]: t = %.2f, K = %.2f - n_scint = %g, n_elec = %g\n",
        // t->GetParticleName().Data(), t->GetTrackID(),
        // t->GetTime(),
        // t->GetInitKineticEne(),
        // t->GetTotalNph(), t->GetTotalNel());
        if (t->GetInitKineticEne() < 0.01)
          continue;
        TGraph *g = new TGraph();
        Color_t col = kBlack;
        TString name = "";

        if (!pdg_particle)
        {
          col = kBlack;
          name = Form("g%s_%i_trk%i", projection.Data(), t->GetPDGID(), t->GetTrackID());
        }
        else
        {
          if (pdg_particle == pdg->GetParticle(22))
            col = kYellow; // γ
          else if (pdg_particle == pdg->GetParticle(11))
            col = kBlue - 6; // e-
          else if (pdg_particle == pdg->GetParticle(-11))
            col = kRed - 7; // e+
          else if (pdg_particle == pdg->GetParticle(2212))
            col = kRed; // p
          else if (pdg_particle == pdg->GetParticle(2112))
            col = kBlue; // n
          else if (pdg_particle == pdg->GetParticle(-211))
            col = kOrange + 7; // pi-
          else if (pdg_particle == pdg->GetParticle(211))
            col = kViolet - 2; // pi+
          else if (pdg_particle == pdg->GetParticle(111))
            col = kGreen; // pi0
          else
            col = kGray + 2;
          name = Form("g%s_%s_trk_%i",
                      projection.Data(), pdg_particle->GetName(), t->GetTrackID());
        }

        for (const auto &pt : points)
        {
          if (pt.fCopy == tpc_id)
            g->AddPoint(
                // TVector3(pt.fX, pt.fY, pt.fZ).Dot( AnodeSysCfg[tpc_id]->GetAxis0()),
                // TVector3(pt.fX, pt.fY, pt.fZ).Dot( AnodeSysCfg[tpc_id]->GetAxis1()) );
                TVector3(pt.fX, pt.fY, pt.fZ).Dot(axesList.at(1)),
                TVector3(pt.fX, pt.fY, pt.fZ).Dot(axesList.at(0)));
        }
        g->SetName(name);
        g->SetLineColor(col);
        g->SetLineWidth(2);
        if (g->GetN() > 2)
          g->Draw("l");
      }
    }
    cProjection2D->Modified();
    cProjection2D->Update();
  }

  // - - - - - - - - - - - - - - Find the two best projections - - - - - - - - - - - -
  // NB: the best projections are the ones with higher non void bin number on the orizontal axis
  int index_best_proj_1 = -1;
  int index_best_proj_2 = -1;

  int N_bin_best_proj_1 = 0;
  int N_bin_best_proj_2 = 0;

  for (int i = 0; i < N_non_void_bin_oriz_axis.size(); i++)
  {
    if (N_non_void_bin_oriz_axis.at(i) > N_bin_best_proj_1)
    {
      if (i != index_best_proj_1 + 3)
      {
      index_best_proj_2 = index_best_proj_1;
      N_bin_best_proj_2 = N_bin_best_proj_1;
      }

      index_best_proj_1 = i;
      N_bin_best_proj_1 = N_non_void_bin_oriz_axis.at(i);
    }
    else if (N_non_void_bin_oriz_axis.at(i) > N_bin_best_proj_2)
    {
      if (index_best_proj_2 != index_best_proj_1 + 3)
      {
        index_best_proj_2 = i;
        N_bin_best_proj_2 = N_non_void_bin_oriz_axis.at(i);
      }
    }
  }

  printf("Best projection index: %d\n", index_best_proj_1);
  printf("Second best projection index: %d\n", index_best_proj_2);

  std::vector<TString> best_projections;
  best_projections.push_back(projectionsList.at(index_best_proj_1));
  best_projections.push_back(projectionsList.at(index_best_proj_2));

  printf("Migliori proiezioni: %s, %s\n", best_projections.at(0).Data(), best_projections.at(1).Data());

  // - - - - - - - - - - - - - - Reconstruct electron direction - - - - - - - - - - - - - -

  int cx = -1;
  int cy = -1;
  int cz = -1;

  TVector3 vElectron_direction;
  TVector3 vSun_direction(36, 41, 18); // Da modificare con quella corretta del Sole

  for (const auto projection : best_projections)
  {
    auto strArray = projection.Tokenize(":");
    std::vector<TVector3> axesList; // cambio nome

    TVector3 vProj_direction;

    for (const auto &obj : *strArray)
    {
      TObjString *str = (TObjString *)obj;
      TString strAxis = str->GetString();

      if (strAxis == "x" && cx < 0)
      {
        axesList.push_back(TVector3(1, 0, 0));
        cx = 1;
      }
      else if (strAxis == "y" && cy < 0)
      {
        axesList.push_back(TVector3(0, 1, 0));
        cy = 1;
      }
      else if (strAxis == "z" && cz < 0)
      {
        axesList.push_back(TVector3(0, 0, 1));
        cz = 1;
      }
      else
      {
        axesList.push_back(TVector3(0, 0, 0));
      }
    }

    if (strArray->GetEntries() != 2)
      return;

    double norm = sqrt(3 * 3 + 1 * 1);       // Normalization                       // sostituire m al posto di 3 --> devo aver memorizzato m nella structure
    vProj_direction += (axesList.at(1) * (1 / norm) + axesList.at(0) * (3 / norm)); // sostituire m al posto di 3 --> devo aver memorizzato m nella structure
                                                                                    // Assegno valore 1 alla coordinata orizzontale (che è la seconda della lista), per cui la coordinata dell'asse verticale sarà uguale a m

    printf("Direzione nella proiezione: %f, %f, %f\n", vProj_direction.X(), vProj_direction.Y(), vProj_direction.Z());

    vElectron_direction += vProj_direction; // Volendo si può togliere vProj_direction e salvare tutto direttamente in vElectron_direction, però così per ora mi sembra più comprensibile per la lettura
  }

  printf("Direzione complessiva per l'elettrone: %f, %f, %f\n", vElectron_direction.X(), vElectron_direction.Y(), vElectron_direction.Z());

  double cosTheta = vElectron_direction.Dot(vSun_direction) / sqrt(vElectron_direction.Dot(vElectron_direction) * vSun_direction.Dot(vSun_direction));
  printf("Cosine of theta: %f\n", cosTheta);

  return;
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

  for (int iev = 0; iev < mc_tree->GetEntries(); iev++)
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
