/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : build_vis_map.C
 * @created     : Monday Oct 17, 2022 17:56:23 CEST
 */

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TH1D.h"
#include "TH3D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TRandom3.h"
#include "G4UIcommand.hh"

#include "config/SLArCfgBaseSystem.hh"
#include "event/SLArMCEvent.hh"
#include "config/SLArCfgMegaTile.hh"
#include "SLArCfgSuperCellArray.hh"

#include "solar_root_style.hpp"

typedef SLArCfgBaseSystem<SLArCfgSuperCellArray> SLArPDSCfg;
typedef SLArCfgBaseSystem<SLArCfgMegaTile> SLArPixCfg;

#include "VisMap/SLArLightPropagationModel.h"

void build_vis_map(const char* data_file_path, const char* output_path = "") 
{
  //--------------------------------------------------------- Source plot style 
  slide_default(); 
  gROOT->SetStyle("slide_default");
  gStyle->SetPalette(kSunset); 

  //-------------------------------------------------------------- Open MC file
  TFile* file = new TFile(data_file_path); 
  
  // Get the configuration of the pixel/SuperCell readout system
  SLArPixCfg* pixCfg = (SLArPixCfg*)file->Get("PixSysConfig"); 

  // create TH3D for storing the visibility map
  TH3D* hvis = new TH3D("hvis", Form("%s visibility", pixCfg->GetName()), 
      18, -1800, +1800, 
      30, -3000, +3000, 
      70, -7000, 7000); 

  // Create semi-analytical light propagation model 
  slarAna::SLArLightPropagationModel lightModel;

  // loop over the map's bins and compute the local visibility
  for (int ixbin = 1; ixbin <= hvis->GetNbinsX(); ixbin++) {
    for (int iybin = 1; iybin <= hvis->GetNbinsY(); iybin++) {
      for (int izbin = 1; izbin <= hvis->GetNbinsZ(); izbin++) {
        double x_ = hvis->GetXaxis()->GetBinCenter(ixbin)/G4UIcommand::ValueOf("cm"); 
        double y_ = hvis->GetYaxis()->GetBinCenter(iybin)/G4UIcommand::ValueOf("cm"); 
        double z_ = hvis->GetZaxis()->GetBinCenter(izbin)/G4UIcommand::ValueOf("cm"); 

        double vis = 0.; 
        for (const auto &mod : pixCfg->GetModuleMap()) {
          for (auto &tile : mod.second->GetMap()) {
            vis += lightModel.VisibilityOpDetTile(tile.second, TVector3(x_, y_, z_));  
          }
        }

        //printf("[%i, %i, %i]\n", ixbin, iybin, izbin); 
        hvis->SetBinContent(ixbin, iybin, izbin, vis); 
      }
    }
  }

  if ( (output_path != NULL) && (output_path[0] !=  '\0') ) {
    TFile* fvismap = new TFile(output_path, "recreate"); 
    hvis->Write();
    fvismap->Close(); 
  }

  return;
}

