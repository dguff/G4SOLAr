#include "SLArIonAndScintLArQL.h"
#include "G4MaterialPropertiesTable.hh"
#include <iostream>


SLArIonAndScintLArQL::SLArIonAndScintLArQL() : 
  SLArIonAndScintModel(), 
  fElectronIonRatio(0.29), fLArQLAlpha(0.0372), fLArQLBeta(0.0124), 
  fLArQLChiPars( {0.00338427, -6.57037, 1.88418, 0.000129379} )
{}

SLArIonAndScintLArQL::SLArIonAndScintLArQL(const G4MaterialPropertiesTable* mpt) : 
  SLArIonAndScintModel(mpt), 
  fElectronIonRatio( mpt->GetConstProperty("LARQL_ELECTRONIONRATIO") ), 
  fLArQLAlpha( mpt->GetConstProperty("LARQL_ALPHA") ), 
  fLArQLBeta( mpt->GetConstProperty("LARQL_BETA") ), 
  fLArQLChiPars( {
      mpt->GetConstProperty("LARQL_CHIPAR0"), 
      mpt->GetConstProperty("LARQL_CHIPAR1"), 
      mpt->GetConstProperty("LARQL_CHIPAR2"), 
      mpt->GetConstProperty("LARQL_CHIPAR3") } )
{}
  
SLArIonAndScintLArQL::~SLArIonAndScintLArQL()
{
  // Default destructor
}


double SLArIonAndScintLArQL::QBirks(double dE, double dx, double E) const {
    double dEdx = dE/dx;
    if(dEdx<0.1) dEdx = 0.1;
    double nom = fBirksA/fWion;
    double denom = 1+ fBirksK*dEdx /(fLArDensity * E);
    return nom/denom;
}

double SLArIonAndScintLArQL::Corr(double dE, double dx, double E) const {
    double dEdx = dE/dx;
    if(dEdx<1) dEdx = 1.;
    return exp(-E/(fLArQLAlpha *log(dEdx) + fLArQLBeta));
}

double SLArIonAndScintLArQL::QChi(double dE, double dx, double E) const {
    double dEdx = dE/dx;
    if(dEdx<1) dEdx = 1.;
    return fLArQLChiPars[0] / 
      (fLArQLChiPars[1]+exp(fLArQLChiPars[2] + fLArQLChiPars[3]*dEdx));
}

double SLArIonAndScintLArQL::Qinf() const {
    return fIonizationDensity;
}

double SLArIonAndScintLArQL::ComputeScintYield(const double energy_deposit, const double hit_distance, const double electric_field) const {
    return fElectronIonRatio * fIonizationDensity + 
      fIonizationDensity - ComputeIonYield(energy_deposit,hit_distance, electric_field);
}

double SLArIonAndScintLArQL::ComputeIonYield(const double energy_deposit, const double hit_distance, const double electric_field) const {
  double QB = QBirks(energy_deposit, hit_distance, electric_field); 
  double QX = Corr(energy_deposit, hit_distance, electric_field)*
      QChi(energy_deposit, hit_distance, electric_field)*Qinf();
  return (QB + QX); 
}

double SLArIonAndScintLArQL::Flat() const {
    return FlatLightYieldPerMeV;
}
