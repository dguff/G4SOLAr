/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : gdml_viewer
 * @created     : giovedì ago 04, 2022 12:58:44 CEST
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
#include "TGeoManager.h"
#include "TGeoPainter.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"
#include "TGLViewer.h"

#include "config/SLArCfgBaseSystem.hh"
#include "event/SLArMCEvent.hh"
#include "config/SLArCfgMegaTile.hh"
#include "SLArCfgSuperCellArray.hh"

typedef SLArCfgBaseSystem<SLArCfgSuperCellArray> SLArPDSCfg;
typedef SLArCfgBaseSystem<SLArCfgMegaTile> SLArPixCfg;


void gdml_viewer(const char* mc_file, const char* gdml_world = "slar_world.gdml") 
{

  // drawing detector geometry 
  TGeoManager::Import(gdml_world); 
  auto vol_list = gGeoManager->GetListOfVolumes(); 
  
  for (int iv = 0; iv < vol_list->GetSize(); iv++) {
    auto vol = vol_list->At(iv); 
    if (vol) {
      TGeoVolume* gvol = (TGeoVolume*)vol; 
      printf("vol[%i/%i] name: %s, id %i\n", 
          iv, vol_list->GetSize(), vol->GetName(), gvol->GetNumber());
      //gvol->SetVisibility(false); 
      if (gvol->IsAssembly()) {
        printf("This is an assembly!\n"); 
      }

    }
  }

  gGeoManager->GetVolume("VesselLV")->SetVisibility(false); 
  gGeoManager->GetVolume("TankLV")->SetVisibility(false); 

  TGeoVolume* gvTarget = gGeoManager->GetVolume("TargetLV"); 
  gvTarget->GetMaterial()->SetTransparency(60); 
  gvTarget->SetLineColor(kAzure-4); 

  TGeoVolume* gvRDTilePCB = gGeoManager->GetVolume("PCBBaseLV"); 
  gvRDTilePCB->SetVisibility(true); 
  gvRDTilePCB->SetLineColor(kGreen+2); 

  gGeoManager->GetVolume("rdtile_cell_plane_lv")->SetVisibility(true);
  gGeoManager->GetVolume("rdtile_cell_plane_lv")->SetLineColor(kBlue); 

  gGeoManager->GetVolume("SiPM_lv")->SetVisibility(true); 
  gGeoManager->GetVolume("SiPM_lv")->SetLineColor(kRed+1); 

  gGeoManager->GetVolume("World")->Draw("ogl"); 
  TGLViewer* glviewer = (TGLViewer*)gPad->GetViewer3D(); 
  glviewer->SetStyle(TGLRnrCtx::kOutline); 

  //---------------------------------------------------------------------------
  // reading mc event file
  TFile* file = new TFile(mc_file); 

  SLArPDSCfg* pdsCfg = (SLArPDSCfg*)file->Get("PDSSysConfig"); 
  SLArPixCfg* pixCfg = (SLArPixCfg*)file->Get("PixSysConfig"); 

  if (pdsCfg) {
    for (auto &array : pdsCfg->GetModuleMap()) {
      SLArCfgSuperCellArray* scArray = array.second; 
      scArray->BuildPolyBinHist();
      new TCanvas(); 
      scArray->GetTH2()->Draw("col");
    }
  }


  TTree* tree = (TTree*)file->Get("EventTree"); 
  
  SLArMCEvent* ev = nullptr; 
  tree->SetBranchAddress("MCEvent", &ev); 

  TH1D* hvis = new TH1D("hvis", "Visible Energy", 200, 0., 20); 

  for (int iev = 0; iev<1; iev++) {
    tree->GetEntry(iev); 

    auto primaries = ev->GetPrimaries(); 
    double ev_edep = 0; 


    size_t ip = 0; 
    for (const auto &p : primaries) {
      int pPDGID = p->GetCode();     // Get primary PDG code 
      int pTrkID = p->GetTrackID();  // Get primary trak id   
      double primary_edep = 0; 

      if (pPDGID == 12 || pPDGID == -12) continue;

      auto trajectories = p->GetTrajectories(); 
      int itrj = 0;
      for (const auto &trj : trajectories) {
        const size_t npoints = trj->GetPoints().size(); 
        std::vector<double> step_x(npoints);
        std::vector<double> step_y(npoints);
        std::vector<double> step_z(npoints);

        int istep = 0; 
        double trj_edep = 0.;
        double point[3] = {0}; 
        for (const auto &tp : trj->GetPoints()) {
          point[0] = tp.fX; point[1] = tp.fY, point[2] = tp.fZ; 
          if (gvTarget->Contains(point)) {
            step_x.at(istep) = tp.fX;     // x coordinate [mm]
            step_y.at(istep) = tp.fY;     // y coordinate [mm]
            step_z.at(istep) = tp.fZ;     // z coordinate [mm]
            double edep      = tp.fEdep;  // Energy deposited in the step [MeV]
            ev_edep += edep; 
            trj_edep += edep; 

            istep++; 
          } else {
            break;
          }
        }

        if (trj->GetPDGID() !=  12 && 
            trj->GetPDGID() != -12 && 
            trj->GetPDGID() !=  22 &&
            trj->GetInitKineticEne() > 0.5) {
          TPolyLine3D _trj(istep, &step_x[0], &step_y[0], &step_z[0]); 
          _trj.SetLineWidth(2); 
          if      (trj->GetPDGID() == 11) {_trj.SetLineColor(kYellow);}
          else if (trj->GetPDGID() == 22) {_trj.SetLineColor(kGreen);}
          else if (trj->GetPDGID() == 1000000010) {_trj.SetLineColor(kBlue+1);}
          else if (trj->GetPDGID() == 1000010010) {_trj.SetLineColor(kRed+1);}
          else if (trj->GetPDGID() == -11) {_trj.SetLineColor(kOrange+7);}
          else {_trj.SetLineColor(kBlack);}

          _trj.DrawClone("ogl same"); 
        } 
        // Draw gamma rays steps as markers
        else if (trj->GetPDGID() == 22 &&
                   trj->GetInitKineticEne() > 0.5) {
          TPolyMarker3D _trj(istep, 20); 
          for (int igs =0; igs<istep; igs++) {
            _trj.SetPoint(igs, step_x[igs], step_y[igs], step_z[igs]); 
          }
          _trj.SetMarkerSize(4); 
          _trj.SetMarkerColor(kGreen); 
          _trj.DrawClone("ogl same"); 
        }
        itrj++;
      } 
      ip++;
    }

    hvis->Fill(ev_edep); 
  }

  TH2D* h2frame = new TH2D("h2frame", "Pix Readout", 700, -7e3, 7e3, 600, -3e3, +3e3);
  TCanvas* cPix = new TCanvas("cPix", "Pixel system readout", 0, 0, 1000, 400);
  cPix->cd(); 
  h2frame->Draw("axis"); 
  double hmax = 0; 
  double tmax = 0; 

  auto pixSys = ev->GetReadoutTileSystem(); 

  std::map<int, TH2Poly*> hpTime;

  for (const auto &mtile : pixSys->GetMegaTilesMap()) {
    printf("ReadoutTile System: %s [%i]\n", 
        mtile.second->GetName(), mtile.first);
    auto mgTileCfg = pixCfg->GetModule(mtile.first);
    mgTileCfg->BuildPolyBinHist(); 

    hpTime.insert(
        std::make_pair(mgTileCfg->GetIdx(), 
          (TH2Poly*)mgTileCfg->GetTH2()->Clone(Form("%s_time", mgTileCfg->GetName()))
          )); 

    for (const auto &tile : mtile.second->GetTileMap()) {
      SLArEventTile* evTile = tile.second; 
      if (!evTile->GetHits().empty()) {
        int tile_idx = evTile->GetIdx();
        int bin_idx = mgTileCfg->GetBaseElement(tile_idx)->GetBinIdx();
        int nhits = evTile->GetNhits(); 
        double tfirst = evTile->GetTime(); 
        if (nhits > hmax) hmax = nhits; 
        if (tfirst > tmax) tmax = tfirst; 
        mgTileCfg->GetTH2()->SetBinContent(
            bin_idx,
            nhits
            );
        hpTime[mgTileCfg->GetIdx()]->SetBinContent(bin_idx, tfirst); 
      }
    }
  }
  
  gStyle->SetPalette(kSunset);
  printf("hmax is %g\n", hmax);

  int imap = 0; 
  for (auto &mtilecfg : pixCfg->GetModuleMap()) {
    mtilecfg.second->GetTH2()->GetZaxis()->SetRangeUser(0., 1.1*hmax); 
    if (imap == 0) mtilecfg.second->GetTH2()->Draw("colz same");
    else mtilecfg.second->GetTH2()->Draw("col same");
    imap++;
  }

  return;
}

