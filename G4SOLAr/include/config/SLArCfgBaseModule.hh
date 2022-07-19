/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgBaseModule
 * @created     : martedì lug 19, 2022 10:40:14 CEST
 */

#ifndef SLARCFGBASEMODULE_HH

#define SLARCFGBASEMODULE_HH

#include "TObject.h"
#include "TPRegexp.h"
#include "TVector3.h"
#include "TGraph.h"

class SLArCfgBaseModule : public TNamed {
  public: 
    SLArCfgBaseModule(); 
    SLArCfgBaseModule(int idx); 
    SLArCfgBaseModule(int idx, float xc, float yc, float zc, float phi, float theta, float psi); 
    virtual ~SLArCfgBaseModule(); 

    virtual void DumpInfo() {}; 
    int         GetIdx   ()                {return fIdx   ;}
    void        SetIdx   (int idx)         {fIdx   = idx  ;}
    float       GetX     ()                {return fX     ;}
    void        SetX     (float x)         {fX     = x    ;}
    float       GetY     ()                {return fY     ;}
    void        SetY     (float y)         {fY     = y    ;}
    float       GetZ     ()                {return fZ     ;}
    void        SetZ     (float z)         {fZ     =  z   ;}
    float       GetPhi   ()                {return fPhi ;}
    void        SetPhi   (float phi)       {fPhi = phi;}
    float       GetTheta ()                {return fTheta ;}
    void        SetTheta (float theta)     {fTheta = theta;}
    float       GetPsi   ()                {return fPsi ;}
    void        SetPsi   (float psi)       {fPsi = psi;}
    float       GetPhysX ()                {return fPhysX ;}
    void        SetPhysX (float x)         {fPhysX = x    ;}
    float       GetPhysY ()                {return fPhysY ;}
    void        SetPhysY (float y)         {fPhysY = y    ;}
    float       GetPhysZ ()                {return fPhysZ ;}
    void        SetPhysZ (float z)         {fPhysZ =  z   ;}
    int         GetBinIdx()                {return  fBin  ;}
    void        SetBinIdx(int bin)         {fBin   =  bin ;}
    TGraph*     GetGraphShape()            {return fGShape;}
    void        SetNormal(double x, double y, double z);
    void        SetNormal(TVector3 vN)     {fNormal = vN  ;}
    TVector3    GetNormal()                {return fNormal;}

  protected: 
    int                 fIdx   ; //!< Module index
    int                 fBin   ; //!< Module bin id in TH2Poly projection
    float               fX     ; //!< Module x coordinate in the parent volume frame
    float               fY     ; //!< Module y coordinate in the parent volume frame
    float               fZ     ; //!< Module z coordinate in the parent volume frame 
    float               fPhi   ; //!< Module rotation phi respect to parent volume
    float               fTheta ; //!< Module rotation theta respect to parent volume
    float               fPsi   ; //!< Module rotation psi respect to parent volume
    float               fPhysX ; //!< Module x coordinate in the World frame
    float               fPhysY ; //!< Module y coordinate in the World frame
    float               fPhysZ ; //!< Module z coordinate in the World frame 
    TVector3            fNormal; //!< Module normal vector
    TGraph*             fGShape; //!< Module shape in the TH2Poly projection

    virtual void BuildGShape() {}; 

  public:
    ClassDef(SLArCfgBaseModule, 1); 

};



#endif /* end of include guard SLARCFGBASEMODULE_HH */

