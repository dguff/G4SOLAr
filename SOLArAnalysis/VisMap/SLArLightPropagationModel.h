/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArLightPropagationModel.h
 * @created     : lunedì ott 17, 2022 18:00:41 CEST
 *
 * @brief       : class containing functions for calculating number 
 *                of hits on each optical channel via the solid angle method
 *                using gaisser-hillas corrections for Rayleigh scattering
 * 
 * Reimplemented from CLSim (Till Dieminger), DUNE simulation framework,
 * based on 
 *
 * D. Garcia-Gamez, P. Green, A.M. Szelc, 
 * "Predicting Transport Effects of Scintillation Light Signals in 
 * Large-Scale Liquid Argon Detectors", 
 * Published in: Eur.Phys.J.C 81 (2021) 4, 349 • e-Print: 2010.00324 [physics.ins-det]
 */

#ifndef SLARLIGHTPROPAGATIONMODEL_H

#define SLARLIGHTPROPAGATIONMODEL_H



#include <vector>
#include <string>

#include "TF1.h"
#include "TVector3.h"


#include "config/SLArCfgBaseSystem.hh"
#include "config/SLArCfgMegaTile.hh"
#include "config/SLArCfgReadoutTile.hh"
#include "config/SLArCfgSuperCell.hh"

typedef SLArCfgBaseSystem<SLArCfgMegaTile> SLArPixCfg;

namespace slarAna {
  class SLArLightPropagationModel {

    private:
      // useful constants
      const double pi = 3.141592653589793;

      // Parameters for fCorr function - fit to plot from LArQL Paper
      std::vector<double> fcorr_param = {0.032, 0.008258}; 

      // DUNE-SP Gaisser-Hillas angle bins
      std::vector<double> angulo = {0, 10, 20, 30, 40, 50, 60, 70, 80};
      const double delta_angle = 10.;

      bool _mathmore_loaded_ = false;

    public:
      // constructor
      SLArLightPropagationModel();

      // destructor
      ~SLArLightPropagationModel(){};


      double VisibilityOpDetTile(SLArCfgReadoutTile* cfgTile, const TVector3 &ScintPoint); 

      // gaisser-hillas function
      static Double_t GaisserHillas(double x, double *par);

      // solid angle of rectangular aperture calculation functions
      double omega(const double &a, const double &b, const double &d) const;
      double solid(SLArCfgReadoutTile* cfgTile, TVector3 &v); 

      // solid angle of circular aperture calculation functions
      double Disk_SolidAngle(double *x, double *p);
      double Disk_SolidAngle(double d, double h, double b);

      // solid angle of dome (PMTs)
      double Omega_Dome_Model(const double distance, const double theta) const;

      // linear interpolation function
      double interpolate( const std::vector<double> &xData, const std::vector<double> &yData, double x, bool extrapolate );

  };
}
#endif /* end of include guard SLARLIGHTPROPAGATIONMODEL_H */
