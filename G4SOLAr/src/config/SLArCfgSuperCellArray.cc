/**
 * @author      : guff (guff@guff-gssi)
 * @file        : BCCfgArrayPMT
 * @created     : giovedì giu 04, 2020 11:47:56 CEST
 */
#include "TRegexp.h"
#include "TPRegexp.h"
#include "TObjString.h"
#include "TObjArray.h"

#include "config/SLArCfgSuperCellArray.hh"

ClassImp(SLArCfgSuperCellArray)

SLArCfgSuperCellArray::SLArCfgSuperCellArray() : fH2Bins(nullptr), fSerie(0), fNSuperCells(0)
{
  SetName("aPMTArrayHasNoName");
}

SLArCfgSuperCellArray::SLArCfgSuperCellArray(TString name, int serie) : 
  fH2Bins(nullptr), fNSuperCells(0)
{
  SetName(name);
  fSerie = serie;
  printf("SLArCfgSuperCellArray created with name %s\n", 
      this->GetName());
}


SLArCfgSuperCellArray::SLArCfgSuperCellArray(const SLArCfgSuperCellArray &cfg)
  : fH2Bins(0), fNSuperCells(0)
{
  SetName(cfg.fName);
  fSerie = cfg.fSerie;

  for (auto &pmt : cfg.fSCMap)
  {
    fSCMap.insert(std::make_pair(
          pmt.first, (SLArCfgSuperCell*)pmt.second->Clone()));
    fNSuperCells++;
  }

  if (fNSuperCells) SetTH2BinIdx();
}

SLArCfgSuperCellArray::~SLArCfgSuperCellArray()
{
  if (fH2Bins) {delete fH2Bins; fH2Bins = nullptr;}
  for (auto &sc : fSCMap)
    if (sc.second) {delete sc.second; sc.second = 0;}
  fSCMap.clear();
  fNSuperCells = 0;
}

void SLArCfgSuperCellArray::DumpMap() 
{
  std::printf("SLArCfgSuperCellArray %s has %i entries\n", 
      fName.Data(), (int)fSCMap.size());
  for (auto &itr : fSCMap)
    itr.second->DumpInfo();
}


void SLArCfgSuperCellArray::RegisterSuperCell(SLArCfgSuperCell* scInfo)
{
  int idx = scInfo->GetIdx();
  if (fSCMap.count(idx)) 
  {
    std::cout<<"SuperCell "<<idx<<" already registered. Skip"<<std::endl;
    return;
  }
  fSCMap.insert( 
      std::make_pair(idx, scInfo)
      );
  fNSuperCells++; 
}


SLArCfgSuperCell* SLArCfgSuperCellArray::GetSuperCellInfo(int idx)
{
  return fSCMap.find(idx)->second;
}

void SLArCfgSuperCellArray::BuildPolyBinHist()
{
  fH2Bins = new TH2Poly();
  fH2Bins->SetName(fName+"_bins");

  fH2Bins->SetFloat();

  int iBin = 1;
  for (auto &pmt : fSCMap) 
  {
    TString gBinName = Form("gBin%i", iBin);
    fH2Bins->AddBin(
        pmt.second->GetGraphShape()->Clone(gBinName));
    iBin ++;
  }
}

TH2Poly* SLArCfgSuperCellArray::GetTH2()
{
  return fH2Bins;
}

void SLArCfgSuperCellArray::SetTH2BinIdx()
{
  BuildPolyBinHist();

  for (auto &pmt : fSCMap)
  {
    double x = pmt.second->GetX();
    double y = pmt.second->GetY();
    int  idx = fH2Bins->FindBin(x, y);
    //std::cout << "BCSystemConfigPMT::SetTH2BinIdx: PMT" << 
      //pmt.second->GetIdx() << " -> bin " << idx << std::endl;
    pmt.second->SetBinIdx(idx);
  }
}

/*
 *void SLArCfgSuperCellArray::ClearSuperCellArrayHits()
 *{
 *  for (auto &sc : fSCMap)
 *    sc.second->ClearHits();
 *}
 */

void SLArCfgSuperCellArray::ResetConfig()
{
  std::cout << "SLArCfgSuperCellArray::ResetConfig" << std::endl;
  fNSuperCells = 0;

  std::cout << "Clear fSCMap" << std::endl;
  for (auto &scInfo : fSCMap)
    if (scInfo.second) delete scInfo.second;
  fSCMap.clear();

  std::cout << "Delete fH2Bins" << std::endl;
  if (fH2Bins) {delete fH2Bins; fH2Bins = nullptr;}
}

void SLArCfgSuperCellArray::SetSuperCellArrayPosition(double x, double y, double z)
{
  fPosition = TVector3(x, y, z);
  return;
}

void SLArCfgSuperCellArray::SetArrayRotAngle(double rx, double ry, double rz)
{
  fRotationAngle = TVector3(rx, ry, rz);
  return;
}


TVector3 SLArCfgSuperCellArray::GetSuperCellArrayPosition()
{
  return fPosition;
}
