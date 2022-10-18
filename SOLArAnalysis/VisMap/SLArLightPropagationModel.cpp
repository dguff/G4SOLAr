#include "SLArLightPropagationModel.h"
#include "SLArLightPropagationPars.h"

#include <iostream>
#include <fstream>
#include <cmath>

#include "TRandom.h"
#include "TSystem.h"
#include "TMath.h"
#include "TFormula.h"
#include "Math/SpecFuncMathMore.h"

#include "G4UIcommand.hh"

using namespace std;
bool debug_2 = false;

namespace slarAna {
  // constructor
  SLArLightPropagationModel::SLArLightPropagationModel() {

    // load mathmore library
    gSystem->Load("libMathMore.so");
    if(gSystem->Load("libMathMore.so") < 0) {
      throw(std::runtime_error("Unable to load MathMore library"));
    }
    _mathmore_loaded_ = true;

    std::cout << "Light simulation for DUNE Single Phase detector." << std::endl;
    std::cout << std::endl;
  }

  double SLArLightPropagationModel::VisibilityOpDetTile(
      SLArCfgReadoutTile* cfgTile, 
      const TVector3 &ScintPoint) 
  {
    TVector3 OpDetPoint(
        cfgTile->GetPhysX()/G4UIcommand::ValueOf("cm"), 
        cfgTile->GetPhysY()/G4UIcommand::ValueOf("cm"), 
        cfgTile->GetPhysZ()/G4UIcommand::ValueOf("cm"));

    TVector3 ScintPoint_rel = ScintPoint - OpDetPoint; 


    double costheta   = 0.; 
    double theta      = 0.; 
    double distance   = (ScintPoint_rel).Mag();

    double r_distance = -1; 
    double solid_angle= -1; 

    //TODO: Add Optical detector normal to cfg object
    TVector3 OpDetNorm(-1, 0, 0); 

    costheta   = (ScintPoint_rel).Unit().Dot(OpDetNorm); 
    theta = acos(costheta)*TMath::RadToDeg();
    r_distance = (
        ScintPoint - 
        OpDetNorm*(OpDetNorm.Dot(ScintPoint - TVector3(0, 0, 0)))
        ).Mag();

    //printf("scint pos: (%g, %g, %g) cm - tile pos: (%g, %g, %g):\n\td = %g cm - cosθ = %g - θ = %g deg\n", 
        //ScintPoint.x(), ScintPoint.y(), ScintPoint.z(), 
        //OpDetPoint.x(), OpDetPoint.y(), OpDetPoint.z(), 
        //distance, costheta, theta);


    if (costheta < 0.001)
      solid_angle = 0;
    else
      solid_angle = solid(cfgTile, ScintPoint_rel);

    // calculate solid angle
    if(solid_angle < 0){
      std::cout << "Error: solid angle is negative" << std::endl;
      exit(1);
    }
    if(r_distance < 0){
      std::cout << "Error: r_distance is negative" << std::endl;
      exit(1);
    }
    
    //printf("\tsolid_angle = %g\n", solid_angle);


    double vis_geo = exp(-1.*distance/L_abs) * (solid_angle / (4*pi));

    //printf("\tvis_geo = %g\n", vis_geo);
    //getchar(); 

    // determine Gaisser-Hillas correction for Rayleigh scattering, 
    // distance and angular dependence, accounting for border effects
    // offset angle bin
    if(theta>89.0){
      return vis_geo;
    }
    int j = (theta/delta_angle);

    // identify GH parameters and border corrections by optical detector type 
    // and scintillation type
    double pars_ini[4] = {0,0,0,0};
    double s1, s2, s3;
    int scintillation_type = 0;  // TODO: Fix for argon
    if (scintillation_type == 0) { // argon
      if (j >= 8){
        pars_ini[0] = fGHVUVPars_flat_argon[0][j];
        pars_ini[1] = fGHVUVPars_flat_argon[1][j];
        pars_ini[2] = fGHVUVPars_flat_argon[2][j];
        pars_ini[3] = fGHVUVPars_flat_argon[3][j];
      }
      else{
        double temp1 = fGHVUVPars_flat_argon[0][j];
        double temp2 = fGHVUVPars_flat_argon[0][j+1];
        pars_ini[0] = temp1 + (temp2-temp1)*(theta-j*delta_angle)/delta_angle;

        temp1 = fGHVUVPars_flat_argon[1][j];
        temp2 = fGHVUVPars_flat_argon[1][j+1];
        pars_ini[1] = temp1 + (temp2-temp1)*(theta-j*delta_angle)/delta_angle;

        temp1 = fGHVUVPars_flat_argon[2][j];
        temp2 = fGHVUVPars_flat_argon[2][j+1];
        pars_ini[2] = temp1 + (temp2-temp1)*(theta-j*delta_angle)/delta_angle;

        temp1 = fGHVUVPars_flat_argon[3][j];
        temp2 = fGHVUVPars_flat_argon[3][j+1];
        pars_ini[3] = temp1 + (temp2-temp1)*(theta-j*delta_angle)/delta_angle;
      }

      s1 = interpolate( angulo, slopes1_flat_argon, theta, true);
      s2 = interpolate( angulo, slopes2_flat_argon, theta, true);
      s3 = interpolate( angulo, slopes3_flat_argon, theta, true);
    }
    else if (scintillation_type == 1) { // xenon
      pars_ini[0] = fGHVUVPars_flat_xenon[0][j];
      pars_ini[1] = fGHVUVPars_flat_xenon[1][j];
      pars_ini[2] = fGHVUVPars_flat_xenon[2][j];
      pars_ini[3] = fGHVUVPars_flat_xenon[3][j];
      s1 = interpolate( angulo, slopes1_flat_xenon, theta, true);
      s2 = interpolate( angulo, slopes2_flat_xenon, theta, true);
      s3 = interpolate( angulo, slopes3_flat_xenon, theta, true);
    }
    else {
      std::cout << "Error: Invalid scintillation type configuration." << endl;
      exit(1);
    }
    // add border correction
    pars_ini[0] = pars_ini[0] + s1 * r_distance;
    pars_ini[1] = pars_ini[1] + s2 * r_distance;
    pars_ini[2] = pars_ini[2] + s3 * r_distance;
    pars_ini[3] = pars_ini[3];

    // calculate correction factor
    double GH_correction = GaisserHillas(distance, pars_ini);

    // apply correction
    double vis_vuv = 0 ;
    vis_vuv = GH_correction*vis_geo/costheta;

    //printf("\tGH_correction = %g\n", GH_correction);
    //printf("\tvis_vuv = %g\n", vis_vuv);
    //getchar(); 

    return vis_vuv;
  }

