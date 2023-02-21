/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArCfgSuperCell
 * @created     : lunedì feb 10, 2020 18:11:46 CET
 *
 * \brief SuperCell information container 
 */

#ifndef SLARCFGSUPERCELL_HH

#define SLARCFGSUPERCELL_HH

#include "config/SLArCfgBaseModule.hh"

class SLArCfgSuperCell : public SLArCfgBaseModule
{
  public:
    SLArCfgSuperCell();
    SLArCfgSuperCell(int idx);
    SLArCfgSuperCell(int idx, float xc, float yc, float zc, 
          float phi, float theta, float psi);
    ~SLArCfgSuperCell();

    double Get2DSize_X() {return f2DSize_X;}
    double Get2DSize_Y() {return f2DSize_Y;}
    void   Set2DSize_X(float _x) {f2DSize_X = _x;}
    void   Set2DSize_Y(float _y) {f2DSize_Y = _y;}
    void   DumpInfo() override;
    TGraph* BuildGShape() override;

  protected:
    float   f2DSize_X; 
    float   f2DSize_Y;

  public:
    ClassDefOverride(SLArCfgSuperCell, 1);
};


#endif /* end of include guard BCPMT_HH */

