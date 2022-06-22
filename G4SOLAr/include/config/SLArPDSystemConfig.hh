/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArPDSystemConfig
 * @created     : lunedì feb 10, 2020 18:10:37 CET
 * 
 * \brief Map of PMTs inside the TestCell
 *
 */

#ifndef SLArPDSystemConfig_HH
                         
#define SLArPDSystemConfig_HH

#include <iostream>
#include <fstream>

#include "config/SLArCfgSuperCellArray.hh"

class SLArPDSystemConfig : public TNamed
{
  public:
    SLArPDSystemConfig();
    SLArPDSystemConfig(const SLArPDSystemConfig & cfg);
    SLArPDSystemConfig(TString name);
    ~SLArPDSystemConfig();

    void DumpPMTSysConfig();
    int  RegisterArray(TString name);
    //int  RegisterArray(TString name, TString ascii_path);
    //int  RegisterArray(TString name, TString ascii_path, double rot);
    int  RegisterArray(SLArCfgSuperCellArray* array);

    SLArCfgSuperCell* GetSuperCellInfo(int idx);

    SLArCfgSuperCellArray*    GetArray(TString name);
    std::map<TString, SLArCfgSuperCellArray*>& GetArrayMap() {return fArrayMap;}

    //void     ClearPMTHits      ();
    void     ResetConfig();

  private:
    int      fNSuperCells;
    std::map<TString, SLArCfgSuperCellArray*> fArrayMap;

  public:
    ClassDef(SLArPDSystemConfig, 2);
};

#endif /* end of include guard BCPMTMAP_HH */


