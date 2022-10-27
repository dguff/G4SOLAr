/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : test_output.cc
 * @created     : mercoledì lug 20, 2022 16:40:43 CEST
 */

#include <getopt.h>
#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TChain.h"
#include "TTree.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH3D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TH2Poly.h"

#include "config/SLArCfgBaseSystem.hh"
#include "event/SLArMCEvent.hh"
#include "config/SLArCfgMegaTile.hh"
#include "config/SLArCfgSuperCellArray.hh"

#include "solar_root_style.hpp"

typedef SLArCfgBaseSystem<SLArCfgSuperCellArray> SLArPDSCfg;
typedef SLArCfgBaseSystem<SLArCfgMegaTile> SLArPixCfg;

const double avgLY         = 25980.5; //!< Average Light Yield
const double pdeSiPM       = 0.15;    //!< Photon Detection Efficiency for crosscheck
const double pdeSC         = 0.03;    //!< SuperCell Photon Detection Efficiency
const double PixFillFactor = 0.36*0.85; 

struct  SLArHistoSet {
  public: 
    SLArHistoSet(); 
    ~SLArHistoSet(); 

    int BuildHitMapHistPix(SLArPixCfg* pixCfg); 
    int BuildHitMapHistSC (SLArPDSCfg* scCfg ); 

    void Write(const char* output_path); 

    SLArPixCfg* fPixCfg; 
    SLArPDSCfg* fSCCfg; 
    std::map<int, TH2Poly*> hPixNPhMap; 
    std::map<int, TH2Poly*> hPixTPhMap; 
    std::map<int, TH2Poly*> hSCNPhMap; 
    TH1D* hvis; 
    TH1D* hNPhotons; 
    TH1D* hPixNHits;
    TH1D* hPixTHits; 
    TH1D* hSCNHits;
    TH1D* hWavelength; 
    TH1D* hEReco; 
    TH1D* hERecoPix; 
    TH1D* hERecoSC; 
    std::vector<TH1D*> hPosition; 

}; 

SLArHistoSet::SLArHistoSet() : 
  fPixCfg(nullptr), fSCCfg(nullptr), hvis(nullptr), hNPhotons(nullptr), 
  hPixNHits(nullptr), hPixTHits(nullptr), hSCNHits(nullptr),
  hWavelength(nullptr), hEReco(nullptr), hERecoPix(nullptr), hERecoSC(nullptr), 
  hPosition(3, nullptr)
{
  hvis = new TH1D("hvis", "Visible Energy", 200, 0., 20); 
  hNPhotons = new TH1D("hNPhotons", 
      "Nr of photons produced;Nr of photons produced (true);Entries",
      1000, 0, 1e6);
  hPixNHits = new TH1D("hPixNHits", 
      "Nr of collected photons (Readout Tile);Nr of detected photons;Entries",
      500, 0, 1e5);
  hPixTHits = new TH1D("hPixTHits", 
      "Time of photons hits (Readout Tile);Time of first photon hit on sensor [ns];Entries", 
      2000, 0, 3000); 
  hSCNHits = new TH1D("hSCNHits", 
      "Nr of collected photons (SuperCell);Nr of detected photons;Entries",
      500, 0, 1e5);
  hWavelength = new TH1D("hWavelength", 
      "Wavelength of detected ph;#it{#lambda} [nm];Entries", 
      500, 100, 400); 
  hEReco = new TH1D("hEReco", 
      "Reconstructed Energy (Pix + SuperCell);#it{E_{L}} [MeV];Entries", 600, 0, 30);
  hERecoPix = new TH1D("hERecoPix", 
      "Reconstructed Energy (Readout Tile);#it{E_{L}} [MeV];Entries", 600, 0, 30);
  hERecoSC = new TH1D("hERecoSC", 
      "Reconstructed Energy (SuperCell);#it{E_{L}} [MeV];Entries", 600, 0, 30);

  double  _dimensions[3] = {1.8, 3, 7}; 
  TString _positions[3] = {"x", "y", "z"};
  for (int i=0; i<3; i++) {
    hPosition[i] = new TH1D(Form("hPosition%s", _positions[i].Data()), 
        Form("Event starting point #it{%s};#it{%s} [mm];Entries", 
          _positions[i].Data(), _positions[i].Data()), 
        200, -_dimensions[i]*1000, _dimensions[i]*1000); 
  }
}

