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

SLArElectronDrift::SLArElectronDrift(const SLArLArProperties& lar_properties) : fLArProperties(lar_properties)
{ }

void SLArElectronDrift::Drift(const int& n, 
    const int& trkId,
    const int& ancestorId,
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

  //#ifdef SLAR_DEBUG
  //printf("%i electrons at [%.0f, %0.f, %0.f] mm, t = %g ns\n", 
  //n, pos.x(), pos.y(), pos.z(), time);
  //printf("axis projection: [%.0f, %.0f]\n", pos.dot(anodeXaxis), pos.dot(anodeYaxis)); 
  //printf("pixID[%i, %i, %i]\n", pixID_tmp[0], pixID_tmp[1], pixID_tmp[2]);
  //#endif

  // Get anode position and compute drift time
  G4double driftLength = (pos - anodePos).dot(anodeNormal);
  if (driftLength < 0) return;

  G4double driftTime   = driftLength / fLArProperties.fvDrift;
  G4double hitTime     = time + driftTime; 
  // compute diffusion length and fraction of surviving electrons
  G4double diffLengthT = sqrt(2*fLArProperties.fDiffCoefficientT*driftTime); 
  G4double diffLengthL = sqrt(2*fLArProperties.fDiffCoefficientL*driftTime); 
  G4double f_surv      = exp (-driftTime/fLArProperties.fElectronLifetime); 

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
  G4RandGauss::shootArray(n_elec_anode, &t_[0], hitTime, diffLengthL / fLArProperties.fvDrift); 

  SLArCfgAnode::SLArPixIdx pixID;
  for (G4int i=0; i<n_elec_anode; i++) {
    pixID = anodeCfg->GetPixelIndex(x_[i], y_[i]); 
    if (pixID[0] >= 0 && pixID[1] >= 0 && pixID[2] >= 0 ) {

      SLArEventChargeHit hit(t_[i], trkId, ancestorId); 
      auto& evPixel = anodeEv->RegisterChargeHit(pixID, hit); 

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

