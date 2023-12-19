// F. Bramati

#ifndef SLARGENIEGENERATORACTION_HH

#define SLARGENIEGENERATORACTION_HH

#include <string>
#include <map>
#include <vector>
#include "TTree.h"
#include "TObjString.h"

#include "G4VUserPrimaryGeneratorAction.hh"

class G4Event;
class G4PrimaryVertex;

struct GenieEvent {
    int Npart;
    int pdg[100];
    double vertex[4];
    double momentum[100][4];
    TObjString process;
    int status[100];
};

class SLArGenieGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    SLArGenieGeneratorAction();
    SLArGenieGeneratorAction(const std::string& input_file_name);
    virtual ~SLArGenieGeneratorAction();

    void load_tree(const char* ext_file_path);
    virtual void GeneratePrimaries(G4Event*) override;

  protected:
    int fIdx;
    TTree* fTree;
    //std::vector<GenieParticle> genie_particles;
    GenieEvent fGenieEvent;

};

#endif /* end of include guard SLARGENIEGENERATORACTION_HH */
