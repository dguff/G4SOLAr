/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArElectronDrift.cc
 * @created     : giovedì nov 10, 2022 18:26:54 CET
 */

#include <functional>
#include "event/SLArEventChargeHit.hh"
#include "physics/SLArElectronDrift.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Poisson.hh"

SLArElectronDrift::SLArElectronDrift() :
  fElectricField(0.5), fLArTemperature(87.7), fMuElectron(1.), 
  fDiffCoefficientL(0.), fDiffCoefficientT(0.), 
  fvDrift(1.0), fElectronLifetime(1e6)
{}

void SLArElectronDrift::ComputeProperties() {
  printf("SLArElectronDrift::ComputeProperties() ");
  printf("Setup electron transport properties in LAr\n");

  fMuElectron = ComputeMobility(fElectricField, fLArTemperature);
  fvDrift     = ComputeDriftVelocity(fElectricField); 
  auto diff   = ComputeDiffusion(fElectricField, fLArTemperature); 
  fDiffCoefficientL = diff.at(0); 
  fDiffCoefficientT = diff.at(1); 
}

double SLArElectronDrift::ComputeMobility(std::array<double, 2> par) {
  double mu = ComputeMobility(par[0], par[1]); 
  return mu;
}

double SLArElectronDrift::ComputeMobility(double E, double larT) { 
  G4double num = a0 + a1*E + a2*pow(E, 1.5) + a3*pow(E, 2.5); 
  G4double den = 1 + (a1/a0)*E + a4*E*E + a5*E*E*E;

  G4double mu=  num/den*pow(larT/larT0, -1.5); 
  
  return mu;
}

double SLArElectronDrift::ComputeDriftVelocity(double E) {
  G4double v = E*fMuElectron*1e3/*kV/cm->V/cm*/*1E-8/*cm/s->mm/ns*/;
  return v;
}

std::array<double,2> SLArElectronDrift::ComputeDiffusion(double E, double larT) {
  G4double num = b0 + b1*E + b2*E*E; 
  G4double den = 1 + (b1/b0)*E + b3*E*E; 

  G4double epsL = (num/den)*(larT/larT1); 

  G4double diffL = epsL * fMuElectron * 1e-7; // [mm2/ns]

  std::array<double, 2> par = {E, larT}; 

  double dMudE = FastMuDerivative(par, 0.005*E, 0); 
  
  G4double diffT = diffL / (1+dMudE*E/fMuElectron); 

  return std::array<double,2>({diffL, diffT});
}

double SLArElectronDrift::FastMuDerivative(
    std::array<double, 2> par, double step, int ipar) {
  
  G4double num = 0; G4double den = 12*step; 
  std::array<double, 2> par_ = par; 
  par_[ipar] = par[ipar] + 2*step; num -=   ComputeMobility(par_); 
  par_[ipar] = par[ipar] + 1*step; num += 8*ComputeMobility(par_); 
  par_[ipar] = par[ipar] - 1*step; num -= 8*ComputeMobility(par_); 
  par_[ipar] = par[ipar] - 2*step; num +=   ComputeMobility(par_); 

  return num/den;
}

void SLArElectronDrift::PrintProperties() {
  printf("**************************************************\n");
  printf("* SLArElectronDrift Electron Transport Properties \n");
  printf("* - - - - - - - - - - - - - - - - - - - - - - - - \n");
  printf("* electron mobility: %g cm²/s/V\n", fMuElectron); 
  printf("* drift velocity: %g cm/μs\n", fvDrift * 1e+2);
  printf("* diff coeff L: %g cm²/s\n", fDiffCoefficientL*1e+7);
  printf("* diff coeff T: %g cm²/s\n", fDiffCoefficientT*1e+7);
  printf("**************************************************\n");
  return;
}

void SLArElectronDrift::Drift(const int& n, const int& trkId,
    const G4ThreeVector& pos, 
    SLArCfgSystemPix* pixCfg, 
    SLArEventReadoutTileSystem* pixEv) 
{
  // Find the megatile interested by the hit
  auto pixID = pixCfg->FindPixel(pos.z(), pos.y()); 
  //printf("%i electrons at [%.0f, %0.f, %0.f] mm\n", n, pos.x(), pos.y(), pos.z());
  //printf("pixID[%i, %i, %i]\n", pixID[0], pixID[1], pixID[2]);

  auto mtile = pixCfg->FindBaseElementInMap(pixID[0]); 
  if (mtile) {
    // Get anode position and compute drift time
    G4double tile_x_pos = mtile->GetX(); 
    G4double driftLength = std::fabs(pos.x() - tile_x_pos); 
    G4double driftTime   = driftLength / fvDrift;
    // compute diffusion length and fraction of surviving electrons
    G4double diffLengthT = sqrt(2*fDiffCoefficientT*driftTime); 
    G4double diffLengthL = sqrt(2*fDiffCoefficientL*driftTime); 
    G4double f_surv      = exp (-driftTime/fElectronLifetime); 

    //printf("Drift len = %g mm, time: %g ns, f_surv = %.2f%% - σ(L) = %g mm, σ(T) = %g mm\n", 
        //driftLength, driftTime, f_surv*100, diffLengthL, diffLengthT);

    G4int n_elec_anode = G4Poisson(n*f_surv); 

    std::vector<double> x_(n_elec_anode); 
    std::vector<double> y_(n_elec_anode); 
    std::vector<double> t_(n_elec_anode);
    G4RandGauss::shootArray(n_elec_anode, &x_[0], pos.z(), diffLengthT); 
    G4RandGauss::shootArray(n_elec_anode, &y_[0], pos.y(), diffLengthT); 
    G4RandGauss::shootArray(n_elec_anode, &t_[0], driftTime, diffLengthL/fvDrift); 

    for (G4int i=0; i<n_elec_anode; i++) {
      pixID = pixCfg->FindPixel(x_[i], y_[i]); 
      if (pixID[0] > 0 && pixID[1] > 0 && pixID[2] >0 ) {

        SLArCfgReadoutTile* tile = (SLArCfgReadoutTile*)mtile->FindBaseElementInMap(pixID[1]); 
        if (!tile) {
          printf("SLArElectronDrift::WARNING Unable to find tile with bin ID %i (%i, %i, %i)\n", 
              pixID[1], pixID[0], pixID[1], pixID[2]);
          getchar(); 
          continue;
        }
        SLArEventMegatile* evMT=pixEv->GetMegaTilesMap().find(mtile->GetIdx())->second;
        if (!evMT) {
          printf("SLArElectronDrift::WARNING No MT event registered at idx %i\n", 
              mtile->GetIdx());
          getchar(); 
          continue;
        }
        SLArEventTile* evT=evMT->GetTileMap().find(tile->GetIdx())->second;
        if (!evT) {
          printf("SLArElectronDrift::WARNING No Tile event registered at idx %i\n", 
              tile->GetIdx());
          getchar(); 
          continue;
        }
        evT->RegisterChargeHit(pixID[2], new SLArEventChargeHit(t_[i], trkId, 0)); 
        //if (i%50==0) {
        //printf("\tdiff x,y: %.2f - %.2f mm\n", x_[i], y_[i]);
        //printf("\tpix id: %i, %i, %i\n", pixID[0], pixID[1], pixID[2]);
      }
    }
  }

}