SLArHistoSet::~SLArHistoSet() {
  if (hvis) delete hvis; 
  if (hNPhotons) delete hNPhotons; 
  if (hPixNHits) delete hPixNHits;
  if (hPixTHits) delete hPixTHits; 
  if (hSCNHits ) delete hSCNHits;
  if (hWavelength) delete hWavelength; 
  if (hEReco) delete hEReco; 
  for (auto &h : hPosition) {
    if (h) delete h; 
  }
  hPosition.clear(); 
}

int SLArHistoSet::BuildHitMapHistPix(SLArPixCfg* pixCfg) {
  fPixCfg = new SLArPixCfg( *pixCfg );
  int nmodules = 0; 
  for (auto &mod : fPixCfg->GetModuleMap()) {
    SLArCfgMegaTile* mgTile = mod.second; 
    for (auto &cell : mgTile->GetMap()) {
      cell.second->BuildGShape(); 
      //printf("cell pos [phys]: [%.2f, %.2f, %.2f] - %g × %g\n", 
          //cell.second->GetPhysX(), cell.second->GetPhysY(), cell.second->GetPhysZ(),
          //cell.second->Get2DSize_X(), cell.second->Get2DSize_Y()); 
    }
    mgTile->BuildPolyBinHist(); 
    TH2Poly* h2p = (TH2Poly*)mgTile->GetTH2(); 
    hPixNPhMap.insert(std::make_pair(mgTile->GetIdx(), 
          (TH2Poly*)h2p->Clone(Form("nhits_map_%i", mgTile->GetIdx()))));
    hPixTPhMap.insert(std::make_pair(mgTile->GetIdx(), 
          (TH2Poly*)h2p->Clone(Form("thits_map_%i", mgTile->GetIdx()))));
  }
  nmodules = hPixNPhMap.size(); 
  return nmodules; 
}

int SLArHistoSet::BuildHitMapHistSC(SLArPDSCfg* scCfg) {
  printf("SLArHistoSet::BuildHitMapHistSC\n");
  fSCCfg = new SLArPDSCfg( *scCfg );
  printf("SCArrays size: %lu\n", fSCCfg->GetModuleMap().size());
  int nmodules = 0; 
  for (auto &mod : fSCCfg->GetModuleMap()) {
    SLArCfgSuperCellArray* sc_array = mod.second; 
    for (auto &cell : sc_array->GetMap()) {
      cell.second->BuildGShape(); 
      printf("IDX: %i\n", cell.first);
      //printf("cell pos [phys]: [%.2f, %.2f, %.2f] - %g × %g\n", 
          //cell.second->GetPhysX(), cell.second->GetPhysY(), cell.second->GetPhysZ(),
          //cell.second->Get2DSize_X(), cell.second->Get2DSize_Y()); 
    }
    sc_array->BuildPolyBinHist(); 
    TH2Poly* h2p = (TH2Poly*)sc_array->GetTH2(); 
    hSCNPhMap.insert(std::make_pair(sc_array->GetIdx(), 
          (TH2Poly*)h2p->Clone(Form("nhits_map_%i", sc_array->GetIdx()))));
  }
  nmodules = hSCNPhMap.size(); 
  return nmodules; 
}

