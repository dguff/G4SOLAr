/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArDetReadoutPlane
 * @created     : lunedì lug 18, 2022 11:09:54 CEST
 */

#ifndef SLARDETREADOUTPLANE_HH

#define SLARDETREADOUTPLANE_HH

#include "detector/SLArBaseDetModule.hh"
#include "material/SLArMaterialInfo.h"

class SLArDetReadoutTile;

class SLArDetReadoutPlane : public SLArBaseDetModule 
{       
  public:
    SLArDetReadoutPlane(); 
    SLArDetReadoutPlane(const SLArDetReadoutPlane& detReadoutPlane); 
    ~SLArDetReadoutPlane(); 

    void BuildMaterial(); 
    void BuildDefalutGeoParMap(); 
    void BuildReadoutPlane(SLArDetReadoutTile*); 

  protected:

  private: 
    SLArMaterialInfo* fMatReadoutPlane; 
};



#endif /* end of include guard SLARDETREADOUTPLANE_HH */

