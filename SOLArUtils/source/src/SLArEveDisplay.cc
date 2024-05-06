/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArEveDisplay
 * @created     : Thursday Apr 11, 2024 16:58:14 CEST
 */

#include "TObject.h"
#include <SLArEveDisplay.hh>
#include <SLArUnit.hpp>
#include <TGeoManager.h>
#include <TEveFrameBox.h>
#include <TEveRGBAPalette.h>
#include <TRootBrowser.h>
#include <TEveBrowser.h>

#include <TGTab.h>
#include <TGButton.h>

#include <TString.h>
#include <TSystem.h>

#include <TStyle.h>
#include <cstddef>

ClassImp(display::SLArEveDisplay)

namespace display {


  SLArEveDisplay::SLArEveDisplay() 
    : TGMainFrame(nullptr, 800, 800), fHitFile(nullptr), fHitTree(nullptr), fLastEvent(1), fCurEvent(0)
  {
    //gStyle->SetPalette(kSunset);
    fTimer = std::make_unique<TTimer>("gSystem->ProcessEvents();", 50, kFALSE);
    fEveManager = std::unique_ptr<TEveManager>( TEveManager::Create() );
    fPalette = std::make_unique<TEveRGBAPalette>();
  }

  SLArEveDisplay::~SLArEveDisplay()
  { 
    if (fHitFile) {
      fHitFile->Close(); 
      delete fHitFile;
    }
  }

  int SLArEveDisplay::LoadHitFile(const TString file_path, const TString tree_key) {
    if (fHitFile) {
      fHitFile->Close();
      fHitTree = nullptr;
    }
    fHitFile = new TFile( file_path ); 
    fHitTree = fHitFile->Get<TTree>( tree_key ); 
    if (fHitTree) {
      fLastEvent = fHitTree->GetEntries() - 1; 
      fHitTree->SetBranchAddress("hit_tpc"  , &fHitVars.hit_tpc); 
      fHitTree->SetBranchAddress("hit_x"    , &fHitVars.hit_x); 
      fHitTree->SetBranchAddress("hit_y"    , &fHitVars.hit_y); 
      fHitTree->SetBranchAddress("hit_z"    , &fHitVars.hit_z); 
      fHitTree->SetBranchAddress("hit_q"    , &fHitVars.hit_q); 
      fHitTree->SetBranchAddress("hit_qtrue", &fHitVars.hit_qtrue); 
    }

    return 0;
  }

  void SLArEveDisplay::Configure(const rapidjson::Value& config) {

    assert( config.HasMember("TPC") ); 
    if ( config["TPC"].IsObject() ) {
      ConfigureTPC( config["TPC"] );
    }
    else if (config["TPC"].IsArray()) {
      for (const auto& jtpc : config["TPC"].GetArray()) {
        ConfigureTPC( jtpc );
      }
    }

    return;
  }

  void SLArEveDisplay::ConfigureTPC(const rapidjson::Value& tpc_config) {

    assert( tpc_config.HasMember("copyID") );
    assert( tpc_config.HasMember("position") ); 
    assert( tpc_config.HasMember("dimensions") ); 

    GeoTPC_t geo_tpc;

    geo_tpc.fID = tpc_config["copyID"].GetInt();

    const auto& jpos = tpc_config["position"].GetObj(); 
    double pos_unit = unit::Unit2Val( jpos["unit"] );
    geo_tpc.fPosition.SetX( jpos["xyz"].GetArray()[0].GetDouble() * pos_unit ); 
    geo_tpc.fPosition.SetY( jpos["xyz"].GetArray()[1].GetDouble() * pos_unit ); 
    geo_tpc.fPosition.SetZ( jpos["xyz"].GetArray()[2].GetDouble() * pos_unit ); 

    const auto& jdims = tpc_config["dimensions"].GetArray(); 
    for (const auto& jdim : jdims) {
      TString var_name = jdim["name"].GetString();
      if      ( var_name == "tpc_x") geo_tpc.fDimension.SetX(unit::ParseJsonVal( jdim )); 
      else if ( var_name == "tpc_y") geo_tpc.fDimension.SetY(unit::ParseJsonVal( jdim )); 
      else if ( var_name == "tpc_z") geo_tpc.fDimension.SetZ(unit::ParseJsonVal( jdim )); 
    }

    geo_tpc.fVolume = std::make_unique<TEveFrameBox>();
    geo_tpc.fVolume->SetAABoxCenterHalfSize( 
        geo_tpc.fPosition.x(), geo_tpc.fPosition.y(), geo_tpc.fPosition.z(), 
        0.5*geo_tpc.fDimension.x(), 0.5*geo_tpc.fDimension.y(), 0.5*geo_tpc.fDimension.z()); 
    geo_tpc.fVolume->SetFrameColor( kCyan ); 
    //printf("Adding TPC at (%.2f %.2f, %.2f) with size (%.2f %.2f, %.2f)\n\n", 
        //geo_tpc.fPosition.x(), geo_tpc.fPosition.y(), geo_tpc.fPosition.z(), 
        //0.5*geo_tpc.fDimension.x(), 0.5*geo_tpc.fDimension.y(), 0.5*geo_tpc.fDimension.z()); 

    auto diff = geo_tpc.fPosition - 0.5*geo_tpc.fDimension;
    auto sum  = geo_tpc.fPosition + 0.5*geo_tpc.fDimension;

    if (diff.x() < fXmin) {
      if (diff.x() < 0 ) fXmin = 1.1*(diff.x());
      else               fXmin = 0.9*(diff.x());
    } 
    if (diff.y() < fYmin) {
      if (diff.y() < 0 ) fYmin = 1.1*(diff.y());
      else               fYmin = 0.9*(diff.y());
    } 
    if (diff.z() < fZmin) {
      if (diff.z() < 0 ) fZmin = 1.1*(diff.z());
      else               fZmin = 0.9*(diff.z());
    } 

    if (sum.x() < fXmax) {
      if (sum.x() < 0 ) fXmax = 1.1*(sum.x());
      else              fXmax = 0.9*(sum.x());
    } 
    if (sum.y() < fYmax) {
      if (sum.y() < 0 ) fYmax = 1.1*(sum.y());
      else              fYmax = 0.9*(sum.y());
    } 
    if (sum.z() < fZmax) {
      if (sum.z() < 0 ) fZmax = 1.1*(sum.z());
      else              fZmax = 0.9*(sum.z());
    } 

    fTPCs.push_back( std::move(geo_tpc) ); 

    auto hit_set = std::make_unique<TEveBoxSet>();
    hit_set->SetNameTitle(Form("hitsTPC%i", geo_tpc.fID), Form("TPC %i hits", geo_tpc.fID));

    fEveManager->AddElement( hit_set.get() ); 
    fHitSet.push_back( std::move(hit_set) ); 

    return;
  }