void SLArHistoSet::Write(const char* output_path) {
  TFile* output = new TFile(output_path, "recreate");
  output->cd(); 

  hvis->Write(); 
  hNPhotons->Write();
  hPixNHits->Write(); 
  hPixTHits->Write(); 
  hSCNHits ->Write(); 
  hWavelength->Write(); 
  hEReco->Write(); 
  hERecoPix->Write(); 
  hERecoSC->Write(); 

  TDirectory* dPix = output->mkdir("pixSysMap"); 
  dPix->cd(); 
  for (auto &h2 : hPixNPhMap) h2.second->Write(); 

  output->cd(); 
  TDirectory* dSCTop = output->mkdir("SCTopSysMap"); 
  TDirectory* dSCBtm = output->mkdir("SCBtmSysMap"); 

  for (auto &h2 : hSCNPhMap) {
    if (h2.first < 5500) {
      dSCTop->cd(); 
      h2.second->Write(); 
    } else {
      dSCBtm->cd(); 
      h2.second->Write(); 
    }
  }
  
  output->cd(); 
  
  TH2D* h2hits = new TH2D("h2hits_avg", "Average nr of hits;#it{z} [mm];#it{y} [mm]", 
      140, -7000, +7000, 60, -3000, +3000); 
  for (const auto &mtile : hPixNPhMap) {
    for (const auto &&tbin : *(mtile.second->GetBins())) {
      TH2PolyBin* bin = (TH2PolyBin*)tbin; 
      TGraph* g = (TGraph*)bin->GetPolygon();
      double _x = 0.5*(g->GetPointX(0) + g->GetPointX(2)); 
      double _y = 0.5*(g->GetPointY(0) + g->GetPointY(2)); 
      //printf("_x = 0.5*(%g + %g)\n_y = 0.5*(%g + %g)\n\n", 
          //g->GetPointX(0), g->GetPointX(1), 
          //g->GetPointY(0), g->GetPointY(2));
      int iibin = h2hits->FindBin(_x, _y);
      h2hits->SetBinContent(iibin, bin->GetContent()); 
    }
  }

  h2hits->Write(); 

  output->Close(); 
}

void readout_event_tree(TTree* tree, SLArHistoSet* h, TH3D* visPix, TH3D* visSC); 

void test_output(const char* input_path, const char* output_path = "", 
    TH3D* hvisPix = nullptr, TH3D* hvisSC = nullptr) 
{
  //--------------------------------------------------------- Source plot style 
  slide_default(); 
  gROOT->SetStyle("slide_default");
  gStyle->SetPalette(kSunset); 

  //-------------------------------------------------------------- Open MC file
  printf("Opening input file...\n");
  TFile* file = new TFile(input_path); 
  
  // Get the configuration of the pixel/SuperCell readout system
  printf("Getting readout system configuration...\n");
  SLArPixCfg* pixCfg = (SLArPixCfg*)file->Get("PixSysConfig"); 
  SLArPDSCfg* scCfg  = (SLArPDSCfg*)file->Get("PDSSysConfig"); 

  // create histograms
  printf("Building hist collection...\n");
  SLArHistoSet* h = new SLArHistoSet(); 
  if (pixCfg) {
    printf("\tSiPM readout...");
    int jj = h->BuildHitMapHistPix(pixCfg); 
    printf("%i\n", jj);
  }

  if (scCfg) {
    printf("\tSuperCell readout...");
    int ii = h->BuildHitMapHistSC(scCfg); 
    printf("%i\n", ii); 
    printf("\texpected %lu\n", scCfg->GetModuleMap().size()); 
  }


  //---------------------------------------------------- Readout the event tree
  printf("Getting data tree...\n");
  TTree* tree = (TTree*)file->Get("EventTree"); 

  readout_event_tree(tree, h, hvisPix, hvisSC); 
  
  if ( (output_path != NULL) && (output_path[0] !=  '\0') ) {
    h->Write(output_path); 
  }
  return;
}

void merge_and_plot(const char* root_file_list, const char* output_path, 
    TH3D* hvisPix, TH3D* hvisSC) 
{
  slide_default(); 
  gROOT->SetStyle("slide_default"); 

  std::ifstream file_list; 
  file_list.open( root_file_list ); 
  if (!file_list.is_open()) {
    printf("%s root file list is not opened. Quit.\n", root_file_list);
    return;
  }

  SLArPixCfg* pixCfg = new SLArPixCfg();
  SLArPDSCfg* scCfg  = nullptr; 
  SLArHistoSet* h = new SLArHistoSet(); 

  TChain* ch = new TChain("EventTree", "G4SOLAr event tree"); 
  std::string str; 
  int ifile = 0; 
  while ( std::getline(file_list, str) ) {
    if (ifile == 0) {
      printf("Reading readout configuration from %s\n", str.c_str());
      TFile* f = new TFile(str.c_str()); 

      pixCfg = (SLArPixCfg*)f->Get("PixSysConfig"); //->Clone("local_pix_cfg");
      scCfg  = (SLArPDSCfg*)f->Get("PDSSysConfig"); //->Clone("local_sc_config");  

      if (pixCfg) {
        h->BuildHitMapHistPix(pixCfg); 
      }

      if (scCfg) {
        h->BuildHitMapHistSC(scCfg); 
      }
    }

    printf("Adding to %s to chain\n", str.c_str());
    ch->AddFile(str.c_str());  
    
    ifile++; 
  } 

  readout_event_tree(ch, h, hvisPix, hvisSC);

  if ( (output_path != NULL) && (output_path[0] !=  '\0') ) {
    h->Write(output_path); 
  }

  return; 
}

