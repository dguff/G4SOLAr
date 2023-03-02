/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArIonAndScintModel.h
 * @created     Wed Mar 01, 2023 09:53:04 CET
 */

#ifndef SLARIONANDSCINTMODEL_H

#define SLARIONANDSCINTMODEL_H

#include <iostream>
#include <vector>

class G4MaterialPropertiesTable;

class SLArIonAndScintModel {
  public: 
    enum EISModel {kSeparate = 0, kLArQL = 1};
    SLArIonAndScintModel();
    SLArIonAndScintModel(const G4MaterialPropertiesTable* mpt); 
    ~SLArIonAndScintModel() {}; 

    virtual double ComputeIonYield(const double energy_deposit, const double step_length, const double electric_field) const = 0; 
    virtual double ComputeScintYield(const double energy_deposit, const double step_length, const double electric_field) const = 0;
    virtual double ComputeIon(const double energy_deposit, const double step_length, const double electric_field) const;
    virtual double ComputeScint(const double energy_deposit, const double step_length, const double electric_field) const;

  protected: 
    const double fWion; 
    const double fBirksK; 
    const double fBirksA; 
    const double fLArDensity; 
    double fIonizationDensity; 

}; 

#endif /* end of include guard SLARIONANDSCINTMODEL_H */

