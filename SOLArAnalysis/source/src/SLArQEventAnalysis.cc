/**
 * @author      Giulia Conti (unimib), Daniele Guffanti (unimib & infn-mib)
 * @file        SLArQEventAnalysis.cc
 * @created     Fri Jul 14, 2023 10:45:44 CEST
 */

#include "TObjArray.h"
#include "TObjString.h"
#include "TMarker.h"
#include "TPRegexp.h"
#include "TGraphErrors.h"

#include "SLArQEventAnalysis.hh"

ClassImp(slarq::SLArQEventAnalysis)

namespace slarq {

SLArQEventAnalysis::SLArQEventAnalysis() : fCfgAnode(nullptr), fLine(nullptr) 
{
  fLine = new TF1("line_fit", "[0] + x*[1]", -3e3, 3e3); 
  fLine->SetNpx(1e4); 
  fLine->SetLineStyle(7); 
  fLine->SetLineWidth(3); 
  fLine->SetLineColor(kGray+2);
}

SLArQEventAnalysis::SLArQEventAnalysis(const SLArCfgAnode* cfgAnode) 
  : fCfgAnode(cfgAnode) {}

SLArQEventAnalysis::~SLArQEventAnalysis() {}

void SLArQEventAnalysis::DrawProjection(SLArQEventReadout* qevent, const TVector3& vtx, const cluster_projection_info_t& proj_info) {

  // Draw main cluster projection
  auto  hn = qevent->GetMaxClusterHn(); 
  TH2D *h2 = hn->Projection(proj_info.fAxisIdx[0], proj_info.fAxisIdx[1]);
  h2->Draw("colz"); 

  float local_vtx_x = proj_info.fAxis[1].Dot(vtx); 
  float local_vtx_y = proj_info.fAxis[0].Dot(vtx);

  //printf("local vtx coord(%g, %g)\n", local_vtx_x, local_vtx_y); 

  int vtx_bin_x = h2->GetXaxis()->FindBin(local_vtx_x); 
  int vtx_bin_y = h2->GetYaxis()->FindBin(local_vtx_y); 

  // Draw vertex marker
  TMarker *m_vertex = new TMarker(local_vtx_x, local_vtx_y, 20);
  m_vertex->SetMarkerSize(1.5);
  m_vertex->Draw();

  // Draw reconstructed direction
  float len = 30;
  TGraph g_dir_rec(2); 
  fLine->SetParameters( proj_info.fPar0, proj_info.fPar1 ); 
  float x_rec_0 = h2->GetXaxis()->GetBinCenter(vtx_bin_x);
  g_dir_rec.SetPoint(0, x_rec_0, fLine->Eval(x_rec_0) );
  float x_rec_1 = x_rec_0 + 
    proj_info.fDirX * sqrt(len*len  / (1 + TMath::Sq(proj_info.fPar1)));
  g_dir_rec.SetPoint(1, x_rec_1, fLine->Eval(x_rec_1) ); 
  g_dir_rec.SetLineWidth(fLine->GetLineWidth()); 
  g_dir_rec.SetLineColor(fLine->GetLineColor()); 
  g_dir_rec.SetLineStyle(fLine->GetLineStyle()); 

  printf("reconstructed direction: %g + (%g) x \n", 
      fLine->GetParameter(0), fLine->GetParameter(1)); 
  printf("reconstructed direction: (%g, %g) -- (%g, %g)\n", 
      g_dir_rec.GetX()[0], g_dir_rec.GetY()[0], 
      g_dir_rec.GetX()[1], g_dir_rec.GetY()[1]);

  g_dir_rec.DrawClone("l");
}

cluster_projection_info_t SLArQEventAnalysis::ProcessProjection(const TString& projection, SLArQEventReadout* qevent, const TVector3& vtx) 
{
  // read axes from the projection string 
  std::vector<TVector3> axesList;
  std::vector<int> axesIndexes;
  std::vector<TString> axesLabels; 

  read_and_fill_axis(projection, &axesList, &axesIndexes, &axesLabels); 

  auto  hn = qevent->GetMaxClusterHn(); 
  
  TH2D *h2 = hn->Projection(axesIndexes.at(0), axesIndexes.at(1));
  h2->SetName(Form("h2_max_cluster_%s_ev%i", projection.Data(), qevent->GetEventNr()));

  float local_vtx_x = axesList.at(1).Dot(vtx); 
  float local_vtx_y = axesList.at(0).Dot(vtx);

  int vtx_bin_x = h2->GetXaxis()->FindBin(local_vtx_x); 
  int vtx_bin_y = h2->GetYaxis()->FindBin(local_vtx_y); 

  cluster_projection_info_t proj_info;
  proj_info.fAxis[1] = axesList.at(1); 
  proj_info.fAxis[0] = axesList.at(0); 
  proj_info.fAxisIdx[1] = axesIndexes.at(1); 
  proj_info.fAxisIdx[0] = axesIndexes.at(0); 
  proj_info.fAxisLabel[0] = axesLabels.at(0); 
  proj_info.fAxisLabel[1] = axesLabels.at(1); 
  
  // scan cluster projection to estimate the event direction 
  scan_cluster_proj(h2, vtx_bin_x, vtx_bin_y, proj_info); 
  // set event direction on the basis of the nr of hits found moving from the vertex
  proj_info.set_dir_xy( cluster_projection_info_t::kLength ); 

  // create graph to fit event direction from a weighted mean of the
  // cluster projection along the y-axis (in the local coordinate system)
  TGraphErrors g_max_cl; 
  float x_bin = 0, y_bin = 0, w_tot = 0, w = 0, num = 0; 
  // Define the x and y error. 
  // In a preliminary approximation is chosen constant, 
  // considering a uniform distribution of the events on the bin
  float ey = h2->GetYaxis()->GetBinWidth(1) / sqrt(12); 
  float ex = h2->GetXaxis()->GetBinWidth(1) / sqrt(12);

  // Loop to calculate the weighted mean
  for (int ix = 1; ix < h2->GetNbinsX() + 1; ix++)
  {
    float x_bin = h2->GetXaxis()->GetBinCenter(ix);

    for (int iy = 1; iy < h2->GetNbinsY() + 1; iy++)
    {
      float y_bin = h2->GetYaxis()->GetBinCenter(iy);
      // printf("num: %f", y_bin);
      w_tot += h2->GetBinContent(ix, iy);
      num += h2->GetBinContent(ix, iy) * y_bin;
    }

    if (w_tot != 0) //
    {
      g_max_cl.AddPoint(x_bin, (num / w_tot));
      g_max_cl.SetPointError(g_max_cl.GetN() - 1, ex, ey);
    }

    num = 0;
    w_tot = 0;
  }

  // fit the graph and store the parameters in proj_info
  float fit_range[2] = {0}; 
  if (proj_info.fDirX == 1) {
    fit_range[0] = local_vtx_x; 
    fit_range[1] = fit_range[0] + 10;
  }
  else if (proj_info.fDirX == -1) {
    fit_range[1] = local_vtx_x; 
    fit_range[0] = fit_range[1] - 10; 
  } 
  else {
    printf("WARNING: fDirX not set properly\n");
  }
  //printf("local vertex coordinates: %.1f - %.1f\n", local_vtx_x, local_vtx_y);
  //printf("projection fit range: %.1f %.1f\n", fit_range[0], fit_range[1]);
  //for (int ip=0; ip<g_max_cl.GetN(); ip++) {
    //if (g_max_cl.GetX()[ip] > fit_range[0] && g_max_cl.GetX()[ip] < fit_range[1]){
      //printf("g_max_cl[%i] = (%g, %g)\n", ip, 
          //g_max_cl.GetX()[ip], g_max_cl.GetY()[ip]); 
    //}
  //}

  init_line_pars(g_max_cl, fit_range); 

  //printf("init parameters: %g, %g\n", fLine->GetParameter(0), fLine->GetParameter(1));

  g_max_cl.Fit(fLine, "nq", "", fit_range[0], fit_range[1]); 

  proj_info.fPar0 = fLine->GetParameter(0); 
  proj_info.fPar1 = fLine->GetParameter(1); 

  if (hn) delete hn; 
  if (h2) delete h2; 

  return proj_info; 
}

void SLArQEventAnalysis::read_and_fill_axis(const TString& projection, std::vector<TVector3>* axis_vec, std::vector<int>* axis_idx, std::vector<TString>* axis_lbl) 
{
  auto strArray = projection.Tokenize(":");
  if (strArray->GetEntries() != 2) {
    printf("SLArQEventAnalysis::read_and_fill_axis(%s) ERROR: ", projection.Data());
    printf("Only two axes are allowed\n");
    return;
  }

  for (const auto &obj : *strArray)
  {
    TObjString *str = (TObjString *)obj;
    TString axis_str = str->GetString();

    //printf("axis_str: %s\n", axis_str.Data());

    if (axis_str == "x")
    {
      axis_vec->push_back(TVector3(1, 0, 0));
      axis_idx->push_back(0);
    }
    else if (axis_str == "y")
    {
      axis_vec->push_back(TVector3(0, 1, 0));
      axis_idx->push_back(1);
    }
    else if (axis_str == "z")
    {
      axis_vec->push_back(TVector3(0, 0, 1));
      axis_idx->push_back(2);
    }

    axis_lbl->push_back( axis_str ); 
  }

  return;
}

int SLArQEventAnalysis::scan_cluster_proj(
    const TH2* h2, 
    const int ix_vtx, 
    const int iy_vtx, 
    cluster_projection_info_t& proj_info) 
{
  
  // vertex region integration range [mm]
  const float range_x = 12; 
  const float range_y = 12;
  const float bin_w_x = h2->GetXaxis()->GetBinWidth(1); 
  const float bin_w_y = h2->GetYaxis()->GetBinWidth(1); 

  int N_y = std::floor(/*0.5* */range_y / bin_w_y ); 
  int N_x = std::floor(/*0.5* */range_x / bin_w_x ); 

  //printf("N_y = %i; N_x = %i\n", N_y, N_x);

  int non_void_bin_hzt_axis = 0; 
  for (int ix = 1; ix < h2->GetNbinsX() + 1; ix++)
  {
    bool same_ix = false;
    for (int iy = std::max(1, iy_vtx-N_y); 
         iy <= std::min(iy_vtx + N_y, h2->GetNbinsY()); iy++)
    {
      if (ix < ix_vtx)
      {
        if (h2->GetBinContent(ix, iy) > 0) {
          proj_info.fChargeX[0] += h2->GetBinContent(ix, iy);
          proj_info.fNHitsX[0] += 1;
          if (same_ix == false){
            proj_info.fLengthX[0] += bin_w_x;
            same_ix = true;
          }
          //printf("adding bin %i,%i to LEFT\n", ix, iy);
        }
      }
      else if (ix > ix_vtx)
      {
        if (h2->GetBinContent(ix, iy) > 0) {
          proj_info.fChargeX[1] += h2->GetBinContent(ix, iy);
          proj_info.fNHitsX [1] += 1;
          if (same_ix == false){
            proj_info.fLengthX[1] += bin_w_x;
            same_ix = true;
          }
          //printf("adding bin %i,%i to RIGHT\n", ix, iy);
        }
      }
    }
  }
  //printf("fLengthX: %f, %f \n", proj_info.fLengthX[0], proj_info.fLengthX[1]);

  for (int ix = std::max(1, ix_vtx-N_x);
       ix <= std::min(ix_vtx+N_x, h2->GetNbinsX()); ix++)
  {
    for (int iy = 1; iy <= h2->GetNbinsY(); iy++)
    {
      if (iy < iy_vtx)
      {
        if (h2->GetBinContent(ix, iy) > 0) {
          proj_info.fChargeY[0] += h2->GetBinContent(ix, iy);
          proj_info.fNHitsY[0] += 1;
          proj_info.fLengthY[0] += bin_w_y;
        }
      }
      else if (iy > iy_vtx)
      {
        if (h2->GetBinContent(ix, iy) > 0) {
          proj_info.fChargeY[1] += h2->GetBinContent(ix, iy);
          proj_info.fNHitsY[1]  += 1;
          proj_info.fLengthY[1] += bin_w_y;
        }
      }
    }
  }

  return non_void_bin_hzt_axis;
}

void SLArQEventAnalysis::init_line_pars(const TGraphErrors& g, const float* fit_range) {
  // get first and last point in the fit range and store the coordinates
  std::array<double, 2> dummy({-1e10, -1e10}); 
  std::array<double, 2> pt0(dummy); 
  std::array<double, 2> pt1(dummy);
  for (int ip=0; ip<g.GetN(); ip++) {
    if (g.GetX()[ip] > fit_range[0] && pt0 == dummy) {
      pt0.at(0) = g.GetX()[ip]; 
      pt0.at(1) = g.GetY()[ip];
    }
    else if (g.GetX()[ip] < fit_range[1]) {
      pt1.at(0) = g.GetX()[ip]; 
      pt1.at(1) = g.GetY()[ip];
    } 
  } 

  // now estimate par0 and par1 assuming a straight line between pt0 and pt1
  double par1 = (pt1[1] - pt0[1]) / (pt1[0] - pt0[0]); 
  double par0 = pt0[1] - par1*pt0[0]; 
  fLine->SetParameters(par0, par1); 

  return;
}

}
