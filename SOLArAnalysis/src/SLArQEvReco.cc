/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArQEvReco
 * @created     : mercoledì giu 29, 2022 08:33:24 CEST
 */

#include "SLArQEvReco.hh"
#include "Math/GenVector/RotationZYX.h"
#include "Math/Vector3Dfwd.h"
#include "TRandom3.h"
#include "TF1.h"

namespace slarq {

  SLArQEvReco::SLArQEvReco() : 
    fQev(nullptr), fPCA(nullptr) 
  {}

  SLArQEvReco::SLArQEvReco(SLArQReadout* qev) : 
    fQev(nullptr), fPCA(nullptr) 
  {
    LoadQEv(qev); 
  }


  SLArQEvReco::~SLArQEvReco()
  {
    delete fPCA;
  }

  void SLArQEvReco::PCA() {
    if (!fPCA) {
      fPCA = new SLArQBlipPCA(3); 
    }


    SLArQCluster* main_cluster = fQev->GetMaxCluster();
    auto hn_template =fQev->GetQHistN(); 
    THnSparse* hn_cluster = (THnSparse*)hn_template->Clone("hn_cluster");
    hn_cluster->Reset(); 
    main_cluster->set_cluster_hist(hn_cluster);

    std::vector<double> bw(3, 0.);
    std::vector<double> xbin(3, 0.); 
    printf("- hn_cluster has %i dimensions\n", hn_cluster->GetNdimensions()); 
    for (int k=0; k<3; k++) {
      bw[k] = hn_cluster->GetAxis(k)->GetBinWidth(10); 
    }

    THnIter* it = new THnIter(hn_cluster, true);
    Long64_t ib = it->Next(); 
    std::vector<int> idx(3, 0); 
    while ( ib >= 0 ) {
      double bc = hn_cluster->GetBinContent(ib, &idx[0]);   
      for (int i=0; i<(int)log10(bc); i++) {
        for (int k=0; k<3; k++) {
          xbin[k] = hn_cluster->GetAxis(k)->GetBinCenter(idx[k]) + bw[k]*(gRandom->Rndm() - 0.5);
        }
        fPCA->AddRow(&xbin[0]); 
      }
      ib = it->Next();
    }

    fPCA->MakePrincipals();

    delete hn_cluster;
  }

  THnBase* SLArQEvReco::RotateCluster(ROOT::Math::RotationZYX rot, ROOT::Math::XYZPointD anchor) 
  {
    SLArQCluster* main_cluster = fQev->GetMaxCluster();
    THnSparse* hn_cluster = (THnSparse*)fQev->GetQHistN()->Clone("hn_cluster");
    hn_cluster->Reset();
    THnSparseD* hn_cluster_rot = 
      (THnSparseD*)hn_cluster->Clone(Form("%s_rot", hn_cluster->GetName())); 
    for (int idim=0; idim<3; idim++) 
      hn_cluster_rot->GetAxis(idim)->SetRange(); 

    main_cluster->set_cluster_hist(hn_cluster);

    ROOT::Math::RotationZYX xrot = fPCA->GetRotation();
    ROOT::Math::XYZPointD xpoint; 
    ROOT::Math::XYZPointD xdelta; 

    Long64_t ii = 0; 
    TVectorD vxbin(3);
    std::vector<double> bw(3, .0); 
    for (int idim =0; idim < 3; idim++) {
      bw[idim] = hn_cluster->GetAxis(idim)->GetBinWidth(1); 
    }

    std::vector<int> idx(3, 0); 
    auto it2 = hn_cluster->CreateIter(true); 

    printf("blipfit hn entries: %g\n", hn_cluster->GetEntries());
    while ( (ii=it2->Next()) >= 0 ) {
      double bc = hn_cluster->GetBinContent(ii, &idx[0]); 
      xpoint.SetX(hn_cluster->GetAxis(0)->GetBinCenter(idx[0])); 
      xpoint.SetY(hn_cluster->GetAxis(1)->GetBinCenter(idx[1])); 
      xpoint.SetZ(hn_cluster->GetAxis(2)->GetBinCenter(idx[2])); 

      auto point = xpoint - anchor;

      auto rpoint = xrot.operator()(point);

      rpoint = rpoint + anchor; 

      double rpoint_[3] = {rpoint.x(), rpoint.y(), rpoint.z()};

      hn_cluster_rot->Fill(rpoint_, bc*0.01); 
      //printf("(%g, %g, %g) -> (%g, %g, %g)\n", 
      //xpoint.x(), xpoint.y(), xpoint.z(),
      //rpoint.x(), rpoint.y(), rpoint.z());
    }

    adjust_h_range(hn_cluster_rot, 0.); 
    
    return hn_cluster_rot; 
  }

