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
    ~SLArCfgReadoutTile(); 

    double Set2DSize_X() {return f2DSize_X;}
    double Set2DSize_Y() {return f2DSize_Y;}
    void   Set2DSize_X(float _x) {f2DSize_X = _x;}
    void   Set2DSize_Y(float _y) {f2DSize_Y = _y;}
    void   DumpInfo();

  private:
    float   f2DSize_X; 
    float   f2DSize_Y;
    void    BuildGShape();

  public:
    ClassDef(SLArCfgReadoutTile, 1);
}; 



#endif /* end of include guard SLARCFGTILE_HH */

