/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : make_solaire_background
 * @created     : Tuesday May 28, 2024 15:44:21 CEST
 */

#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>
#include <stdio.h>
#include <getopt.h>
#include <TApplication.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TH1D.h>
#include <TH2D.h>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <vector>

void print_usage() {
  printf("SOLAIRE make_solaire_background: basic event clustering and scintillation evaluation\n"); 
  printf("Usage: "); 
  printf("process_mc_truth\n"); 
  printf("\t[--input     | -i] input configuration file\n"); 
  printf("\t[--output    | -o] output_file\n"); 
  printf("\t[--help      | -h] Show this message\n\n");

  exit( EXIT_SUCCESS );
};

struct BackgroundComponent_t {
  TString fSource = {};
  Double_t fMass = {};
  TString fComponent = {}; 
  Double_t fActivity = {}; 
  Double_t fPropagation = {};
  TString fHistFilePath = {}; 
  std::vector<TH1D*> fHist = {}; 

  void get_hist(const std::vector<TString>& keys) {
    TFile hist_file(fHistFilePath); 
    TH1D* hNPrimaries =  hist_file.Get<TH1D>("nPrimaries"); 
    const Double_t n_simulated_events = hNPrimaries->GetEntries(); 
    for (const auto& key : keys) {
      TH1D* h = hist_file.Get<TH1D>(key); 
      h->SetDirectory(nullptr); 
      const Double_t integral = h->Integral(); 
      h->Scale( 1.0 / n_simulated_events ); 
      fHist.push_back( h ); 
    }
    hist_file.Close(); 
  }

  void print() const {
    printf("%s[%s]: mass: %.2f kg - activity: %g Bq/kq\n", 
        fSource.Data(), fComponent.Data(), fMass, fActivity); 
  }
};

void parse_config_file(const TString config_file_path, std::vector<BackgroundComponent_t>& backgrounds, std::vector<TString>& hist_keys) {

  FILE* config_file = fopen(config_file_path.Data(), "r"); 
  if (config_file == nullptr) {
    fprintf(stderr, "ERROR: Unable to open background model configuration file\n"); 
    exit( EXIT_FAILURE ); 
  }

  char readBuffer[65536]; 
  rapidjson::FileReadStream readStream(config_file, readBuffer, sizeof(readBuffer)); 

  rapidjson::Document d; 
  d.ParseStream<rapidjson::kParseCommentsFlag>( readStream ); 

  if ( d.HasMember("exposure") == false) {
    fprintf(stderr, "WARNING: No exposure given in background configuration. Assuming 1 year\n"); 
  }

  if (d.HasMember("hist_keys")) {
    const rapidjson::Value&  jkeys = d["hist_keys"]; 
    if (jkeys.IsArray() == false) {
      throw std::invalid_argument("ERROR: error in background_configuration_file: \"hist_keys\" must be an array!\n"); 
    }

    for (const auto& jk : jkeys.GetArray()) {
      TString str = jk.GetString(); 
      hist_keys.push_back( str ); 
    }
  }

  if (d.HasMember("sources") == false) {
    throw std::invalid_argument("ERROR: no background sources given in configuration!\n");
  }

  const rapidjson::Value& sources = d["sources"]; 
  if (sources.IsArray() == false) {
    throw std::invalid_argument("ERROR: \"sources\" in configuration file must be an array!\n");
  }

  for (const auto& s : sources.GetArray()) {
    if (s.HasMember("components") == false) {
      char msg[1024]; 
      sprintf(msg, "ERROR: %s must have at least one background component!\n", 
          s["name"].GetString());
      throw std::invalid_argument(msg); 
    }

    const auto& jcomps = s["components"].GetArray();

    for (const auto& jc : jcomps) {
      BackgroundComponent_t comp; 
      comp.fSource = s["name"].GetString(); 
      comp.fMass   = s["mass"].GetDouble(); 
      comp.fComponent = jc["name"].GetString(); 
      comp.fActivity = jc["activity"].GetDouble(); 
      comp.fPropagation = jc["propagation"].GetDouble(); 
      comp.fHistFilePath = jc["file"].GetString();

      comp.print();

      backgrounds.push_back( comp ); 
    }
  }
  

  fclose( config_file ); 
}


int main(int argc, char* argv[])
{

  const char* short_opts = "i:o:h";
  static struct option long_opts[7] = 
  {
    {"input", required_argument, 0, 'i'}, 
    {"output", required_argument, 0, 'o'}, 
    {"help", no_argument, 0, 'h'}, 
    {nullptr, no_argument, nullptr, 0}
  };

  int c, option_index; 

  TString config_file_path = ""; 
  TString output_file_path = ""; 

  while ( (c = getopt_long(argc, argv, short_opts, long_opts, &option_index)) != -1) {
    switch(c) {
      case 'i' :
        config_file_path = optarg;
        printf("background configuration file: %s\n", config_file_path.Data());
        break;
      case 'o' :
        output_file_path = optarg;
        printf("output file: %s\n", output_file_path.Data());
        break;
      case 'h' : 
        print_usage(); 
        exit( EXIT_SUCCESS ); 
        break;
      case '?' : 
        printf("make_solaire_background error: unknown flag %c\n", optopt);
        print_usage(); 
        exit( EXIT_FAILURE ); 
        break;
    }
  }

  TApplication app("make_solaire_background", &argc, argv); 

  std::vector<BackgroundComponent_t> backgrounds; 
  std::vector<TString> histo_keys; 
  try {
    parse_config_file(config_file_path, backgrounds, histo_keys);
  }
  catch (const std::exception& err) {
    std::cerr << err.what() << std::endl; 
    exit(EXIT_FAILURE); 
  }

 
  const double exposure_time = 2*3600*24*365; // 1 year hardcoded exposure

  std::vector<TH1D*> hTotal(histo_keys.size(), nullptr); 

  for (auto& bkg : backgrounds) {
    bkg.get_hist( histo_keys ); 

    double scaling = exposure_time * bkg.fMass * bkg.fActivity * bkg.fPropagation;

    size_t ihist = 0; 
    for (auto& h : bkg.fHist) {
      if (hTotal[ihist] == nullptr) {
        hTotal[ihist] = (TH1D*)h->Clone(Form("%s_total", h->GetName())); 
        hTotal[ihist]->Reset(); 
      }

      h->SetName(Form("%s%s_%s", bkg.fSource.Data(), bkg.fComponent.Data(), h->GetName()));
      h->Scale( scaling );  

      hTotal[ihist]->Add(h);
      ihist++; 
    }
  }

  size_t ihist = 0;
  for (const auto& key : histo_keys) {
    TCanvas* c = new TCanvas(key, key, 0, 0, 800, 600); 
    c->SetTicks(1, 1); c->SetGrid(1, 1); 
    hTotal[ihist]->SetMarkerStyle(20); 
    hTotal[ihist]->SetLineColor(kGray+1); 
    hTotal[ihist]->SetMarkerColor(kBlack); 
    hTotal[ihist]->Draw("pe"); 

    for (const auto& bkg : backgrounds) {
      bkg.fHist[ihist]->Draw("hist same"); 
    }

    ihist++; 
  }

  app.Run(); 

  return 0;
}