  // gaisser-hillas function definition
  Double_t SLArLightPropagationModel::GaisserHillas(double x,double *par) {
    //This is the Gaisser-Hillas function
    Double_t X_mu_0=par[3];
    Double_t Normalization=par[0];
    Double_t Diff=par[1]-X_mu_0;
    Double_t Term=pow((x-X_mu_0)/Diff,Diff/par[2]);
    Double_t Exponential=TMath::Exp((par[1]-x)/par[2]);

    return ( Normalization*Term*Exponential);
  }


  // solid angle of rectanglular aperture calculation functions

  double SLArLightPropagationModel::omega(const double &a, const double &b, const double &d) const{

    double aa = a/(2.0*d);
    double bb = b/(2.0*d);
    double aux = (1+aa*aa+bb*bb)/((1.+aa*aa)*(1.+bb*bb));
    return 4*std::acos(std::sqrt(aux));

  }

  double SLArLightPropagationModel::solid(SLArCfgReadoutTile* cfgTile, TVector3 &v) {
    //TODO: add normal to photon detector in cfg 
    TVector3 OpDetNorm(-1, 0, 0);     

    // TODO: Fix to make it independent of detector plane
    // detector size
    double Dx_ = cfgTile->Get2DSize_X()/G4UIcommand::ValueOf("cm"); 
    double Dy_ = cfgTile->Get2DSize_Y()/G4UIcommand::ValueOf("cm");

    // project scintillation point onto the photon detector plane
    TVector3 vv = v - OpDetNorm.Dot(v)*OpDetNorm; 

    // The hit is directly above the SiPM
    if( vv.Y()==0.0 && vv.Z()==0.0){
      return omega(Dx_,Dy_,v.Mag());
    }

    if( (std::fabs(v.Y()) > Dx_*0.5) && (std::fabs(v.Z()) > Dy_*0.5)){
      double A, B, a, b, d;
      A = std::fabs(v.Y())-Dx_*0.5;
      B = std::fabs(v.Z())-Dy_*0.5;
      a = Dx_;
      b = Dy_;
      d = fabs(v.X());
      double to_return = (
           omega(2*(A+a),2*(B+b),d) 
          -omega(2*A,2*(B+b),d)
          -omega(2*(A+a),2*B,d)
          +omega(2*A,2*B,d))*0.25;
      return to_return;
    }

    if( (std::fabs(v.Y()) <= Dx_*0.5) && (std::fabs(v.Z()) <= Dy_*0.5)){
      double A, B, a, b, d;
      A = -std::abs(v.Y())+Dx_*0.5;
      B = -std::abs(v.Z())+Dy_*0.5;
      a = Dx_;
      b = Dy_;
      d = fabs(v.X());
      double to_return = (
            omega(2*(a-A),2*(b-B),d)
           +omega(2*A,2*(b-B),d)
           +omega(2*(a-A),2*B,d)
           +omega(2*A,2*B,d))*0.25;
      return to_return;
    }

    if( (std::fabs(v.Y()) > Dx_*0.5) && (std::fabs(v.Z()) <= Dy_*0.5)){
      double A, B, a, b, d;
      A =  std::fabs(v.Y())-Dx_*0.5;
      B = -std::fabs(v.Z())+Dy_*0.5;
      a = Dx_;
      b = Dy_;
      d = fabs(v.X());
      double to_return = (
          omega(2*(A+a),2*(b-B),d)
          -omega(2*A,2*(b-B),d)
          +omega(2*(A+a),2*B,d)
          -omega(2*A,2*B,d))*0.25;
      return to_return;
    }

    if( (std::fabs(v.Y()) <= 0.5*Dx_) && (std::fabs(v.Z()) > 0.5*Dy_)){
      double A, B, a, b, d;
      A = -std::fabs(v.Y())+0.5*Dx_;
      B =  std::fabs(v.Z())-0.5*Dy_;
      a = Dx_;
      b = Dy_;
      d = fabs(v.X());
      double to_return = (
          omega(2*(a-A),2*(B+b),d)
          -omega(2*(a-A),2*B,d)
          +omega(2*A,2*(B+b),d)
          -omega(2*A,2*B,d))*0.25;
      return to_return;
    }
    // error message if none of these cases, i.e. something has gone wrong!
    std::cout << "Warning: invalid solid angle call." << std::endl;
    return 0.0;
  }


