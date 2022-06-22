/**
 * @author      : guff (guff@guff-gssi)
 * @file        : BCCfgArrayPMT
 * @created     : giovedì giu 04, 2020 11:47:35 CEST
 */

#ifndef BCCFGARRAYPMT_HH

#define BCCFGARRAYPMT_HH

#include <iostream>
#include <fstream>
#include <map>

#include "TNamed.h"
#include "TVector3.h"
#include "TH2Poly.h"
#include "config/SLArCfgSuperCell.hh"

class SLArCfgSuperCellArray : public TNamed {
  public:
    SLArCfgSuperCellArray();
    SLArCfgSuperCellArray(TString name, int serie = 0);
    SLArCfgSuperCellArray(const SLArCfgSuperCellArray & cfg);
    ~SLArCfgSuperCellArray();

    void DumpMap();
    int  GetSuperCellArraySerie() {return fSerie;}
    SLArCfgSuperCell* GetSuperCellInfo(int idx);
    std::map<int, SLArCfgSuperCell*>& GetMap()  {return fSCMap;}
    TH2Poly* GetTH2();
    void     SetSuperCellArrayPosition(double x, double y, double z);
    TVector3 GetSuperCellArrayPosition();
    void     SetArrayRotAngle  (double rx, double ry, double rz);
    TVector3 GetRotationAngle  () {return fRotationAngle;}
    void     SetTH2BinIdx      ();
    //void     ClearSuperCellArrayHits ();
    void     ResetConfig();
    void     RegisterSuperCell(SLArCfgSuperCell* scInfo);   


  private:
    TH2Poly*                 fH2Bins;
    TVector3                 fPosition;
    TVector3                 fRotationAngle;
    int                      fSerie;
    int                      fNSuperCells; 
    std::map<int, SLArCfgSuperCell*> fSCMap;
    
    // private methods
    void        RegisterMapLine(TString str, double rot_angle);
    void        RegisterMapLine(TString str, 
                                std::vector<int> excludelist);
    void        BuildPolyBinHist();
    double      ReadValue(TString str, TString field);
    TString     ReadModel(TString str, TString field);

  public:
    ClassDef(SLArCfgSuperCellArray,1);
};

#endif /* end of include guard BCCFGARRAYPMT_HH */

