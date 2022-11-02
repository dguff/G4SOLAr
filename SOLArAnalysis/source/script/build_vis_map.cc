/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : build_vis_map.cc
 * @created     : Monday Oct 17, 2022 17:56:23 CEST
 */

#include <iostream>
#include <getopt.h>
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


#include "event/SLArMCEvent.hh"
#include "config/SLArCfgBaseSystem.hh"
#include "config/SLArCfgMegaTile.hh"
#include "config/SLArCfgSuperCellArray.hh"

#include "solar_root_style.hpp"

typedef SLArCfgBaseSystem<SLArCfgSuperCellArray> SLArPDSCfg;
typedef SLArCfgBaseSystem<SLArCfgMegaTile> SLArPixCfg;

#include "SLArLightPropagationModel.hh"

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
  SLArPDSCfg*  scCfg = (SLArPDSCfg*)file->Get("PDSSysConfig");   

  // create TH3D for storing the visibility map
  TH3D* hvisPixSys = new TH3D("hvisPix", Form("%s visibility", pixCfg->GetName()), 
      18 , -1800, +1800, 
      20 , -3000, +3000, 
      28, -7000, 7000); 
  TH3D* hvisSCSys  = new TH3D("hvisSC", Form("%s visibility", scCfg->GetName()), 
      18 , -1800, +1800, 
      20 , -3000, +3000, 
      28 , -7000, 7000); 

  // Create semi-analytical light propagation model 
  slarAna::SLArLightPropagationModel lightModel;
  lightModel.SetDetectorClass(slarAna::kNorth  , slarAna::kReadoutTile);
  lightModel.SetDetectorClass(slarAna::kSouth  , slarAna::kReadoutTile);
  lightModel.SetDetectorClass(slarAna::kTop    , slarAna::kSuperCell);
  lightModel.SetDetectorClass(slarAna::kBottom , slarAna::kSuperCell);

  // loop over the map's bins and compute the local visibility
  int ibin = 0; 
  for (int ixbin = 1; ixbin <= hvisPixSys->GetNbinsX(); ixbin++) {
    for (int iybin = 1; iybin <= hvisPixSys->GetNbinsY(); iybin++) {
      for (int izbin = 1; izbin <= hvisPixSys->GetNbinsZ(); izbin++) {
        double x_ = hvisPixSys->GetXaxis()->GetBinCenter(ixbin)/G4UIcommand::ValueOf("cm"); 
        double y_ = hvisPixSys->GetYaxis()->GetBinCenter(iybin)/G4UIcommand::ValueOf("cm"); 
        double z_ = hvisPixSys->GetZaxis()->GetBinCenter(izbin)/G4UIcommand::ValueOf("cm"); 

        double vis = 0.; 
        for (const auto &mod : pixCfg->GetModuleMap()) {
          for (auto &tile : mod.second->GetMap()) {
            vis += lightModel.VisibilityOpDetTile(tile.second, TVector3(x_, y_, z_));  
          }
        }

        if (ibin%50 == 0) printf("[%i, %i, %i]\n", ixbin, iybin, izbin); 
        hvisPixSys->SetBinContent(ixbin, iybin, izbin, vis); 
        ibin++; 
      }
    }
  }

  // loop over the map's bins and compute the local visibility
  ibin = 0; 
  for (int ixbin = 1; ixbin <= hvisSCSys->GetNbinsX(); ixbin++) {
    for (int iybin = 1; iybin <= hvisSCSys->GetNbinsY(); iybin++) {
      for (int izbin = 1; izbin <= hvisSCSys->GetNbinsZ(); izbin++) {
        double x_ = hvisSCSys->GetXaxis()->GetBinCenter(ixbin)/G4UIcommand::ValueOf("cm"); 
        double y_ = hvisSCSys->GetYaxis()->GetBinCenter(iybin)/G4UIcommand::ValueOf("cm"); 
        double z_ = hvisSCSys->GetZaxis()->GetBinCenter(izbin)/G4UIcommand::ValueOf("cm"); 

        double vis = 0.; 
        for (const auto &mod : scCfg->GetModuleMap()) {
          for (auto &tile : mod.second->GetMap()) {
            vis += lightModel.VisibilityOpDetTile(tile.second, TVector3(x_, y_, z_));  
          }
        }

        if (ibin%50 == 0) printf("[%i, %i, %i]\n", ixbin, iybin, izbin); 
        hvisSCSys->SetBinContent(ixbin, iybin, izbin, vis); 
        ibin++; 
      }
    }
  }


  if ( (output_path != NULL) && (output_path[0] !=  '\0') ) {
    TFile* fvismap = new TFile(output_path, "recreate"); 
    hvisPixSys->Write();
    hvisSCSys ->Write(); 
    fvismap->Close(); 
  }

  return;
}

void PrintUsage() {
  printf("build_vis_map: Build a visibility map based on the semi-analytical light propagation model\n");
  printf("Usage:\nbuild_vis_map\n");
  printf("\t-i(--input) input file with PDS configuration\n");
  printf("\t-o(--output) output file with visibility map\n"); 
  printf("\t-h(--help) print this message\n");
}

int main(int argc, char *argv[])
{
  const char* short_opts = "i:o:h";
  static struct option long_opts[4] = 
  {
    {"input", required_argument, 0, 'i'}, 
    {"output", required_argument, 0, 'o'}, 
    {"help", no_argument, 0, 'h'}, 
    {nullptr, no_argument, nullptr, 0} 
  };

  int c, option_index; 

  const char* input_file = ""; 
  const char* output_file = ""; 

  while ( (c = getopt_long(argc, argv, short_opts, long_opts, &option_index)) != -1) {
    switch(c) {
      case 'i' : 
        input_file = optarg; 
        break;
      case 'o':
        output_file = optarg; 
        break;
      case 'h':
        PrintUsage(); 
        return 4; 
        break;
    }
  }
  
  build_vis_map(input_file, output_file); 
  return 0;
}