  std::vector<TF1*>& SLArQEvReco::ClusterFit(THnBase* h) {
    for (auto &f : fProjFit) {delete f;} 
    fProjFit.resize(2);
    std::fill(fProjFit.begin(), fProjFit.end(), nullptr);

    TH2D* hxy = h->Projection(1,0);
    TH2D* hxz = h->Projection(2,0);

    TProfile* p1 = hxz->ProfileX("profxz",1,-1,"e");
    TProfile* p2 = hxy->ProfileX("profxy",1,-1,"e");

    fProjFit[0] = new TF1("f1", "pol1", hxz->GetXaxis()->GetXmin(), hxz->GetXaxis()->GetXmax()); 
    fProjFit[1] = new TF1("f2", "pol1", hxy->GetXaxis()->GetXmin(), hxy->GetXaxis()->GetXmax()); 

    double bw = hxy->GetXaxis()->GetBinWidth(1);
    if( find_direction(hxy, fVertex.x()) == 1) {
      p1->Fit(fProjFit[0],"QN0","",fVertex.x(),fVertex.x()+bw*6);
      p2->Fit(fProjFit[1],"QN0","",fVertex.x(),fVertex.x()+bw*6); 
    } else {
      p1->Fit(fProjFit[0],"QN0","",fVertex.x()-6*bw,fVertex.x());
      p2->Fit(fProjFit[1],"QN0","",fVertex.x()-6*bw,fVertex.x()); 
    }

    delete p1;
    delete p2;
    delete hxy;
    delete hxz;

    return fProjFit;
  }

  ROOT::Math::XYZVectorD SLArQEvReco::GetDirection(ROOT::Math::RotationZYX* rot) {
    ROOT::Math::XYZVectorD dir(0, 0, 0);
    if (fProjFit[0]->GetParameter(0) != 0 && fProjFit[1]->GetParameter(0) != 0){
      double a  = fProjFit[0]->GetParameter(0);
      double aa = fProjFit[1]->GetParameter(0);
      dir.SetXYZ(1,aa,a);
      dir = dir.Unit();
      if (rot) {
        dir = rot->operator()(dir);
      }
    }
    return dir;
  }

  double SLArQEvReco::GetCosAngle(ROOT::Math::XYZVectorD ref_dir, ROOT::Math::RotationZYX* rot) {
    double cos_theta = -3.14;
    ROOT::Math::XYZVectorD dir = GetDirection(rot);
    if(fProjFit[0]->GetParameter(0) != 0 && fProjFit[1]->GetParameter(0) != 0){
    cos_theta = dir.Dot(ref_dir); }

    return cos_theta;
  }

  void SLArQEvReco::SetVertex(double *xvertex) {
    fVertex.SetXYZ(xvertex[0], xvertex[1], xvertex[2]); 
    return; 
  }

  void SLArQEvReco::SetVertex(ROOT::Math::XYZPointD xvertex) {
    fVertex = xvertex; 
    return; 
  }

  int SLArQEvReco::find_direction(TH2* h2, double xpos) {
    int bpos = h2->GetXaxis()->FindBin(xpos); 
    double integral_left  = h2->Integral(1, bpos, 1, h2->GetNbinsY()); 
    double integral_right = h2->Integral(bpos, h2->GetNbinsX(), 1, h2->GetNbinsY()); 

    if (integral_left < integral_right) {return 1;}
    else {return 2;}
  }

  void SLArQEvReco::Reset() {
    fPCA->Clear();
    fQev = nullptr; 
  }


  SLArQBlipPCA::SLArQBlipPCA() : TPrincipal() 
  {}

  SLArQBlipPCA::SLArQBlipPCA(int N, const char* opt) : TPrincipal(N, opt)
  {}
  
  ROOT::Math::RotationZYX SLArQBlipPCA::GetRotation() {
    // "rotate track using PCA eigenvector matrix
    // compute euler angles:
    // sin θ = -R[3][1];
    // tan ψ =  R[3][2] / R[3][3]
    // tan φ =  R[2][1] / R[1][1]

    double theta = TMath::ASin(-fEigenVectors.operator()(2, 0)); 
    double psi   = TMath::ATan2(fEigenVectors.operator()(2, 1) / cos(theta), 
        fEigenVectors.operator()(2, 2) / cos(theta));
    double phi   = TMath::ATan2(fEigenVectors.operator()(1, 0) / cos(theta), 
        fEigenVectors.operator()(0, 0) / cos(theta));

    return ROOT::Math::RotationZYX(phi, theta, psi); 
  }
}


