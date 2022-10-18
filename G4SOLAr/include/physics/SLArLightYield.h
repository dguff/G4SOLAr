#ifndef SLArLightYield_H
#define SLArLightYield_H 1

#include <math.h>       /* exp */
#include <vector>
#include "G4SystemOfUnits.hh"


class SLArLightYield{

  private:
    // ************************************************************************
    // LArQL Argon Scintillation Prediction PARAMETRIZATION
    // https://lss.fnal.gov/archive/2022/conf/fermilab-conf-22-088-nd.pdf
    // ************************************************************************

    // All used for LArQL/Q
    const double k_Birks=0.0486; // Birks Constant
    const double A_Birks=0.800; // Birks Constant
    const double Wion = 23.6 ; // W-value for ionisation in eV/e-
    const double Nex_Ni = 0.29; // Ratio of number of electrons to number of ions
    const double Ni = 1.0e6/Wion; // Ionization density in LAr
    const double pLAr = 1.30;  // Density Liquid Argon
    const double alpha = 0.032; // Fit result for the Corr function
    const double beta = 0.008258; // Fit result for the Corr function
    const std::vector<double> chi_param = {2.151572666e-5, -3.988504, 1.38343421, 1.9919521e-6};

    double QBirks(double dE, double dx, double E_field); // Default Birks Law
    double Corr(double dE, double dx, double E_field); // Correction factor for Birks Law
    double QChi(double dE, double dx, double E_field); // Chi2 fit to Birks Law Correction
    double Qinf(); // Charge yield at an infinite electric field -

    //Set fixed light yield for Flat-example
    double FlatLightYieldPerMeV = 24000.;


  public:
    SLArLightYield(); // Constructor
    ~SLArLightYield(); // Destructor

    // Light and charge yield functions
    double LArQL(double energyDeposit, double stepWidth, double electricField);
    double LArQQ(double energyDeposit, double stepWidth, double electricField);
    double Flat();

};
#endif
