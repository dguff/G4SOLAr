/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArIonAndScintSeparate.h
 * @created     Wed Mar 01, 2023 10:39:57 CET
 */

#ifndef SLARIONANDSCINTSEPARATE_H

#define SLARIONANDSCINTSEPARATE_H

#include "physics/SLArIonAndScintModel.h"

class SLArIonAndScintSeparate : public SLArIonAndScintModel {

  private: 
    double fModBoxA; 
    double fModBoxB; 
    bool   fUseModBoxRecomb; 
    double fLightYield;

  public: 
    SLArIonAndScintSeparate(); 
    SLArIonAndScintSeparate(const G4MaterialPropertiesTable* mpt);
    ~SLArIonAndScintSeparate() {}; 

    double ComputeIonYield(const double energy_deposit, const double step_length, const double electric_field) const override; 
    double ComputeScintYield(const double energy_deposit, const double step_length, const double electric_field) const override; 
    double ComputeIon(const double energy_deposit, const double step_length, const double electric_field) const override; 
    double ComputeScint(const double energy_deposit, const double step_length, const double electric_field) const override; 
    void SetLightYield(const double ly) {fLightYield = ly;}
}; 



#endif /* end of include guard SLARIONANDSCINTSEPARATE_H */

