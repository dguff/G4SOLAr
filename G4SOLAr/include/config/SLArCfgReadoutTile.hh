/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgReadoutTile.hh
 * @created     : martedì lug 19, 2022 13:38:21 CEST
 */

#ifndef SLARCFGREADOUTTILE_HH

#define SLARCFGREADOUTTILE_HH

#include "config/SLArCfgBaseModule.hh"

class SLArCfgReadoutTile : public SLArCfgBaseModule 
{
  public: 
    SLArCfgReadoutTile(); 
    SLArCfgReadoutTile(int idx); 
    SLArCfgReadoutTile(int idx, float xc, float yc, float zc, float phi, float theta, float psi); 
    SLArCfgReadoutTile(const SLArCfgReadoutTile& ref); 
    ~SLArCfgReadoutTile(); 

    double Get2DSize_X() {return f2DSize_X;}
    double Get2DSize_Y() {return f2DSize_Y;}
    void   Set2DSize_X(float _x) {f2DSize_X = _x;}
    void   Set2DSize_Y(float _y) {f2DSize_Y = _y;}
    void   DumpInfo() override;
    void   BuildGShape() override;

  protected:
    float   f2DSize_X; 
    float   f2DSize_Y;

  public:
    ClassDefOverride(SLArCfgReadoutTile, 1);
}; 



#endif /* end of include guard SLARCFGTILE_HH */

