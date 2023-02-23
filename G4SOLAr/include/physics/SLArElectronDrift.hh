/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArElectronDrift.hh
 * @created     : giovedì nov 10, 2022 17:42:44 CET
 */

#ifndef SLARELECTRONDRIFT_HH

#define SLARELECTRONDRIFT_HH

#include <math.h>
#include <vector>
#include <functional>
#include "G4ThreeVector.hh"

class SLArEventReadoutTileSystem; 
class SLArCfgSystemPix;

class SLArElectronDrift {
  public:
    SLArElectronDrift(); 
    ~SLArElectronDrift() {} 

    void SetElectricField(double E); 
    void ComputeProperties(); 

    void Drift(const int& n, const int& trkId, const G4ThreeVector& pos, 
        const double time, 
        SLArCfgSystemPix* pixCfg, SLArEventReadoutTileSystem* pixEv);

    void PrintProperties(); 

  private: 
    double fElectricField;       //!< TPC Electric Field
    double fLArTemperature;      //!< Liquid Argon Temperature
    double fMuElectron;          //!< Electron mobility
    double fDiffCoefficientL;    //!< Longitudinal Diffusion Coefficient
    double fDiffCoefficientT;    //!< Transverse Diffusion Coefficient
    double fvDrift;              //!< Electron drift velocity
    double fElectronLifetime;    //!< Electron lifetime 

    double ComputeMobility(double E, double larT);
    double ComputeMobility(std::array<double, 2> par); 
    std::array<double,2>   ComputeDiffusion(double E, double larT); 
    double ComputeDriftVelocity(double E); 
    double FastMuDerivative(std::array<double, 2>, double, int); 

    const double a0 = 551.6; 
    const double a1 = 7158.3;
    const double a2 = 4440.43;
    const double a3 = 4.29;
    const double a4 = 43.63;
    const double a5 = 0.2053; 
    const double larT0 = 89.0; 
    const double larT1 = 87.0; 
    const double b0 = 0.0075; 
    const double b1 = 742.9; 
    const double b2 = 3269.6; 
    const double b3 = 31687.2;

};


#endif /* end of include guard SLARELECDRIFT_HH */

