/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : test_output
 * @created     : mercoledì lug 20, 2022 16:40:43 CEST
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

#include "config/SLArCfgBaseSystem.hh"
#include "event/SLArMCEvent.hh"
#include "config/SLArCfgMegaTile.hh"
#include "SLArCfgSuperCellArray.hh"

typedef SLArCfgBaseSystem<SLArCfgSuperCellArray> SLArPDSCfg;
typedef SLArCfgBaseSystem<SLArCfgMegaTile> SLArPixCfg;

void test_output(const char* path) 
{
  TFile* file = new TFile(path); 

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

  if (pixCfg) {
    TH2D* h2frame = new TH2D("h2frame", "Pix Readout", 700, -7e3, 7e3, 600, -3e3, +3e3);
    new TCanvas(); 
    h2frame->Draw("axis"); 

    for (auto &mod : pixCfg->GetModuleMap()) {
      SLArCfgMegaTile* mgTile = mod.second; 
      for (const auto &cell : mgTile->GetMap()) {
        printf("cell pos [phys]: [%.2f, %.2f, %.2f]\n", 
            cell.second->GetPhysX(), cell.second->GetPhysY(), cell.second->GetPhysZ()); 
      }
      mgTile->BuildPolyBinHist(); 
      mgTile->GetTH2()->Draw("colsame"); 

    }
  }

  TTree* tree = (TTree*)file->Get("EventTree"); 
  
  SLArMCEvent* ev = nullptr; 
  tree->SetBranchAddress("MCEvent", &ev); 

  for (int iev = 0; iev<10; iev++) {
    tree->GetEntry(iev); 

    auto primaries = ev->GetPrimaries(); 

    size_t ip = 0; 
    for (const auto &p : primaries) {
      int pPDGID = p->GetCode();     // Get primary PDG code 
      int pTrkID = p->GetTrackID();  // Get primary trak id   
      double primary_edep = 0; 

      auto trajectories = p->GetTrajectories(); 
      int itrj = 0;
      for (const auto &trj : trajectories) {
        for (const auto &tp : trj->GetPoints()) {
          double pos_x = tp.fX;     // x coordinate [mm]
          double pos_y = tp.fY;     // y coordinate [mm]
          double pos_z = tp.fZ;     // z coordinate [mm]
          double edep  = tp.fEdep;  // Energy deposited in the step [MeV]
        }
        itrj++;
      } 
      ip++;
    }
  }

 
  return;
}