void readout_event_tree(TTree* tree, SLArHistoSet* h, TH3D* hvisPix, TH3D* hvisSC)
{
  SLArMCEvent* ev = nullptr; 
  tree->SetBranchAddress("MCEvent", &ev); 

  double tmax = 0; 
  double hmax = 0; 

  for (int iev = 0; iev<tree->GetEntries(); iev++) {
    tree->GetEntry(iev); 

    auto primaries = ev->GetPrimaries(); 
    double ev_edep = 0; 
    double primary_pos[3] = {0};

    //--------------------------------------------- Readout primaries and MC true
    size_t ip = 0; 
    int nphotons = 0; 
    for (const auto &p : primaries) {
      int pPDGID = p->GetCode();     // Get primary PDG code 
      int pTrkID = p->GetTrackID();  // Get primary trak id   

      nphotons += p->GetTotalScintPhotons(); 
      auto trajectories = p->GetTrajectories(); 
      int itrj = 0;
      for (const auto &trj : trajectories) {
        if (trj->GetTrackID() == pTrkID) {
          primary_pos[0] = trj->GetPoints().front().fX; 
          primary_pos[1] = trj->GetPoints().front().fY; 
          primary_pos[2] = trj->GetPoints().front().fZ; 

          for (int kk=0; kk<3; kk++) h->hPosition[kk]->Fill(primary_pos[kk]); 
        }
        /*
         *for (const auto &tp : trj->GetPoints()) {
         *  double pos_x = tp.fX;     // x coordinate [mm]
         *  double pos_y = tp.fY;     // y coordinate [mm]
         *  double pos_z = tp.fZ;     // z coordinate [mm]
         *  double edep  = tp.fEdep;  // Energy deposited in the step [MeV]
         *  ev_edep += edep; 
         *}
         */
        itrj++;
      } 
      ip++;
    }
    h->hvis->Fill(ev_edep); 
    h->hNPhotons->Fill(nphotons); 

    //---------------------------------------- Readout detected optical photons
    double htotPix = 0; 
    auto evpds = ev->GetReadoutTileSystem(); 
    for (const auto &mtile : evpds->GetMegaTilesMap()) {
      auto mgTileCfg = h->fPixCfg->GetModule(mtile.first);
      for (const auto &tile : mtile.second->GetTileMap()) {
        SLArEventTile* evTile = tile.second; 
        if (!evTile->GetHits().empty()) {
          int tile_idx = evTile->GetIdx();
          auto tileCfg = mgTileCfg->GetBaseElement(tile_idx); 
          int bin_idx = tileCfg->GetBinIdx();
          int nhits = evTile->GetNhits(); 
          double tfirst = evTile->GetTime(); 
          if (tfirst > tmax) tmax = tfirst; 
          double bc_ = h->hPixNPhMap[mgTileCfg->GetIdx()]->GetBinContent(bin_idx); 
          h->hPixNPhMap[mgTileCfg->GetIdx()]->SetBinContent(bin_idx, bc_ + nhits);
          h->hPixTPhMap[mgTileCfg->GetIdx()]->SetBinContent(bin_idx, tfirst); 

          htotPix += nhits; 

          for (const auto &hit : evTile->GetHits()) {
            h->hPixTHits->Fill( hit->GetTime(), 1./tree->GetEntries() ); 
            h->hWavelength->Fill( hit->GetWavelength()); 
          }
        }
      }
    }
    h->hPixNHits->Fill(htotPix); 

    double htotSC = 0.; 
    auto evsc = ev->GetSuperCellSystem(); 
    for (const auto &scev_ : evsc->GetSuperCellMap()) {
      SLArEventSuperCell* evSC = scev_.second; 
      if (!evSC->GetHits().empty()) {
        int  ArraySerial = ((int)scev_.first/100)*100; 
        auto SCArrayCfg = h->fSCCfg->GetModule(ArraySerial); 
        int tile_idx = evSC->GetIdx();
        auto SC = SCArrayCfg->GetBaseElement(tile_idx); 
        int bin_idx = SC->GetBinIdx();
        int nhits = evSC->GetNhits(); 
        double bc_ = h->hSCNPhMap[SCArrayCfg->GetIdx()]->GetBinContent(bin_idx); 
        h->hSCNPhMap[SCArrayCfg->GetIdx()]->SetBinContent(bin_idx, bc_ + nhits);

        htotSC += nhits; 

        for (const auto &hit : evSC->GetHits()) {
          h->hWavelength->Fill( hit->GetWavelength()); 
        }
      }
    }
    h->hSCNHits->Fill(htotSC); 

    //-------------------------------------------- Compute reconstructed energy
    if (hvisPix) {
      /*
       *if (iev%50 == 0) {
       *  printf("primary position: %g, %g, %g mm\n", 
       *      primary_pos[0], primary_pos[1], primary_pos[2]); 
       *}
       */
      int ibin = hvisPix->FindBin(primary_pos[0], primary_pos[1], primary_pos[2]); 
      double vis = hvisPix->GetBinContent(ibin); 

      double Erec = htotPix / (avgLY*vis*PixFillFactor*pdeSiPM);
      h->hERecoPix->Fill(Erec);
    }

    if (hvisSC) {
      int ibin = hvisSC->FindBin(primary_pos[0], primary_pos[1], primary_pos[2]); 
      double vis = hvisSC->GetBinContent(ibin); 

      double Erec = htotSC / (avgLY*vis*pdeSC);
      h->hERecoSC->Fill(Erec);
    }

    if (hvisSC && hvisPix) {
      int ibin_pix = hvisPix->FindBin(primary_pos[0], primary_pos[1], primary_pos[2]); 
      double vis_pix = hvisPix->GetBinContent(ibin_pix);  

      int ibin_sc = hvisSC->FindBin(primary_pos[0], primary_pos[1], primary_pos[2]); 
      double vis_sc = hvisSC->GetBinContent(ibin_sc); 

      double Erec = (htotPix+htotSC) / 
        (avgLY*(vis_pix*PixFillFactor*pdeSiPM + vis_sc*pdeSC) );

      h->hEReco->Fill(Erec); 
    }



  }

  printf("------------------------------------------ Drawing MC truth info\n");
  TCanvas* cVisibleEnergy = new TCanvas("cVisibleEnergy", "cVisibleEnergy", 
      0, 0, 800, 500);
  cVisibleEnergy->Divide(2, 1); 
  cVisibleEnergy->cd(1); 
  h->hvis->Draw("hist");
  cVisibleEnergy->cd(2); 
  h->hNPhotons->Draw("hist"); 
  TCanvas* cPosition = new TCanvas("cPosition", "cPosition", 0, 0, 1200, 500); 
  cPosition->Divide(3, 1); 
  for (int j=0; j<3; j++) {
    cPosition->cd(j+1); 
    h->hPosition[j]->Draw("hist"); 
  }
 

  printf("------------------------------------------ Drawing Nhits (hmax = %g)\n",
      hmax);
  TH2D* h2frame = new TH2D("h2frame", "Pix Readout", 700, -7e3, 7e3, 600, -3e3, +3e3);
  TCanvas* cPixN = new TCanvas("cPixN", "Pixel system readout - Nhits", 0, 0, 1500, 600);

  cPixN->cd(); 
  int imap = 0; 
  h2frame->DrawClone("axis"); 
  for (const auto &hmap : h->hPixNPhMap) {
    hmap.second->Scale(1./tree->GetEntries()); 
    if (hmax < hmap.second->GetMaximum()) 
      hmax = hmap.second->GetMaximum(); 
  }

  for (auto &hmap : h->hPixNPhMap) {
    hmap.second->GetZaxis()->SetRangeUser(0, 1.1*hmax); 
    if (imap == 0) hmap.second->Draw("colz0 same"); 
    else hmap.second->Draw("col0 same"); 
    imap++; 
  }
  auto txt_hmap = add_preliminary(0, 1); 
  txt_hmap->SetTextSize(30);
  txt_hmap->SetX(gStyle->GetPadLeftMargin()); 
  txt_hmap->SetY(1-gStyle->GetPadTopMargin()+0.02);
  txt_hmap->SetTextAlign(11); 
  txt_hmap->Draw(); 

  TCanvas* cNhits = new TCanvas("cNhits", "cNhits", 0, 0, 600, 600); 
  cNhits->cd(); 
  h->hPixNHits->Draw("hist");
  auto txt_nh = add_preliminary(0, 1); 
  txt_nh->Draw(); 

  TCanvas* cTime = new TCanvas("cTime", "cTime", 0, 0, 900, 600); 
  cTime->cd(); 
  h->hPixTHits->Draw("hist");
  auto txt_th = add_preliminary(1, 1); 
  txt_th->Draw(); 

  TCanvas* cWavelength = new TCanvas("cWavelength", "cWavelength", 0, 0, 600, 600); 
  cWavelength->cd(); 
  h->hWavelength->Draw("hist");

  return;
}

