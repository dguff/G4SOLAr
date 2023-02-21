#include "SLArLightYield.h"
#include <iostream>


SLArLightYield::SLArLightYield()
{
  // Default constructor
}
SLArLightYield::~SLArLightYield()
{
  // Default destructor
}


double SLArLightYield::QBirks(double dE, double dx, double E){
    double dEdx = dE/dx;
    if(dEdx<0.1) dEdx = 0.1;
    double nom = A_Birks/Wion;
    double denom = 1+ k_Birks*dEdx /(pLAr * E);
    return nom/denom*1.0e6;
}

double SLArLightYield::Corr(double dE, double dx, double E){
    double dEdx = dE/dx;
    if(dEdx<1) dEdx = 1.;
    return exp(-E/(alpha *log(dEdx) + beta));
}

double SLArLightYield::QChi(double dE, double dx, double E){
    double dEdx = dE/dx;
    if(dEdx<1) dEdx = 1.;
    return chi_param[0]/(chi_param[1]+exp(chi_param[2] + chi_param[3]*dEdx));
}

double SLArLightYield::Qinf(){
    return 1.0e6/Wion;
}

double SLArLightYield::LArQL(const double energy_deposit, const double hit_distance, const double electric_field){
    return Nex_Ni * Ni + Ni - LArQQ(energy_deposit,hit_distance, electric_field);
}

double SLArLightYield::LArQQ(const double energy_deposit, const double hit_distance, const double electric_field){
  double QB = QBirks(energy_deposit, hit_distance, electric_field); 
  double QX = Corr(energy_deposit, hit_distance, electric_field)*
      QChi(energy_deposit, hit_distance, electric_field)*Qinf();
  return (QB + QX); 
}

double SLArLightYield::Flat(){
    return FlatLightYieldPerMeV;
}