  int SLArEveDisplay::ReDraw() {
    auto top = fEveManager->GetCurrentEvent();

    //gMultiView->DestroyEventRPhi();
    //gMultiView->ImportEventRPhi(top);

    //gMultiView->DestroyEventRhoZ();
    //gMultiView->ImportEventRhoZ(top);

    size_t i = 0; 
    for (auto& hitset : fHitSet) {
      hitset->SetFrame( fTPCs.at(i).fVolume.get() );
      i++;
    }

    fEveManager->Redraw3D( kTRUE ); 

    return 0;
  }

  int SLArEveDisplay::ReadHits() {
    fHitTree->GetEntry( fCurEvent );

    float q_max = 0;
    
    for (size_t ihit = 0; ihit < fHitVars.hit_tpc->size(); ihit++) {
      int tpc_idx = GetTPCindex( fHitVars.hit_tpc->at(ihit) ); 
      fHitSet.at(tpc_idx)->AddBox( 
          fHitVars.hit_x->at(ihit), fHitVars.hit_y->at(ihit), fHitVars.hit_z->at(ihit) ); 
      fHitSet.at(tpc_idx)->DigitValue( fHitVars.hit_q->at(ihit) ); 
      if (fHitVars.hit_q->at(ihit) > q_max) q_max = fHitVars.hit_q->at(ihit);
    }

    fPalette->SetMax(1.1*q_max); 
    fPalette->SetMin(1500); 
  
    for (auto &hitset : fHitSet) {
      hitset->RefitPlex(); 
      hitset->SetDefDepth(4.0); 
      hitset->SetDefWidth(4.0); 
      hitset->SetDefHeight(4.0); 
      hitset->SetPalette( fPalette.get() ); 
    }


    return 0;
  }

  void SLArEveDisplay::ResetHits() {
    for (auto& hitset : fHitSet) {
      printf("deleting %i hits...\n", hitset.get()->NumberOfConnections());
      hitset->Reset(TEveBoxSet::kBT_AABoxFixedDim, false, hitset->GetNItems());
      //fEveManager->GetViewers()->DeleteAnnotations();
      //fEveManager->GetCurrentEvent()->DestroyElements();
    }
    return;
  }

  void SLArEveDisplay::GoToEvent(const Long64_t iev) {
    printf("display event %lld\n", fCurEvent);
    ResetHits(); 

    ReadHits(); 

    ReDraw();
  }

  void SLArEveDisplay::NextEvent() { 
    fCurEvent = TMath::Max(static_cast<Long64_t>(0), fCurEvent+1); 
    GoToEvent( fCurEvent ); 
  } 
  void SLArEveDisplay::PrevEvent() {
    fCurEvent = TMath::Min(static_cast<Long64_t>(fLastEvent), fCurEvent-1); 
    GoToEvent( fCurEvent ); 
  }


  int SLArEveDisplay::MakeGUI() {
   // Create minimal GUI for event navigation.

   auto browser = fEveManager->GetBrowser();
   browser->StartEmbedding(TRootBrowser::kLeft);

   auto frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
   frmMain->SetWindowName("XX GUI");
   frmMain->SetCleanup(kDeepCleanup);

   auto hf = new TGHorizontalFrame(frmMain);
   //{
      TString icondir(TString::Format("%s/icons/", gSystem->Getenv("ROOTSYS")));
      TGPictureButton* b = 0;

      b = new TGPictureButton(hf, gClient->GetPicture(icondir+"GoBack.gif"));
      hf->AddFrame(b);
      b->Connect("Clicked()", "display::SLArEveDisplay", this, "PrevEvent()");

      b = new TGPictureButton(hf, gClient->GetPicture(icondir+"GoForward.gif"));
      hf->AddFrame(b);
      b->Connect("Clicked()", "display::SLArEveDisplay", this, "NextEvent()");
   //}
   frmMain->AddFrame(hf);

   frmMain->MapSubwindows();
   frmMain->Resize();
   frmMain->MapWindow();

   browser->StopEmbedding();
   browser->SetTabTitle("Event Control", 0);

   return 1;
}
}