void PrintUsage() {
  printf("test_output usage:\n./test_output\n"); 
  printf("\t-i(--input) input_file\n"); 
  printf("\t-l(--list) input_list\n");
  printf("\t-o(--output) output_file\n");
  printf("\t-v(--visibility) visibility_file_map\n");
  printf("\t-h(--help) print usage\n\n"); 
  return; 
}

int main(int argc, char *argv[])
{
  const char* short_opts = "i:l:o:v:h";
  static struct option long_opts[6] = 
  {
    {"input"     , required_argument, 0, 'i'}, 
    {"list"      , required_argument, 0, 'l'}, 
    {"output"    , required_argument, 0, 'o'}, 
    {"visibility", required_argument, 0, 'v'}, 
    {"help"      , no_argument, 0, 'h'}, 
    {nullptr, no_argument, nullptr, 0}
  };

  int c, option_index; 

  const char* input_file_ = ""; 
  const char* input_list_ = ""; 
  const char* output_file_ = ""; 
  const char* visibility_file_ = ""; 

  while ( (c = getopt_long(argc, argv, short_opts, long_opts, &option_index)) != -1) {
    switch(c) {
      case 'i' : {
        input_file_ = optarg;
        printf("input_file: %s\n", input_file_);
        break;
      }
      case 'o' : 
      {
        output_file_ = optarg; 
        printf("output_file: %s\n", output_file_);
        break;
      }
      case 'l' :
      {
        input_list_ = optarg; 
        break;
      }
      case 'v' : 
      {
        visibility_file_ = optarg; 
        break;
      }
      case 'h':
      {
        PrintUsage();
        return 4;
        break;
      }
    }
  }

  TString input_file = input_file_; 
  TString output_file = output_file_; 
  TString input_list = input_list_; 
  TString visibility_file = visibility_file_; 

  TH3D* hvisPix = nullptr; 
  TH3D* hvisSC  = nullptr; 
  if (!visibility_file.IsNull()) {
    TFile* file = new TFile(visibility_file); 
    hvisPix = (TH3D*)file->Get("hvisPix")->Clone(); 
    hvisSC  = (TH3D*)file->Get("hvisSC")->Clone(); 
  }

  printf("input file: %s\n", input_file.Data()); 
  
  if (input_list.IsNull() && !input_file.IsNull()) {
    test_output(input_file.Data(), output_file.Data(), hvisPix, hvisSC); 
  } else if (!input_list.IsNull()) {
    merge_and_plot(input_list.Data(), output_file.Data(), hvisPix, hvisSC); 
  } else {
    printf("No valid input provided\n");
    PrintUsage(); 
  }
  return 0;
}
