/**
 * @author      : Claudio Savarese (University of Manchester), Daniele Guffanti (University and INFN Milano-Bicocca)
 * @file        : process_mc_truth.cc
 * @created     : Friday May 03, 2024 12:40:41 CEST
 */

#include <iostream>
#include <cmath>
#include <vector>
#include <getopt.h>
#include <string>
#include <algorithm>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TGraph.h>
#include <TRandom3.h>

#include <event/SLArMCEvent.hh>
#include <event/SLArMCPrimaryInfo.hh>

using namespace std;

struct cluster {
  double x;
  double y;
  double z;
  double ene;
  double ndep;
};

double distXY(double x1, double y1, double x2, double y2){
  return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}

double distZ(double z1, double z2){
  return abs(z1-z2);
}

// Convert energy deposits to Nph (E-field@600V/cm)
int OD_ene_to_nph(double ene, const TGraph* g, TRandom3 r){
  return r.Poisson(g->Eval(ene)*ene);
}

// Convert Nph in OD to PEs
int nph_to_npe(int nph, float effective_pde, TRandom3 r){
  return r.Binomial(nph,effective_pde);
}

int process_mc_truth(const TString mc_file_path, const TString output_file_path) 
{
  // Hard coded paramters to put in the function arguments
  float geom_eff_vuv = 0.035*(2.*2-1)/(2*2*2+2*1.6*4); // 3.5% SiPM coverage on pixel tiles, divided by total inner surface of the OD.
  float geom_eff_vis = 0.035*(0.5*(2.*2+2*1.6*4)/(2.*2*2+2*1.6*4))*((2.*2-1)/(2*2*2+2*1.6*4))+0.5*1./(2*2*2+2*1.6*4); // same as VUV, multiplied by WLS prob.
  float geom_eff_vuv_id_top = 0*(1.*1)/(1.*1*2+1.*0.2*4);
  float geom_eff_vuv_id_bottom = 0*0.25*(1.*1)/(1.*1*2+1.*0.2*4);
  float geom_eff_vis_id_top = 0.5*(1.*0.2*4+0.5*(1+1))/(1.*1*2+1.*0.2*4);
  float geom_eff_vis_id_bottom = 0.5*0.25*(1.*0.2*4+0.5*(1))/(1.*1*2+1.*0.2*4);
  float pde_vuv = 0.14;
  float pde_vis = 0.45;
  float effective_pde_od = geom_eff_vuv*pde_vuv+geom_eff_vis*pde_vis;
  float effective_pde_id_top = geom_eff_vuv_id_top*pde_vuv+geom_eff_vis_id_top*pde_vis;
  float effective_pde_id_bottom = geom_eff_vuv_id_bottom*pde_vuv+geom_eff_vis_id_bottom*pde_vis;

  TFile* mc_file = new TFile(mc_file_path); 
  TTree* mc_tree = mc_file->Get<TTree>("EventTree");
  TFile *fout = new TFile(output_file_path,"RECREATE");

  const TString SOLAIRE_DATA_DIR = std::getenv("SOLAIRE_DATA");

  TRandom3 r;
  TGraph *gER_QY_nph_600V = new TGraph(SOLAIRE_DATA_DIR + "ER_ly_600V.csv","%lg %lg",",");
  gER_QY_nph_600V->SetBit(TGraph::kIsSortedX);
  TGraph *gER_QY_nph_0V = new TGraph(SOLAIRE_DATA_DIR + "ER_ly_0V.csv","%lg %lg",",");
  gER_QY_nph_0V->SetBit(TGraph::kIsSortedX);

  TH1D* hPrimaryGammaEnergy = new TH1D("hPrimaryGammaEnergy", 
      "Primary #gamma energy;Energy [MeV];Counts", 
      3500, 0, 3.5); 
  TH2D* hEdepProfile_XY = new TH2D("hEdepProfile_XY", "#it{E}_{dep} profile;x [mm];y [mm]", 
      120, -1200, 1200, 120, -1200, 1200); 
  TH2D* hEdepProfile_RZ = new TH2D("hEdepProfile_RZ", "#it{E}_{dep} profile;r [mm];z [mm]", 
      100, 0, 2000, 120, -1200, 1200); 
  TH2D* hEdepProfile_XZ = new TH2D("hEdepProfile_XZ", "#it{E}_{dep} profile;x [mm];z [mm]", 
      120, -1200, 1200, 120, -1200, 1200); 
  TH1D* hEdepID_NoCuts = new TH1D("hEdepID_NoCuts","Energy deposits in ID; Energy [keV]; Counts",
      500,0,5000);
  TH1D* hEdepID_NoCuts_Zoom0200 = new TH1D("hEdepID_NoCuts_Zoom0200","Energy deposits in ID; Energy [keV]; Counts",
      800,0,200);
  TH1D* hEdepID_NoCuts_Zoom020 = new TH1D("hEdepID_NoCuts_Zoom020","Energy deposits in ID; Energy [keV]; Counts",
      800,0,20);
  TH1D* nPrimaries = new TH1D("nPrimaries","Number of Primary particles; nPrimaries; Counts",
      10,0,10);
  TH1D* nTrajectories = new TH1D("nTrajectories","Number of Trajectories; nTrajectories; Counts",
    50,0,50);
  TH1D* nEdep = new TH1D("nEdep","Number of Energy deposits; nEdep; Counts",
      100,0,100);
  TH2D* hTP = new TH2D("hTP", ";Energy deposits; Trajectories", 
      200, 0, 200, 50, 0, 50); 
  TH2D* hCP = new TH2D("hCP", ";Energy deposits; Clusters", 
      200, 0, 200, 200, 0, 200); 
  TH2D* h_EPrimary_nClus = new TH2D("h_EPrimary_nClus", ";Primary Energy [keV];# of Clusters", 
      1000, 0, 5000, 100, 0, 100); 
  TH1D* hClusterEne_NoCuts = new TH1D("hClusterEne_NoCuts","Clusters Energy in ID; Energy [keV]; Counts",
      800,0,200);
  TH2D* hClusterProfile_RZ_NoCuts = new TH2D("hClusterProfile_RZ_NoCuts","Clusters Profile; r [mm];z [mm]",
      200,0,1000,200,-1000,1000);
  TH2D* hClusterProfile_XY_NoCuts = new TH2D("hClusterProfile_XY_NoCuts","Clusters Profile; x [mm];y [mm]",
      200,-1000,1000,200,-1000,1000);
  TH1D* hClusterEne_nclus1 = new TH1D("hClusterEne_nclus1","Clusters Energy in ID; Energy [keV]; Counts",
      800,0,200);
  TH2D* hClusterProfile_RZ_nclus1 = new TH2D("hClusterProfile_RZ_nclus1","Clusters Profile; r [mm];z [mm]",
      200,0,1000,200,-1000,1000);
  TH2D* hClusterProfile_XY_nclus1 = new TH2D("hClusterProfile_XY_nclus1","Clusters Profile; x [mm];y [mm]",
      200,-1000,1000,200,-1000,1000);
  TH2D* hEdepProfile_RZ_nclus1 = new TH2D("hEdepProfile_RZ_nclus1", "#it{E}_{dep} profile;r [mm];z [mm]", 
      120, 0, 1200, 120, -1200, 1200); 
  TH2D* hEdepProfile_XY_nclus1 = new TH2D("hEdepProfile_XY_nclus1", "#it{E}_{dep} profile;x [mm];y [mm]", 
      120, -1200, 1200, 120, -1200, 1200); 
  TH1D* hOD_Ene = new TH1D("hOD_Ene","Deposited Energy in OD; Energy [keV]; Counts",
      250,0,2500);
  TH1D* hOD_nph = new TH1D("hOD_nph","Photons in OD; nPH; Counts",
      500,0,5e5);
  TH2D* hOD_ene_nph = new TH2D("hOD_ene_nph","Photons vs Energy Deposits in OD; Energy [keV]; Photons",
      250,0,2500, 250,0,2.5e5);
  TH1D* hOD_nPE = new TH1D("hOD_nPE","PhotoElectrons in OD; nPE; Counts",
      5000,0,5e3);
  TH2D* hOD_nph_npe = new TH2D("hOD_nph_npe","Photoelectrons vs Photons in OD; nPH; nPE",
      250,0,2.5e5, 1500,0,1.5e3);
  TH2D* hOD_ene_npe = new TH2D("hOD_ene_npe","Photoelectrons vs Energy Deposits in OD; Energy [keV]; nPE",
      250,0,2500, 500,0,5e3);
  TH1D* hCluster_Ene_nclus1_ODnpe20 = new TH1D("hClusterEne_nclus1_ODnpe20","Clusters Energy in ID; Energy [keV]; Counts",
      800,0,20);
  TH2D* hClusterProfile_RZ_nclus1_ODnpe20 = new TH2D("hClusterProfile_RZ_nclus1_ODnpe20","Clusters Profile; r [mm];z [mm]",
      200,0,1000,200,-1000,1000);
  TH2D* hClusterProfile_XY_nclus1_ODnpe20 = new TH2D("hClusterProfile_XY_nclus1_ODnpe20","Clusters Profile; x [mm];y [mm]",
      200,-1000,1000,200,-1000,1000);
  TH1D* hID_buffer_top_Ene = new TH1D("hID_buffer_top_Ene","Deposited Energy in ID buffer top; Energy [keV]; Counts",
      250,0,2500);
  TH1D* hID_buffer_top_nph = new TH1D("hID_buffer_top_nph","Photons in ID buffer top; nPH; Counts",
      500,0,5e5);
  TH2D* hID_buffer_top_ene_nph = new TH2D("hOD_buffer_top_ene_nph","Photons vs Energy Deposits in ID buffer top; Energy [keV]; Photons",
      250,0,2500, 250,0,2.5e5);
  TH1D* hID_buffer_top_nPE = new TH1D("hID_buffer_top_nPE","PhotoElectrons in ID buffer top; nPE; Counts",
      1000,0,1e4);
  TH2D* hID_buffer_top_nph_npe = new TH2D("hID_buffer_top_nph_npe","Photoelectrons vs Photons in ID buffer top; nPH; nPE",
      250,0,2.5e5, 1500,0,1.5e3);
  TH2D* hID_buffer_top_ene_npe = new TH2D("hID_buffer_top_ene_npe","Photoelectrons vs Energy Deposits in ID buffer top; Energy [keV]; nPE",
      250,0,2500, 1000,0,1e4);
  TH1D* hID_buffer_bottom_Ene = new TH1D("hID_buffer_bottom_Ene","Deposited Energy in ID buffer bottom; Energy [keV]; Counts",
      250,0,2500);
  TH1D* hID_buffer_bottom_nph = new TH1D("hID_buffer_bottom_nph","Photons in ID buffer top; nPH; Counts",
      500,0,5e5);
  TH2D* hID_buffer_bottom_ene_nph = new TH2D("hOD_buffer_bottom_ene_nph","Photons vs Energy Deposits in ID buffer top; Energy [keV]; Photons",
      250,0,2500, 250,0,2.5e5);
  TH1D* hID_buffer_bottom_nPE = new TH1D("hID_buffer_bottom_nPE","PhotoElectrons in ID buffer top; nPE; Counts",
      1000,0,1e4);
  TH2D* hID_buffer_bottom_nph_npe = new TH2D("hID_buffer_bottom_nph_npe","Photoelectrons vs Photons in ID buffer top; nPH; nPE",
      250,0,2.5e5, 1500,0,1.5e3);
  TH2D* hID_buffer_bottom_ene_npe = new TH2D("hID_buffer_bottom_ene_npe","Photoelectrons vs Energy Deposits in ID buffer top; Energy [keV]; nPE",
      250,0,2500, 1000,0,1e4);
  TH1D* hCluster_Ene_nclus1_ODnpe20_IDbtbnpe50 = new TH1D("hClusterEne_nclus1_ODnpe20_IDbtbnpe50","Clusters Energy in ID; Energy [keV]; Counts",
      800,0,20);
  TH2D* hClusterProfile_RZ_nclus1_ODnpe20_IDbtbnpe50 = new TH2D("hClusterProfile_RZ_nclus1_ODnpe20_IDbtbnpe50","Clusters Profile; r [mm];z [mm]",
      200,0,1000,200,-1000,1000);
  TH2D* hClusterProfile_XY_nclus1_ODnpe20_IDbtbnpe50 = new TH2D("hClusterProfile_XY_nclus1_ODnpe20_IDbtbnpe50","Clusters Profile; x [mm];y [mm]",
      200,-1000,1000,200,-1000,1000);


  SLArMCEvent* ev = nullptr; 
  mc_tree->SetBranchAddress("MCEvent", &ev); 

  const Long64_t n_entries = mc_tree->GetEntries();

  // Set event counters and event global info
  int n_primaries = 0;
  int n_trajectories = 0;
  int n_edeps = 0;
  double event_ene = 0;
  double OD_energy = 0;
  double OD_photons = 0;
  int OD_npe = 0;
  double ID_buffer_top_energy = 0;
  double ID_buffer_top_photons = 0;
  int ID_buffer_top_npe = 0;
  double ID_buffer_bottom_energy = 0;
  double ID_buffer_bottom_photons = 0;
  int ID_buffer_bottom_npe = 0;
  bool OD_veto = 0;
  bool top_buffer_veto = 0;
  bool bottom_buffer_veto = 0;
  double cl_x, cl_y, cl_z, cl_r;

  // Set clusters 
  vector<cluster> v_cl;
  v_cl.clear();
  int nclusters = 0;

  for (Long64_t i = 0; i < n_entries; i++) {
    mc_tree->GetEntry(i); 

    // get the vector contaning the primary particles (here is just 1)
    auto& primaries = ev->GetPrimaries();

    cluster cl_temp;
    cl_temp.x = -99999;
    cl_temp.y = -99999;
    cl_temp.z = -99999;
    cl_temp.ene = -99999;
    cl_temp.ndep = -99999;
    v_cl.push_back(cl_temp);
    OD_energy = 0;
    OD_photons = 0;
    OD_npe = 0;
    ID_buffer_top_energy = 0;
    ID_buffer_top_photons = 0;
    ID_buffer_top_npe = 0;
    ID_buffer_bottom_energy = 0;
    ID_buffer_bottom_photons = 0;
    ID_buffer_bottom_npe = 0;
    OD_veto = 0;
    top_buffer_veto = 0;
    bottom_buffer_veto = 0;
    cl_x = 0;
    cl_y = 0;
    cl_z = 0;
    cl_r = 0;

    for (const auto& p : primaries) {
      n_primaries++;
      hPrimaryGammaEnergy->Fill(p.GetEnergy());
      event_ene = p.GetEnergy()*1000; // in keV

      // access the trajectoris of the primary itself and of all its daughters
      const auto& trajectories = p.GetConstTrajectories();

      for (const auto& t : trajectories) {
        // access the step points
        n_trajectories++;
        const auto& steps = t->GetConstPoints();
        for (const auto& p : steps) {
            // here you can access the step-level information
            // p.fX, p.fZ, p.fY; step-point coordinates [mm]  
            // p.fLAr [bool] if the step was in LAr or not
            // p.fEdep; Energy loss in step [MeV]
            // p.fCopy; Copy Nr of the volume
            // p.fKEnergy; Current Kinetic Energy of the particle
            // p.Nel; Nr of ionization electrons produced
            // p.fNph; Nr of scintillation produces
          // Control plots
          if ( p.fLAr && p.fEdep > 0) {
              hEdepProfile_XY->Fill(p.fX, p.fZ, p.fEdep); 
              hEdepProfile_RZ->Fill(sqrt(pow(p.fX,2)+pow(p.fZ,2)), p.fY, p.fEdep); 
              if(abs(p.fZ) < 1500)
                  hEdepProfile_XZ->Fill(p.fX, p.fY, p.fEdep); 
          }
          // Clustering in ID volume
          if ( p.fLAr && p.fEdep > 0) {
              n_edeps++;
              if(abs(p.fX) < 500 && abs(p.fZ) < 500 && abs(p.fY) < 675) {
                  hEdepID_NoCuts->Fill(p.fEdep*1000);
                  hEdepID_NoCuts_Zoom0200->Fill(p.fEdep*1000);
                  hEdepID_NoCuts_Zoom020->Fill(p.fEdep*1000);
              // Construct clusters from Energy deposits
              if(distXY(p.fX,p.fZ, v_cl.back().x, v_cl.back().y) < 10 && distZ(p.fY, v_cl.back().z) < 3){
                  v_cl.back().x = (p.fX*p.fEdep +  v_cl.back().x*v_cl.back().ene)/(p.fEdep+v_cl.back().ene);
                  v_cl.back().y = (p.fZ*p.fEdep +  v_cl.back().y*v_cl.back().ene)/(p.fEdep+v_cl.back().ene);
                  v_cl.back().z = (p.fY*p.fEdep +  v_cl.back().z*v_cl.back().ene)/(p.fEdep+v_cl.back().ene);
                  v_cl.back().ene += p.fEdep*1000;
                  v_cl.back().ndep ++;
              }
              else {
                  if(v_cl.size() >= 1 && v_cl.back().ene >= 0) {
                      cluster cl1;
                      v_cl.push_back(cl1);
                      v_cl.back().ene = p.fEdep*1000;
                      v_cl.back().x = p.fX;
                      v_cl.back().y = p.fZ;
                      v_cl.back().z = p.fY;
                      v_cl.back().ndep = 1;
                      nclusters++;
                  }
                  else if(v_cl.size() == 1 && v_cl.back().ene < 0){
                      nclusters++;
                      v_cl.back().ene = p.fEdep*1000;
                      v_cl.back().x = p.fX;
                      v_cl.back().y = p.fZ;
                      v_cl.back().z = p.fY;
                      v_cl.back().ndep = 1;
                  }
                  else cout << "Warning!!! Clusters counting problem!  Vector size: " << v_cl.size() << "   Counter: " << nclusters << endl;
              }
            }
            // Energy deposited in the Outer Detector
            else if (abs(p.fX) >= 500 || abs(p.fZ) >= 500){
                OD_energy += p.fEdep*1000;
                OD_photons += OD_ene_to_nph(p.fEdep*1000, gER_QY_nph_600V, r);
            }
            else if (abs(p.fX) < 500 && abs(p.fZ) < 500 && abs(p.fY) >= 675){
                if(p.fY>0){
                    ID_buffer_top_energy += p.fEdep*1000;;
                    ID_buffer_top_photons += OD_ene_to_nph(p.fEdep*1000, gER_QY_nph_0V, r);;
                } 
                else {               
                    ID_buffer_bottom_energy += p.fEdep*1000;;
                    ID_buffer_bottom_photons += OD_ene_to_nph(p.fEdep*1000, gER_QY_nph_0V, r);;
                } 
            }
            else cout << "Something's strange in the segmentation conditions!" << endl;
          }
          // cout << i << "  " << n_edeps << "  " << p.fX << "  " << p.fZ << "  " << p.fY << "  " << p.fEdep*1000 << endl; 
        }
      }
    }
    if(n_primaries>0) nPrimaries->Fill(n_primaries);
    if(n_trajectories>0) nTrajectories->Fill(n_trajectories);
    if(n_edeps>0) nEdep->Fill(n_edeps);
    if(n_edeps>0 && n_trajectories>0) hTP->Fill(n_edeps,n_trajectories);
    if(n_edeps>0) hCP->Fill(n_edeps, nclusters);
    if(OD_energy>0){ 
        hOD_Ene->Fill(OD_energy);
        // OD Energy to Scintillation
        if(OD_photons > 0){
            hOD_nph->Fill(OD_photons);
            hOD_ene_nph->Fill(OD_energy,OD_photons);
        }
        OD_npe = nph_to_npe(OD_photons, effective_pde_od, r);
        hOD_nPE->Fill(OD_npe);
        hOD_nph_npe->Fill(OD_photons, OD_npe);
        hOD_ene_npe->Fill(OD_energy, OD_npe);
    }
    if(ID_buffer_top_energy>0){ 
        hID_buffer_top_Ene->Fill(ID_buffer_top_energy);
        // OD Energy to Scintillation
        if(ID_buffer_top_photons > 0){
            hID_buffer_top_nph->Fill(ID_buffer_top_photons);
            hID_buffer_top_ene_nph->Fill(ID_buffer_top_energy,ID_buffer_top_photons);
        }
        if(ID_buffer_top_photons != 0) ID_buffer_top_npe = nph_to_npe(ID_buffer_top_photons, effective_pde_id_top, r);
        hID_buffer_top_nPE->Fill(ID_buffer_top_npe);
        hID_buffer_top_nph_npe->Fill(ID_buffer_top_photons, ID_buffer_top_npe);
        hID_buffer_top_ene_npe->Fill(ID_buffer_top_energy, ID_buffer_top_npe);
    }
    if(ID_buffer_bottom_energy>0){ 
        hID_buffer_bottom_Ene->Fill(ID_buffer_bottom_energy);
        // OD Energy to Scintillation
        if(ID_buffer_bottom_photons > 0){
            hID_buffer_bottom_nph->Fill(ID_buffer_bottom_photons);
            hID_buffer_bottom_ene_nph->Fill(ID_buffer_bottom_energy,ID_buffer_bottom_photons);
        }
        if(ID_buffer_bottom_photons != 0) ID_buffer_bottom_npe = nph_to_npe(ID_buffer_bottom_photons, effective_pde_id_bottom, r);
        hID_buffer_bottom_nPE->Fill(ID_buffer_bottom_npe);
        hID_buffer_bottom_nph_npe->Fill(ID_buffer_bottom_photons, ID_buffer_bottom_npe);
        hID_buffer_bottom_ene_npe->Fill(ID_buffer_bottom_energy, ID_buffer_bottom_npe);
    }
    // Clusters Analysis
    nclusters = v_cl.size();
    for(int j = 0; j < nclusters; j++){
      //if(nclusters == 1 && v_cl[j].ene >= 0) cout << i << "  " << j <<"  " << v_cl[j].x << "  " << v_cl[j].y << "  " << v_cl[j].z << "  " << v_cl[j].ene << endl;
      hClusterEne_NoCuts->Fill(v_cl[j].ene);
      cl_r = sqrt(v_cl[j].x*v_cl[j].x+v_cl[j].y*v_cl[j].y);
      cl_x = v_cl[j].x;
      cl_y = v_cl[j].y;
      cl_z = v_cl[j].z;
      hClusterProfile_RZ_NoCuts->Fill(cl_r,cl_z);
      hClusterProfile_XY_NoCuts->Fill(cl_x,cl_y);
      // Multiplicity cut
    }
    if(nclusters == 1 && v_cl[0].ene >= 0){
        cl_r = sqrt(v_cl[0].x*v_cl[0].x+v_cl[0].y*v_cl[0].y);
        cl_x = v_cl[0].x;
        cl_y = v_cl[0].y;
        cl_z = v_cl[0].z;
        hClusterEne_nclus1->Fill(v_cl[0].ene);
        hClusterProfile_RZ_nclus1->Fill(cl_r,cl_z);
        hClusterProfile_XY_nclus1->Fill(cl_x,cl_y);
        for (const auto& p : primaries) {
            const auto& trajectories = p.GetConstTrajectories();
            for (const auto& t : trajectories) {
                const auto& steps = t->GetConstPoints();
                for (const auto& p : steps) {
                    if ( p.fLAr && p.fEdep > 0) {
                        hEdepProfile_RZ_nclus1->Fill(sqrt(pow(p.fX,2)+pow(p.fZ,2)),p.fY);
                        hEdepProfile_XY_nclus1->Fill(p.fX,p.fZ);
                    }
                }
            }
        }
        // OD Anti-coincidence cut
        if(OD_npe <= 20){
            hCluster_Ene_nclus1_ODnpe20->Fill(v_cl[0].ene);
            hClusterProfile_RZ_nclus1_ODnpe20->Fill(cl_r,cl_z);
            hClusterProfile_XY_nclus1_ODnpe20->Fill(cl_x,cl_y);
            // ID Buffer Anti-coincidence cut
            if(ID_buffer_top_npe <= 50 && ID_buffer_bottom_npe <= 50){
                hCluster_Ene_nclus1_ODnpe20_IDbtbnpe50->Fill(v_cl[0].ene);
                hClusterProfile_RZ_nclus1_ODnpe20_IDbtbnpe50->Fill(cl_r,cl_z);
                hClusterProfile_XY_nclus1_ODnpe20_IDbtbnpe50->Fill(cl_x,cl_y);
        }
      }
    }
    h_EPrimary_nClus->Fill(event_ene,nclusters);

    // Reset Counters
    n_primaries = 0;
    n_trajectories = 0;
    n_edeps = 0;
    nclusters = 0;
    v_cl.clear();
    event_ene = 0;
    OD_energy = 0;
    OD_photons = 0;
    OD_npe = 0;
    ID_buffer_top_energy = 0;
    ID_buffer_top_photons = 0;
    ID_buffer_top_npe = 0;
    ID_buffer_bottom_energy = 0;
    ID_buffer_bottom_photons = 0;
    ID_buffer_bottom_npe = 0;
    OD_veto = 0;
    top_buffer_veto = 0;
    bottom_buffer_veto = 0;
    // Event counter
    if(i%500000 == 0) cout << "Event ID: " << i << endl;
  }

  gER_QY_nph_600V->Write();
  gER_QY_nph_0V->Write();
  //TCanvas *c0 = new TCanvas("cPrimaryGammaEnergy","Primary Gamma Energy", 1920, 1080); 
  //hPrimaryGammaEnergy->Draw("hist");  
  hPrimaryGammaEnergy->Write();
  //c0->Write();
  //TCanvas *c1 = new TCanvas("cEdepProfile_XY","XY deposits profile", 1080, 1080);
  //hEdepProfile_XY->Draw("colz"); 
  hEdepProfile_XY->Write();
  //c1->Write();
  //TCanvas *c2 = new TCanvas("cEdepProfile_RZ","RZ deposits profile", 1080, 1080);
  //hEdepProfile_RZ->Draw("colz"); 
  hEdepProfile_RZ->Write();
  //c2->Write();
  //TCanvas *c3 = new TCanvas("cEdepProfile_XZ","XZ deposits profile", 1080, 1080);
  //hEdepProfile_XZ->Draw("colz"); 
  hEdepProfile_XZ->Write();
  //c3->Write();
  hEdepID_NoCuts->Write();
  hEdepID_NoCuts_Zoom0200->Write();
  hEdepID_NoCuts_Zoom020->Write();
  nPrimaries->Write();
  nTrajectories->Write();
  nEdep->Write();
  hTP->Write();
  hCP->Write();
  h_EPrimary_nClus->Write();
  hClusterEne_NoCuts->Write();
  hClusterProfile_RZ_NoCuts->Write();
  hClusterProfile_XY_NoCuts->Write();
  hClusterEne_nclus1->Write();
  hClusterProfile_RZ_nclus1->Write();
  hClusterProfile_XY_nclus1->Write();
  hEdepProfile_RZ_nclus1->Write();
  hEdepProfile_XY_nclus1->Write();
  hOD_Ene->Write();
  hOD_nph->Write();
  hOD_ene_nph->Write();
  hOD_nPE->Write();
  hOD_nph_npe->Write();
  hOD_ene_npe->Write();
  hID_buffer_top_Ene->Write();
  hID_buffer_top_nph->Write();
  hID_buffer_top_ene_nph->Write();
  hID_buffer_top_nPE->Write();
  hID_buffer_top_nph_npe->Write();
  hID_buffer_top_ene_npe->Write();
  hID_buffer_bottom_Ene->Write();
  hID_buffer_bottom_nph->Write();
  hID_buffer_bottom_ene_nph->Write();
  hID_buffer_bottom_nPE->Write();
  hID_buffer_bottom_nph_npe->Write();
  hID_buffer_bottom_ene_npe->Write();
  hCluster_Ene_nclus1_ODnpe20->Write();
  hClusterProfile_RZ_nclus1_ODnpe20->Write();
  hClusterProfile_XY_nclus1_ODnpe20->Write();
  hCluster_Ene_nclus1_ODnpe20_IDbtbnpe50->Write();
  hClusterProfile_RZ_nclus1_ODnpe20_IDbtbnpe50->Write();
  hClusterProfile_XY_nclus1_ODnpe20_IDbtbnpe50->Write();
  fout->Close();
  mc_file->Close();
  return 0;
}