  // solid angle of circular aperture
  double SLArLightPropagationModel::Disk_SolidAngle(double *x, double *p) {
    const double d = x[0];
    const double h = x[1];
    const double b = p[0];
    if(b <= 0. || d < 0. || h <= 0.) return 0.;
    const double aa = TMath::Sqrt(h*h/(h*h+(b+d)*(b+d)));
    if(d == 0) {
      return 2.*TMath::Pi()*(1.-aa);
    }
    const double bb = TMath::Sqrt(4*b*d/(h*h+(b+d)*(b+d)));
    const double cc = 4*b*d/((b+d)*(b+d));

    if(!_mathmore_loaded_) {
      if(gSystem->Load("libMathMore.so") < 0) {
        throw(std::runtime_error("Unable to load MathMore library"));
      }
      _mathmore_loaded_ = true;
    }
    if(TMath::Abs(ROOT::Math::comp_ellint_1(bb) - bb) < 1e-10 && TMath::Abs(ROOT::Math::comp_ellint_3(cc,bb) - cc) <1e-10) {
      throw(std::runtime_error("please do gSystem->Load(\"libMathMore.so\") before running Disk_SolidAngle for the first time!"));
    }
    if(d < b) {
      return 2.*TMath::Pi() - 2.*aa*(ROOT::Math::comp_ellint_1(bb) + TMath::Sqrt(1.-cc)*ROOT::Math::comp_ellint_3(cc,bb));
    }
    if(d == b) {
      return TMath::Pi() - 2.*aa*ROOT::Math::comp_ellint_1(bb);
    }
    if(d > b) {
      return 2.*aa*(TMath::Sqrt(1.-cc)*ROOT::Math::comp_ellint_3(cc,bb) - ROOT::Math::comp_ellint_1(bb));
    }

    return 0.;
  }

