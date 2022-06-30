/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArQDiffusion
 * @created     : martedì giu 28, 2022 09:11:33 CEST
 */

#include "SLArQDiffusion.hh"
#include <type_traits>

namespace slarq {

  SLArQDiffusion::SLArQDiffusion() 
  {} 

  SLArQDiffusion::~SLArQDiffusion()
  {}

  double SLArQDiffusion::Ldiffusion(double l) {
    double t = l / Vdrift; //μs
    float Ldiff = sqrt(2 * Dlong * t ); //mm
    return Ldiff;
  }

  double SLArQDiffusion::Tdiffusion(double l) {
    double t = l / Vdrift; //s
    float Tdiff = sqrt(2 * Dtrns * t ); //mm
    return Tdiff;
  }

  double SLArQDiffusion::DiffuseRandom(const trj_point* tpt, SLArQReadout* qev) {
    double xloc[3]; 
    xloc[0] = tpt->fX + xshift[0]; 
    xloc[1] = tpt->fY + xshift[1]; 
    xloc[2] = tpt->fZ + xshift[2]; 
    if (xloc[0]*xloc[1]*xloc[2] <=0) {
      std::cerr << "SLArQDiffusion::DiffuseRandom: " 
             << "WARNING trajectory point coordinate is negative!" << std::endl;
      std::cerr << "x: " << xloc[0] << " - y: " << xloc[1] << " - z: " << xloc[2] << std::endl;
      return 0.;
    }

    double t = (xloc[0]/ Vdrift); //μs           

    double q_step_true_exp = (tpt->fEdep / W);
    double q_step_true     = (int)gRandom->Poisson(q_step_true_exp); 
    double q_step_obs_exp = q_step_true * exp(-t / Elifetime);
    double q_step_obs = (int)gRandom->Poisson(q_step_obs_exp);

    double reco_weight = exp( t / Elifetime ); 

    double xx[3] = {0.}; 

    for (int iq = 0; iq<q_step_obs; iq++) {
      xx[0] = xloc[0] + gRandom->Gaus(0., Ldiffusion(xloc[0])); 
      xx[1] = xloc[1] + gRandom->Gaus(0., Tdiffusion(xloc[0])); 
      xx[2] = xloc[2] + gRandom->Gaus(0., Tdiffusion(xloc[0]));

      qev->Record( xx[0] / Vdrift, xx); 
    }

    //printf("tp [%.2f, %.2f, %.2f] : Edep = %g -> q_step_obs = %g\n", 
        //xloc[0], xloc[1], xloc[2], tpt->fEdep, q_step_obs); 

    return q_step_obs; 
  }

