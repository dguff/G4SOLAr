/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArBackgroundGeneratorAction.hh
 * @created     Wed Jan 11, 2023 11:40:12 CET
 */

#ifndef SLARBACKGROUNDGENERATORACTION_HH

#define SLARBACKGROUNDGENERATORACTION_HH

#include <map>
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4VPhysicalVolume.hh"
#include "globals.hh"

namespace bxdecay0_g4 {
  class PrimaryGeneratorAction; 
}
class SLArBulkVertexGenerator;

namespace background {
  struct SLArBackgroundInfo {
    G4String fIsotope; 
    G4String fVolume; 
    double fRate; 

    inline SLArBackgroundInfo(const char* iso, const char* vol, double rate)
      : fIsotope(iso), fVolume(vol), fRate(rate) {}
    inline SLArBackgroundInfo(const SLArBackgroundInfo& right) {
      fIsotope = right.fIsotope; 
      fVolume  = right.fVolume; 
      fRate    = right.fRate;
    }
  }; 
}

class SLArBackgroundGeneratorAction : public G4VUserPrimaryGeneratorAction 
{
  public: 
    SLArBackgroundGeneratorAction(); 
    SLArBackgroundGeneratorAction(const char* bkg_cfg_path); 
    virtual ~SLArBackgroundGeneratorAction(); 

    void BuildBackgroundTable(const char* bkg_cfg_path); 
    virtual void GeneratePrimaries(G4Event* ev); 
    G4double SetBackgroundVolume(SLArBulkVertexGenerator*, G4String);

    inline void LoadPrimaryGenerator(G4VUserPrimaryGeneratorAction* primary_gen)
      {fPrimaryGenAction = primary_gen;}
    inline void LoadBackgroundGenerator(bxdecay0_g4::PrimaryGeneratorAction* bkg_gen)
      {fDecay0GenAction = bkg_gen;} 
    inline void LoadVertexGenerator(SLArBulkVertexGenerator* vtx_gen) 
      {fBulkVtxGen = vtx_gen;}

    void PrintBackgroundModel(); 

  private: 
    std::map<size_t, background::SLArBackgroundInfo> fBkgModel;
    G4VUserPrimaryGeneratorAction* fPrimaryGenAction; 
    bxdecay0_g4::PrimaryGeneratorAction* fDecay0GenAction; 
    SLArBulkVertexGenerator* fBulkVtxGen; 
    double fTime0; 
    double fTime1;
};

#endif /* end of include guard SLARBACKGROUNDGENERATORACTION_HH */

