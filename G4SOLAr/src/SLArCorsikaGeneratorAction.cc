/* ================ Corsika Gen ================= *
 * Created: 14-06-2024                            *
 * Author:  Jordan McElwee                        *
 * Email: mcelwee@lp2ib.in2p3.fr                  *
 *                                                *
 * Corsika generator action to apply corsika      *
 * events to each event.                          *
 *                                                *
 * Changelog:                                     *
 * ============================================== */


#include "SLArCorsikaGeneratorAction.hh"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

namespace gen {


// ***************************************************************************
// ***** CONFIGURE ***********************************************************


  // ----- Constructors -----
  SLArCorsikaGeneratorAction::SLArCorsikaGeneratorAction(const G4String label)
    : SLArBaseGenerator(label)
  {}

  // ----- Destructor -----
  SLArCorsikaGeneratorAction::~SLArCorsikaGeneratorAction()
  {}

  // ----- Configure -----
  void SLArCorsikaGeneratorAction::Configure(const rapidjson::Value &config)
  {    
    assert( config.HasMember("corsika_db_dir") );
    fConfig.corsika_db_dir = config["corsika_db_dir"].GetString();
  }

  G4String SLArCorsikaGeneratorAction::WriteConfig() const
  {
    G4String config_str = "";

    rapidjson::Document d;
    d.SetObject();
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    G4String gen_type = GetGeneratorType();

    d.AddMember("type" , rapidjson::StringRef(gen_type.data()), d.GetAllocator());
    d.AddMember("label", rapidjson::StringRef(fLabel.data()), d.GetAllocator());
    d.AddMember("corsika_db_dir", rapidjson::StringRef(fConfig.corsika_db_dir.data()), d.GetAllocator());

    d.Accept(writer);
    config_str = buffer.GetString();
    return config_str;
  }


// ***************************************************************************


// ***************************************************************************
// ***** CONFIGURE ***********************************************************

  void SLArCorsikaGeneratorAction::GeneratePrimaries(G4Event *ev)
  {
    std::cout << "Something random" << std::endl;
  }

// ***************************************************************************



}