  double SLArLightPropagationModel::Disk_SolidAngle(double d, double h, double b) {
    double x[2] = { d, h };
    double p[1] = { b };
    if(!_mathmore_loaded_) {
      if(gSystem->Load("libMathMore.so") < 0) {
        throw(std::runtime_error("Unable to load MathMore library"));
      }
      _mathmore_loaded_ = true;
    }
    return Disk_SolidAngle(x,p);
  }

  double SLArLightPropagationModel::Omega_Dome_Model(const double distance, const double theta) const {
    // this function calculates the solid angle of a semi-sphere of radius b,
    // as a correction to the analytic formula of the on-axix solid angle,
    // as we move off-axis an angle theta. We have used 9-angular bins
    // with delta_theta width.

    // par0 = Radius correction close
    // par1 = Radius correction far
    // par2 = breaking distance betwween "close" and "far"

    double par0[9] = {0., 0., 0., 0., 0., 0.597542, 1.00872, 1.46993, 2.04221};
    double par1[9] = {0, 0, 0.19569, 0.300449, 0.555598, 0.854939, 1.39166, 2.19141, 2.57732};
    const double delta_theta = 10.;
    int j = int(theta/delta_theta);
    // 8" PMT radius
    const double b = 8*2.54/2.;
    // distance form which the model parameters break (empirical value)
    const double d_break = 5*b;//par2

    if(distance >= d_break) {
      double R_apparent_far = b - par1[j];
      return  (2*3.1416 * (1 - sqrt(1 - pow(R_apparent_far/distance,2))));

    }
    else {
      double R_apparent_close = b - par0[j];
      return (2*3.1416 * (1 - sqrt(1 - pow(R_apparent_close/distance,2))));
    }
  }

  double SLArLightPropagationModel::interpolate( 
      const std::vector<double> &xData, 
      const std::vector<double> &yData, 
      double x, bool extrapolate ) 
  {
    int size = xData.size();
    int i = 0;                  // find left end of interval for interpolation
    if ( x >= xData[size - 2] ) // special case: beyond right end
    {
      i = size - 2;
    }
    else
    {
      while ( x > xData[i+1] ) i++;
    }
    double xL = xData[i];
    double yL = yData[i];
    double xR = xData[i+1]; 
    double yR = yData[i+1]; // points on either side (unless beyond ends)
    if ( !extrapolate )     // if beyond ends of array and not extrapolating
    {
      if ( x < xL ) yR = yL;
      if ( x > xR ) yL = yR;
    }
    double dydx = ( yR - yL ) / ( xR - xL );            // gradient
    return yL + dydx * ( x - xL );                      // linear interpolation
  }
}
