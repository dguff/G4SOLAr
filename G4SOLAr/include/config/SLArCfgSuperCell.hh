/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArCfgSuperCell
 * @created     : lunedì feb 10, 2020 18:11:46 CET
 *
 * \brief SuperCell information container 
 */

#ifndef SLARCFGSUPERCELL_HH

#define SLARCFGSUPERCELL_HH

#include "TObject.h"
#include "TPRegexp.h"
#include "TVector3.h"
#include "TGraph.h"

class SLArCfgSuperCell : public TObject
{
  public:
    SLArCfgSuperCell();
    SLArCfgSuperCell(int idx);
    SLArCfgSuperCell(int idx, float xc, float yc, float zc, 
          float phi, float theta, float psi);
    ~SLArCfgSuperCell();

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
    void        Set2DSize_X(float xx)      {f2DSize_X = xx;}
    void        Set2DSize_Y(float yy)      {f2DSize_Y = yy;}
    float       Get2DSize_X()              {return f2DSize_X;}
    float       Get2DSize_Y()              {return f2DSize_Y;}
    TGraph*     GetGraphShape()            {return fGShape;}
    void        SetPMTNormal(double x, double y, double z);
    void        SetPMTNormal(TVector3 vN)  {fNormal = vN  ;}
    TVector3    GetPMTNormal()             {return fNormal;}

    //void        RecordHit(BCEventHitPMT* hit);
    //bool        IsHit    ()          ;
    //std::vector<BCEventHitPMT*>&
                //GetHits  ()          ;
    //void        SetHits  (std::vector<BCEventHitPMT*>);
    //double      GetNhits ()        {return fHits.size();}
    double      GetTime  ()          ;
    double      GetCharge()          ;
    //void        ClearHits()          ; 
    void        ReadModel(TString modelName);
    void        ParseModelField(TString field, 
                                TString key, TString val="");

    void        DumpInfo();

  private:
    int                 fIdx   ;
    int                 fBin   ;
    // PMT coordinates in the array frame
    float               fX     ;
    float               fY     ;
    float               fZ     ; 
    float               fPhi   ;
    float               fTheta ; 
    float               fPsi   ;
    // PMT coordinates in the physical volume
    float               fPhysX ;
    float               fPhysY ;
    float               fPhysZ ; 
    float               f2DSize_X; 
    float               f2DSize_Y;
    TVector3            fNormal;
    TGraph*             fGShape;
    TString             fModel ;
    //std::vector<BCEventHitPMT*> fHits  ;

    void      BuildGShape();

  public:
    ClassDef(SLArCfgSuperCell, 1);
};


#endif /* end of include guard BCPMT_HH */

