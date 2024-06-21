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

#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TEveManager.h>
#include <TEveBoxSet.h>
#include <TEveManager.h>
#include <TEveEventManager.h>
#include <TEveViewer.h>
#include <TEveFrameBox.h>
#include <TEveTrack.h>
#include <Math/Vector3D.h>
#include <TTimer.h>

#include <memory>
#include <rapidjson/document.h>

#include <event/SLArMCEvent.hh>
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
    int LoadTrackFile(const TString file_path, const TString tree_key);

    void Configure(const rapidjson::Value& config); 
    int  MakeGUI(); 
    int  ReadHits(); 
    int  ReadTracks();
    void ResetHits();  
    int  ReDraw(); 
    void NextEvent();
    void PrevEvent();
    void ProcessEvent(); 

    inline void SetEntry() {
      fCurEvent = fEnterEntry->GetNumberEntry()->GetIntNumber();
      ProcessEvent();
    }

    inline void SetEntry(const Long64_t iev) {
      fCurEvent = iev;
      ProcessEvent();
    }



  private: 
    TFile* fHitFile = {};
    TTree* fHitTree = {}; 
    TFile* fMCTruthFile = {};
    TTree* fMCTruthTree = {};
    hitvarContainersPtr_t fHitVars = {};
    SLArMCEvent* fMCEvent = {};
    std::unique_ptr<TTimer> fTimer = {};
    std::unique_ptr<TEveManager> fEveManager = {};
    std::vector<std::unique_ptr<TEveBoxSet>> fHitSet = {};
    std::vector<std::unique_ptr<TEveTrackList>> fTrackLists = {}; 
    TEveTrackPropagator* fPropagator = {};
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

    TGNumberEntry* fEnterEntry = {};
    TGHorizontalFrame*  fGframeEntry = {};
    TGLabel*       fEntryLabel = {};

    void ConfigureTPC(const rapidjson::Value& tpc_config);
    inline Int_t GetTPCindex(const Int_t itpc) {
      Int_t index = 0;
      for (const auto& tpc : fTPCs) {
        if (itpc == tpc.fID)  return index;
        index++;
      }
      return -1;
    }

    void set_track_style(TEveTrack* track); 

    inline void update_entry_label() {
      fEntryLabel->SetText(Form("%lld",fCurEvent));
      fEnterEntry->SetIntNumber( fCurEvent );
      fGframeEntry->Layout();
      return;
    }

    ClassDef(display::SLArEveDisplay, 0)
};
}
#endif /* end of include guard SLAREVEDISPLAY_HH */

