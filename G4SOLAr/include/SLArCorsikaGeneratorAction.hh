#ifndef SLARCORSIKAGENERATORACTION_HH
#define SLARCORSIKAGENERATORACTION_HH

#include <string>

#include "SLArBaseGenerator.hh"

#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"

#include "DBReader.hh"
#include "detector.hh"
#include "EParticle.hh"
#include "EShower.hh"
#include "EHandler.hh"


namespace gen{

  class SLArCorsikaGeneratorAction : public SLArBaseGenerator
  {

  public:    

    // Read configuration file
    struct CorsikaConfig_t {
      G4String corsika_db_dir {};
    };

    // ----- Constructors -----
    SLArCorsikaGeneratorAction(const G4String label);
    ~SLArCorsikaGeneratorAction();

    G4String GetGeneratorType() const override {return "corsika";}
    EGenerator GetGeneratorEnum() const override {return kCorsika;}

    G4String WriteConfig() const override;
    void Configure(const rapidjson::Value &config) override;

    void GeneratePrimaries(G4Event *ev);

  protected:
    CorsikaConfig_t fConfig;



  }; // Generator action

} // Namespace gen



#endif // End of header guard