  double SLArQDiffusion::DiffuseAnalytical(const trj_point* tpt, SLArQReadout* qev) {
    double xloc[3]; 
    xloc[0] = tpt->fX + xshift[0]; 
    xloc[1] = tpt->fY + xshift[1]; 
    xloc[2] = tpt->fZ + xshift[2]; 
    if (xloc[0]*xloc[1]*xloc[2] <=0) return 0.; 

    //printf("point: (%.2f, %.2f, %.2f) mm\n", xloc[0], xloc[1], xloc[2]);
    // compute diffusion coefficients
    double sigma_dL = Ldiffusion(xloc[0]); 
    double sigma_dT = Tdiffusion(xloc[0]);

    double xmin[3]; double xmax[3]; 
    xmin[0] = xloc[0] - sigma_dL*4;  xmax[0] = xloc[0] + sigma_dL*4; 
    xmin[1] = xloc[1] - sigma_dT*4;  xmax[1] = xloc[1] + sigma_dT*4;
    xmin[2] = xloc[2] - sigma_dT*4;  xmax[2] = xloc[2] + sigma_dT*4;

    //for (int idim=0; idim<3; idim++) {
      //printf("dim %i: %.2f - %.2f\n", idim, xmin[idim], xmax[idim]); 
    //}


    double t = (xloc[0] / Vdrift); //μs           
    double q_step_true_exp = (tpt->fEdep / W);
    double q_step_true     = (int)gRandom->Poisson(q_step_true_exp); 
    double q_step_obs_exp  = q_step_true * exp(-t / Elifetime);

    // define difussion profiles
    std::vector<TF1> fGaus(3); 
    fGaus[0] = TF1("fgx", "gausn", xmin[0], xmax[0]); 
    fGaus[1] = TF1("fgy", "gausn", xmin[1], xmax[1]); 
    fGaus[2] = TF1("fgz", "gausn", xmin[2], xmax[2]); 

    for (int idim=0; idim<3; idim++) fGaus[idim].SetParameter(0, 1.); 
    fGaus[0].SetParameter(1, xloc[0]); fGaus[0].SetParameter(2, sigma_dL); 
    fGaus[1].SetParameter(1, xloc[1]); fGaus[1].SetParameter(2, sigma_dT); 
    fGaus[2].SetParameter(1, xloc[2]); fGaus[2].SetParameter(2, sigma_dT); 

    // define redout region of interest
    double bw[3]; // bin width for the three readout axes
    bw[0] = qev->GetQReadoutPitchAxis(kX); 
    bw[1] = qev->GetQReadoutPitchAxis(kY); 
    bw[2] = qev->GetQReadoutPitchAxis(kZ); 
    double bw_vol = bw[0]*bw[1]*bw[2]; 

    int ibin_min[3]; int ibin_max[3]; 
    for (int idim =0; idim <3; idim++) {
      ibin_min[idim] = floor(xmin[idim] / bw[idim]) +1; 
      ibin_max[idim] = floor(xmax[idim] / bw[idim]) +1; 
      //printf("dim %i: range %i -%i\n", idim, ibin_min[idim], ibin_max[idim]); 
    }

    double xint_low[3], xint_up[3], xx[3]; 
    double xint, yint, zint; 

    double q_step_obs = 0.; 

    for (int ix=ibin_min[0]; ix < ibin_max[0]; ix++) {
      xint_low[0] = qev->GetQHistN()->GetAxis(0)->GetBinLowEdge(ix); 
      xint_up [0] = qev->GetQHistN()->GetAxis(0)->GetBinLowEdge(ix+1); 
      xx[0] = 0.5*(xint_up[0] + xint_low[0]);
      xint = fGaus[0].Integral(xint_low[0], xint_up[0], 1e-6); 
      //printf("x_true = %.2f: σ = %g, integral(%.2f, %.2f) = %g\n", 
          //xloc[0], sigma_dL, xint_low[0], xint_up[0], xint); 
      for (int iy=ibin_min[1]; iy < ibin_max[1]; iy++) {
        xint_low[1] = qev->GetHist(kY)->GetXaxis()->GetBinLowEdge(iy); 
        xint_up [1] = qev->GetHist(kY)->GetXaxis()->GetBinLowEdge(iy+1); 
        xx[1] = 0.5*(xint_up[1] + xint_low[1]);
        yint = fGaus[1].Integral(xint_low[1], xint_up[1], 1e-3); 
        for (int iz=ibin_min[2]; iz < ibin_max[2]; iz++) {
          xint_low[2] = qev->GetHist(kZ)->GetXaxis()->GetBinLowEdge(iz); 
          xint_up [2] = qev->GetHist(kZ)->GetXaxis()->GetBinLowEdge(iz+1); 
          xx[2] = 0.5*(xint_up[2] + xint_low[2]);
          zint = fGaus[2].Integral(xint_low[2], xint_up[2], 1e-3); 
          double bc_exp = (xint*yint*zint) * q_step_obs_exp; 
          //printf("bc_exp = %g*%g*%g*[%g], q_step_obs_exp = %g\n", 
             //xint, yint, zint, bw_vol, q_step_obs_exp); 
          double w = gRandom->Poisson(bc_exp);
          q_step_obs += w; 

          //printf("Recording hit at (%.1f, %.1f, %.1f) mm - w = %g\n", 
              //xx[0], xx[1], xx[2], w); 
          qev->Record(xx[0] / Vdrift, xx, w); 
        }
      }
    }

    //getchar(); 
    return q_step_obs; 
  }

}