void print_usage() {
  printf("SOLAIRE process_mc_truth: basic event clustering and scintillation evaluation\n"); 
  printf("Usage: "); 
  printf("process_mc_truth\n"); 
  printf("\t[--input     | -i] input_simulatin_file\n"); 
  printf("\t[--output    | -o] output_hit_file\n"); 
  printf("\t[--help      | -h] Show this message\n\n");

  exit( EXIT_SUCCESS );
};


int main (int argc, char *argv[]) {
     const char* short_opts = "i:o:h";
   static struct option long_opts[7] = 
   {
     {"input", required_argument, 0, 'i'}, 
     {"output", required_argument, 0, 'o'}, 
     {"help", no_argument, 0, 'h'}, 
     {nullptr, no_argument, nullptr, 0}
   };

  int c, option_index; 

  TString input_file_path = ""; 
  TString output_file_path = ""; 

  while ( (c = getopt_long(argc, argv, short_opts, long_opts, &option_index)) != -1) {
    switch(c) {
      case 'i' :
        input_file_path = optarg;
        printf("Monte Carlo input file: %s\n", input_file_path.Data());
        break;
      case 'o' :
        output_file_path = optarg;
        printf("hit output file: %s\n", output_file_path.Data());
        break;
      case 'h' : 
        print_usage(); 
        exit( EXIT_SUCCESS ); 
        break;
      case '?' : 
        printf("solar_sim error: unknown flag %c\n", optopt);
        print_usage(); 
        exit( EXIT_FAILURE ); 
        break;
    }
  }

  process_mc_truth(input_file_path, output_file_path); 
  
  return 0;
}

