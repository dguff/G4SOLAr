/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEveDisplay.hh
 * @created     : Thursday Apr 11, 2024 16:47:34 CEST
 */

#ifndef SLAREVEDISPLAY_HH

#define SLAREVEDISPLAY_HH

#include <RtypesCore.h>
#include <cstddef>
#include <iostream>
#include <TFile.h>
#include <TTree.h>

#include <TEveManager.h>
#include <TEveBoxSet.h>
#include <TEveManager.h>
#include <TEveEventManager.h>
#include <TEveViewer.h>
#include <TEveFrameBox.h>
#include <Math/Vector3D.h>
#include <TTimer.h>

#include <memory>
#include <rapidjson/document.h>

#include <SLArRecoHits.hpp>

namespace display {

  struct GeoTPC_t {
    std::unique_ptr<TEveFrameBox> fVolume;
    ROOT::Math::XYZVectorD fPosition = {};
    ROOT::Math::XYZVectorD fDimension = {};
    Int_t fID = {};
  };

class SLArEveDisplay : public TGMainFrame {
  public: 
    SLArEveDisplay();
    ~SLArEveDisplay();

    int LoadHitFile(const TString file_path, const TString tree_key); 

    void Configure(const rapidjson::Value& config); 
    int  MakeGUI(); 
    int  ReadHits(); 
    void ResetHits();  
    int  ReDraw(); 
    void NextEvent();
    void PrevEvent();
    void GoToEvent(const Long64_t iev); 


  private: 
    TFile* fHitFile = {};
    TTree* fHitTree = {}; 
    hitvarContainersPtr_t fHitVars = {};
    std::unique_ptr<TTimer> fTimer = {};
    std::unique_ptr<TEveManager> fEveManager = {};
    std::vector<std::unique_ptr<TEveBoxSet>> fHitSet = {};
    std::unique_ptr<TEveRGBAPalette> fPalette = {};
    std::vector<GeoTPC_t> fTPCs;

    Long64_t  fCurEvent = {};
    Long64_t  fLastEvent = {};
    

    Float_t fXmin = {}; 
    Float_t fXmax = {}; 
    Float_t fYmin = {}; 
    Float_t fYmax = {}; 
    Float_t fZmin = {}; 
    Float_t fZmax = {}; 

    void ConfigureTPC(const rapidjson::Value& tpc_config);
    inline Int_t GetTPCindex(const Int_t itpc) {
      Int_t index = 0;
      for (const auto& tpc : fTPCs) {
        if (itpc == tpc.fID)  return index;
        index++;
      }
      return -1;
    }

    ClassDef(display::SLArEveDisplay, 0)
};
}
#endif /* end of include guard SLAREVEDISPLAY_HH */

