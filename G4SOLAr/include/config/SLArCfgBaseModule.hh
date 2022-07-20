/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArCfgBaseModule.hh
 * @created     : martedì lug 19, 2022 10:40:14 CEST
 */

#ifndef SLARCFGBASEMODULE_HH

#define SLARCFGBASEMODULE_HH

#include <iostream>
#include "TNamed.h"
#include "TVector3.h"
#include "TGraph.h"

class SLArCfgBaseModule : public TNamed {
  public: 
    SLArCfgBaseModule(); 
    SLArCfgBaseModule(int idx); 
    SLArCfgBaseModule(int idx, float xc, float yc, float zc, float phi, float theta, float psi); 
    SLArCfgBaseModule(const SLArCfgBaseModule& base); 
    virtual ~SLArCfgBaseModule(); 

    inline virtual void DumpInfo() {}; 
    inline int GetIdx() {return fIdx;}
    inline void SetIdx(int idx) {fIdx = idx;}
    inline float GetX() {return fX;}
    inline void SetX(float x) {fX = x;}
    inline float GetY() {return fY;}
    inline void SetY(float y) {fY = y;}
    inline float GetZ() {return fZ;}
    inline void SetZ(float z) {fZ =  z;}
    inline float GetPhi() {return fPhi;}
    inline void SetPhi(float phi) {fPhi = phi;}
    inline float GetTheta() {return fTheta;}
    inline void SetTheta(float theta) {fTheta = theta;}
    inline float GetPsi() {return fPsi;}
    inline void SetPsi(float psi) {fPsi = psi;}
    inline float GetPhysX() {return fPhysX;}
    inline void SetPhysX(float x) {fPhysX = x;}
    inline float GetPhysY() {return fPhysY;}
    inline void SetPhysY(float y) {fPhysY = y;}
    inline float GetPhysZ() {return fPhysZ;}
    inline void SetPhysZ(float z) {fPhysZ =  z;}
    inline int GetBinIdx() {return  fBin;}
    inline void SetBinIdx(int bin) {fBin =  bin;}
    inline TGraph* GetGraphShape() {return fGShape;}
    inline void SetNormal(double x, double y, double z) {fNormal = TVector3(x, y, z);};
    inline void SetNormal(TVector3 vN) {fNormal = vN;}
    inline TVector3 GetNormal() {return fNormal;}

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

