/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : solar_root_style.hpp
 * @created     : Thursday Oct 06, 2022 14:51:29 CEST
 *
 * ROOT style definitions for readable axis on slides and 
 * a couple of useful functions to add a date or "PRELIMINARY"
 * label to the plot.
 */

#ifndef SOLAR_ROOT_STYLE_HPP

#define SOLAR_ROOT_STYLE_HPP

#include <iostream>
#include "TStyle.h"
#include "TLatex.h"
#include "TInterpreter.h"


void slide_default() {
  printf("ROOT: sourcing SOLAr slide_default style...\n");

  TStyle* slide_default = new TStyle("slide_default", "Default SOLAr slide plot style");

  //------------------------------ Global settings
  slide_default->SetTextFont(43);
  slide_default->SetTextSize(23); 
  slide_default->SetPadTickX(1);
  slide_default->SetPadTickY(1);
  slide_default->SetPadLeftMargin  (0.12);
  slide_default->SetPadRightMargin (0.12);
  slide_default->SetPadBottomMargin(0.12);
  slide_default->SetPalette(kSunset);
  slide_default->SetCanvasBorderMode(0);
  slide_default->SetFrameBorderMode(0);
  slide_default->SetPadBorderMode(0);
  slide_default->SetCanvasColor(kWhite);
  slide_default->SetPadColor(kWhite);
  //------------------------------ Title settings
  slide_default->SetTitleX(0.5);
  slide_default->SetTitleY(0.99); 
  slide_default->SetTitleAlign(23);
  slide_default->SetTitleFont(43, "t");
  slide_default->SetTitleSize(23, "t");
  slide_default->SetTitleBorderSize(0);
  slide_default->SetTitleFillColor(0);
  //------------------------------ Stat settings
  slide_default->SetStatBorderSize(1);
  slide_default->SetStatFont(43);
  slide_default->SetStatFontSize(23); 
  slide_default->SetStatColor(0);
  //------------------------------ Axes settings
  slide_default->SetLabelFont(43, "xy");
  slide_default->SetLabelSize(23, "xy");
  slide_default->SetTitleFont(43, "xy");
  slide_default->SetTitleSize(23, "xy");
  slide_default->SetTitleOffset(1.0, "xy");
  //------------------------------ Hist settings
  slide_default->SetHistLineWidth(3);
  slide_default->SetHistLineColor(kBlue+1); 
  slide_default->SetMarkerStyle(20);
  //------------------------------ Legend settings
  slide_default->SetLegendFont(43);
  slide_default->SetLegendTextSize(23);
  slide_default->SetLegendBorderSize(1);
  
  return;
}

void add_date() {
  if (gStyle->GetOptDate() > 0) {
    gStyle->SetOptDate(0);
  } else {
    gStyle->SetOptDate(223);
    auto date_ = gStyle->GetAttDate();
    date_->SetTextFont(43);
    date_->SetTextSize(20);
    date_->SetTextAngle(90);
    gStyle->SetDateX( 1.0 - gStyle->GetPadRightMargin() +0.02);
    gStyle->SetDateY( 1.0 - gStyle->GetPadTopMargin  () +0.00);
  }

  return;
}

TLatex* add_preliminary(int ix, int iy) {
  double xx = 0.; 
  double yy = 0.;
  TLatex* txt = new TLatex(xx, yy, "#bf{SoLAr} Preliminary"); 
  txt->SetTextFont(43); 
  txt->SetTextSize(25); 
  txt->SetNDC(true); 

  if      (ix==0 && iy==0) {
    txt->SetTextAlign(11); 
    xx = gStyle->GetPadLeftMargin()+0.02; 
    yy = gStyle->GetPadBottomMargin()+0.02; 
  }
  else if (ix==0 && iy==1) {
    txt->SetTextAlign(13); 
    xx = gStyle->GetPadLeftMargin()+0.02; 
    yy = 1-gStyle->GetPadTopMargin()-0.02; 
  } 
  else if (ix==1 && iy==1) {
    txt->SetTextAlign(33); 
    xx = 1-gStyle->GetPadRightMargin()-0.02; 
    yy = 1-gStyle->GetPadTopMargin()-0.02; 
  } 
  else if (ix==1 && iy==0) {
    txt->SetTextAlign(31); 
    xx = 1-gStyle->GetPadRightMargin()-0.02; 
    yy = gStyle->GetPadBottomMargin()+0.02; 
  } 

  txt->SetX(xx); 
  txt->SetY(yy); 
  return txt;
}

#endif /* end of include guard SOLAR_ROOT_STYLE_HPP */

