/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArElectronDrift.cc
 * @created     Thur Nov 10, 2022 18:26:54 CET
 */

#include <functional>
#include "SLArAnalysisManager.hh"
#include "SLArBacktrackerManager.hh"
#include "event/SLArEventAnode.hh"
#include "event/SLArEventChargeHit.hh"
#include "config/SLArCfgAnode.hh"

#include "physics/SLArElectronDrift.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Poisson.hh"

SLArElectronDrift::SLArElectronDrift() :
  fElectricField(0.5), fLArTemperature(87.7), fMuElectron(1.), 
  fDiffCoefficientL(0.), fDiffCoefficientT(0.), 
  fvDrift(1.0), fElectronLifetime(1e7)
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
    const double time, 
    SLArCfgAnode* anodeCfg, 
    SLArEventAnode* anodeEv) 
{
  auto ana_mngr = SLArAnalysisManager::Instance();
  auto bkt_mngr = ana_mngr->GetBacktrackerManager( backtracker:: kCharge );

  // Find the megatile interested by the hit
  G4ThreeVector anodeXaxis = 
    G4ThreeVector(anodeCfg->GetAxis0().x(), anodeCfg->GetAxis0().y(), anodeCfg->GetAxis0().z());
  G4ThreeVector anodeYaxis = 
    G4ThreeVector(anodeCfg->GetAxis1().x(), anodeCfg->GetAxis1().y(), anodeCfg->GetAxis1().z());
  G4ThreeVector anodeNormal= 
    G4ThreeVector(anodeCfg->GetNormal().x(), anodeCfg->GetNormal().y(), anodeCfg->GetNormal().z());
  G4ThreeVector anodePos = 
    G4ThreeVector(anodeCfg->GetPhysX(), anodeCfg->GetPhysY(), anodeCfg->GetPhysZ()); 

  //auto pixID_tmp = anodeCfg->GetPixelCoord( pos.dot(anodeXaxis), pos.dot(anodeYaxis) ); 
  //#ifdef SLAR_DEBUG
  //printf("%i electrons at [%.0f, %0.f, %0.f] mm, t = %g ns\n", 
  //n, pos.x(), pos.y(), pos.z(), time);
  //printf("axis projection: [%.0f, %.0f]\n", pos.dot(anodeXaxis), pos.dot(anodeYaxis)); 
  //printf("pixID[%i, %i, %i]\n", pixID_tmp[0], pixID_tmp[1], pixID_tmp[2]);
  //#endif

  // Get anode position and compute drift time
  G4double driftLength = (pos - anodePos).dot(anodeNormal);
  if (driftLength < 0) return;

  G4double driftTime   = driftLength / fvDrift;
  G4double hitTime     = time + driftTime; 
  // compute diffusion length and fraction of surviving electrons
  G4double diffLengthT = sqrt(2*fDiffCoefficientT*driftTime); 
  G4double diffLengthL = sqrt(2*fDiffCoefficientL*driftTime); 
  G4double f_surv      = exp (-driftTime/fElectronLifetime); 

  //#ifdef SLAR_DEBUG
  //printf("Drift len = %g mm, time: %g ns, f_surv = %.2f%% - σ(L) = %g mm, σ(T) = %g mm\n", 
  //driftLength, driftTime, f_surv*100, diffLengthL, diffLengthT);
  //getchar(); 
  //#endif

  G4int n_elec_anode = G4Poisson(n*f_surv); 

  std::vector<double> x_(n_elec_anode); 
  std::vector<double> y_(n_elec_anode); 
  std::vector<double> t_(n_elec_anode);
  G4RandGauss::shootArray(n_elec_anode, &x_[0], pos.dot(anodeXaxis), diffLengthT); 
  G4RandGauss::shootArray(n_elec_anode, &y_[0], pos.dot(anodeYaxis), diffLengthT); 
  G4RandGauss::shootArray(n_elec_anode, &t_[0], hitTime, diffLengthL/fvDrift); 

  SLArCfgAnode::SLArPixIdxCoord pixID;
  for (G4int i=0; i<n_elec_anode; i++) {
    pixID = anodeCfg->GetPixelCoord(x_[i], y_[i]); 
    if (pixID[0] > 0 && pixID[1] > 0 && pixID[2] > 0 ) {
      SLArCfgMegaTile& mtile = anodeCfg->GetBaseElementByID(pixID[0]); 
      SLArCfgReadoutTile& tile = mtile.GetBaseElementByID(pixID[1]); 
      //if (!tile) {
        //printf("SLArElectronDrift::WARNING Unable to find tile with bin ID %i (%i, %i, %i)\n", 
            //pixID[1], pixID[0], pixID[1], pixID[2]);
        //getchar(); 
        //continue;
      //}

      SLArEventChargeHit hit(t_[i], trkId, 0); 
      auto& evPixel = anodeEv->RegisterChargeHit(pixID, hit); 

      //SLArEventMegatile* evMT=nullptr;
      //auto mt_itr = anodeEv->GetMegaTilesMap().find(mtile->GetIdx());
      //if (mt_itr == anodeEv->GetMegaTilesMap().end()) {
        //evMT = anodeEv->CreateEventMegatile(mtile->GetIdx()); 
      //}
      //else {
        //evMT = mt_itr->second;
      //}

      //SLArEventTile* evT=nullptr;
      //auto t_itr = evMT->GetTileMap().find(tile->GetIdx()); 
      //if (t_itr == evMT->GetTileMap().end()) {
        //evT = evMT->CreateEventTile(tile->GetIdx()); 
      //}
      //else {
        //evT = t_itr->second;
      //}
      //auto ev_pixel = evT->RegisterChargeHit(pixID[2], hit); 

      //#ifdef SLAR_DEBUG
      //printf("\tdiff x,y: %.2f - %.2f mm\n", x_[i], y_[i]);
      //printf("\tpix id: %i, %i, %i\n", pixID[0], pixID[1], pixID[2]);
      ////evT->PrintHits(); 
      //getchar();
      //#endif

      if (bkt_mngr == nullptr) continue;

      if (bkt_mngr->IsNull()) continue;

      auto& records = 
        evPixel.GetBacktrackerVector( evPixel.ConvertToClock<float>(hit.GetTime()));

      for (size_t ib = 0; ib < bkt_mngr->GetBacktrackers().size(); ib++) {
        bkt_mngr->GetBacktrackers().at(ib)->Eval(&hit, 
            &records.GetRecords().at(ib));
      }
    }
  }
}

