/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : plot_light_map
 * @created     : mercoledì ott 19, 2022 10:11:26 CEST
 * @brief       : macro to plot the light map used in the analysis
 */

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TLatex.h"

#include "solar_root_style.hpp"

void CanvasPartition(TCanvas *C,const Int_t Nx,const Int_t Ny,
    Float_t lMargin, Float_t rMargin,
    Float_t bMargin, Float_t tMargin); 

void plot_light_map(const char* map_file_path) 
{
  slide_default(); 
  gROOT->SetStyle("slide_default"); 

  TFile* file = new TFile(map_file_path); 
  TH3D* h3 = (TH3D*)file->Get("hvis"); 

  TCanvas* cMap = new TCanvas("cMap", "visibility map", 0, 0, 700, 850); 
  CanvasPartition(cMap, 3, 1, 0.12, 0.02, 0.12, 0.01); 
  TPad *pad[3]; 

  double y_quota[3] = {0, 1000, 2000};
  TH2D*  h2xz[3] = {nullptr}; 

  double vmin = 1e8; 
  double vmax = 0; 
  for (int j=0; j<3; j++) {
    int iybin = h3->GetYaxis()->FindBin(y_quota[j]); 
    h3->GetYaxis()->SetRange(iybin, iybin);

    h2xz[j] = (TH2D*)h3->Project3D("zx"); 
    h2xz[j]->SetName(Form("h2xz_%g", y_quota[j])); 
    h2xz[j]->SetTitle(Form("#it{y} = %g mm;#it{x} [mm];#it{z} [mm]", y_quota[j]));
    h2xz[j]->SetTitleFont(43, "xyzt"); 
    h2xz[j]->SetTitleSize(23, "xyzt"); 
    h2xz[j]->SetLabelFont(43, "xyzt"); 
    h2xz[j]->SetLabelSize(23, "xyzt"); 
    h2xz[j]->SetTitleOffset(5.0, "y"); 

    if (h2xz[j]->GetMaximum() > vmax) vmax = h2xz[j]->GetMaximum(); 
    if (h2xz[j]->GetMinimum() < vmin) vmin = h2xz[j]->GetMinimum(); 
  }

  double lpos[3] = {0.65, 0.50, 0.35}; 
  gStyle->SetOptStat(0); 
  gStyle->SetOptTitle(0); 
  TLatex label;
  label.SetTextFont(43); 
  label.SetTextSize(23); 
  label.SetTextAlign(21);
  for (int j=0; j<3; j++) {
    h2xz[j]->GetZaxis()->SetRangeUser(0.9*vmin, 1.1*vmax); 
    cMap->cd(0);
    TString padname = Form("pad_%i_0", j); 
    pad[j] = (TPad*)gROOT->FindObject(padname); 
    pad[j]->Draw(); 
    pad[j]->SetFillStyle(4000); 
    pad[j]->SetFrameFillStyle(4000); 
    pad[j]->SetTopMargin(0.05); 
    pad[j]->cd(); 

    pad[j]->SetTicks(1, 1); 
    if (j>0) {
      h2xz[j]->SetTitleSize(0.0, "y"); 
      h2xz[j]->SetLabelSize(0.0, "y"); 
    }

    TString opt = "col"; 
    if (j==2) opt += "z"; 
    h2xz[j]->Draw(opt); 

    label.DrawLatexNDC(lpos[j], 0.97, Form("#it{y} = %g mm", y_quota[j])); 
  }
  
  return;
}


void CanvasPartition(TCanvas *C,const Int_t Nx,const Int_t Ny,
    Float_t lMargin, Float_t rMargin,
    Float_t bMargin, Float_t tMargin)
{
  if (!C) return;
  // Setup Pad layout:
  Float_t vSpacing = 0.0;
  Float_t vStep  = (1.- bMargin - tMargin - (Ny-1) * vSpacing) / Ny;
  Float_t hSpacing = 0.0;
  Float_t hStep  = (1.- lMargin - rMargin - (Nx-1) * hSpacing) / Nx;
  Float_t vposd,vposu,vmard,vmaru,vfactor;
  Float_t hposl,hposr,hmarl,hmarr,hfactor;
  for (Int_t i=0;i<Nx;i++) {
    if (i==0) {
      hposl = 0.0;
      hposr = lMargin + hStep;
      hfactor = hposr-hposl;
      hmarl = lMargin / hfactor;
      hmarr = 0.0;
    } else if (i == Nx-1) {
      hposl = hposr + hSpacing;
      hposr = hposl + hStep + rMargin;
      hfactor = hposr-hposl;
      hmarl = 0.0;
      hmarr = rMargin / (hposr-hposl);
    } else {
      hposl = hposr + hSpacing;
      hposr = hposl + hStep;
      hfactor = hposr-hposl;
      hmarl = 0.0;
      hmarr = 0.0;
    }
    for (Int_t j=0;j<Ny;j++) {
      if (j==0) {
        vposd = 0.0;
        vposu = bMargin + vStep;
        vfactor = vposu-vposd;
        vmard = bMargin / vfactor;
        vmaru = 0.0;
      } else if (j == Ny-1) {
        vposd = vposu + vSpacing;
        vposu = vposd + vStep + tMargin;
        vfactor = vposu-vposd;
        vmard = 0.0;
        vmaru = tMargin / (vposu-vposd);
      } else {
        vposd = vposu + vSpacing;
        vposu = vposd + vStep;
        vfactor = vposu-vposd;
        vmard = 0.0;
        vmaru = 0.0;
      }
      C->cd(0);
      char name[16];
      sprintf(name,"pad_%i_%i",i,j);
      TPad *pad = (TPad*) gROOT->FindObject(name);
      if (pad) delete pad;
      pad = new TPad(name,"",hposl,vposd,hposr,vposu);
      pad->SetLeftMargin(hmarl);
      pad->SetRightMargin(hmarr);
      pad->SetBottomMargin(vmard);
      pad->SetTopMargin(vmaru);
      pad->SetFrameBorderMode(0);
      pad->SetBorderMode(0);
      pad->SetBorderSize(0);
      pad->Draw();
    }
  }
}
