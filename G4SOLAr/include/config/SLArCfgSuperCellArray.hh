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

#include "config/SLArCfgAssembly.hh"
#include "config/SLArCfgSuperCell.hh"

class SLArCfgSuperCellArray : public SLArCfgAssembly<SLArCfgSuperCell> {
  public:
    SLArCfgSuperCellArray();
    SLArCfgSuperCellArray(TString name, int serie = 0);
    SLArCfgSuperCellArray(const SLArCfgSuperCellArray &cfg);
    ~SLArCfgSuperCellArray() override;

    void DumpMap(); 

    ClassDefOverride(SLArCfgSuperCellArray, 1);
};

#endif /* end of include guard BCCFGARRAYPMT_HH */

