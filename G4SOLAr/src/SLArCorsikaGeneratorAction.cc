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

    // X Detector Dimensions
    if ( config.HasMember("corsika_det_xlow") )
      fConfig.corsika_det_x[0] = config["corsika_det_xlow"].GetDouble();
    else
      fConfig.corsika_det_x[0] = -4.;

    if ( config.HasMember("corsika_det_xhigh") )
      fConfig.corsika_det_x[1] = config["corsika_det_xhigh"].GetDouble();
    else
      fConfig.corsika_det_x[1] = 4.;

    // Y Detector Dimensions
    if ( config.HasMember("corsika_det_ylow") )
      fConfig.corsika_det_y[0] = config["corsika_det_ylow"].GetDouble();
    else
      fConfig.corsika_det_y[0] = -4.;

    if ( config.HasMember("corsika_det_yhigh") )
      fConfig.corsika_det_y[1] = config["corsika_det_yhigh"].GetDouble();
    else
      fConfig.corsika_det_y[1] = 4.;

    // Energy regime
    if ( config.HasMember("corsika_Elow") )
      fConfig.corsika_E[0] = config["corsika_Elow"].GetInt();
    else
      fConfig.corsika_E[0] = 50;

    if ( config.HasMember("corsika_Ehigh") )
      fConfig.corsika_E[1] = config["corsika_Ehigh"].GetInt();
    else
      fConfig.corsika_E[1] = 100000;

    // Spill Time
    if ( config.HasMember("corsika_dT") )
      fConfig.corsika_dT = config["corsika_dT"].GetDouble();
    else
      fConfig.corsika_dT = 2.;
    
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
    d.AddMember("corsika_det_xlow",  fConfig.corsika_det_x[0], d.GetAllocator());
    d.AddMember("corsika_det_xhigh",  fConfig.corsika_det_x[1], d.GetAllocator());
    d.AddMember("corsika_det_ylow",  fConfig.corsika_det_y[0], d.GetAllocator());
    d.AddMember("corsika_det_yhigh",  fConfig.corsika_det_y[1], d.GetAllocator());
    d.AddMember("corsika_Elow",  fConfig.corsika_E[0], d.GetAllocator());
    d.AddMember("corsika_Ehigh",  fConfig.corsika_E[1], d.GetAllocator());
    d.AddMember("corsika_dT", fConfig.corsika_dT, d.GetAllocator());
    
    d.Accept(writer);
    config_str = buffer.GetString();
    return config_str;
  }


// ***************************************************************************


// ***************************************************************************
// ***** GENERATE ************************************************************

  void SLArCorsikaGeneratorAction::GeneratePrimaries(G4Event *ev)
  {

    double xVals[2] = {-4.,4.};
    double yVals[2] = {-4.,4.};
    double EVals[2] = {50,100000};
    double spillT = 0.001;

    std::cout << fConfig.corsika_det_x[0] << " " << fConfig.corsika_det_x[1] << std::endl;
    //    exit(0);
    
    // Read the database using the reader class 
    DBReader *corsDB = new DBReader((fConfig.corsika_db_dir+"/cosmic_db_H_50_10000000.root").c_str());
   
    //Setup a detector level to read from 
    Detector *pdMuon = new Detector(xVals, yVals, EVals);
    pdMuon->ValidateRange();
    
    // Collect primaries from the detector. Should probably put this in the function
    std::vector<int> primary_gen = pdMuon->GetPrimaries(corsDB, 1.8E4, spillT);
    // Sort the vector lowest to highest in order to speed up searching (we can skip many
    // of the early events this way...
    sort(primary_gen.begin(), primary_gen.end());
  
    // Set the spill time for all EHandler objects
    EHandler::SetSpillT(spillT);

    // Create a handler for the shower
    EShower showerHandler(corsDB, pdMuon);
    //    showerHandler.CreateTree();

    // Create a handler for the particle 
    EParticle particleHandler(corsDB, pdMuon);
    //    particleHandler.CreateTree();

    // Loop through the showers selected and process them 
    for (int shower : primary_gen) {

      showerHandler.Process(shower);
      particleHandler.Process(shower, &showerHandler);
 
    }

    const std::vector<EParticle::Particle> &particles = particleHandler.GetParticles();
  

    G4ThreeVector vtx(0., 0., 0.);
    std::vector<G4PrimaryVertex*> primary_vertices;

    int particle_idx = 0;

    for (const auto &part : particles) {
      
      G4PrimaryParticle *incident_part = new G4PrimaryParticle(part.m_pdg,
							       part.m_mom[0]*1E3,
							       part.m_mom[1]*1E3,
							       part.m_mom[2]*1E3,
							       part.m_eK*1E3);
      vtx.set(part.m_vtx[0], part.m_vtx[1], 4.);
      auto vertex = new G4PrimaryVertex(vtx, 0.);
      vertex->SetPrimary(incident_part);
      primary_vertices.push_back(vertex);
      
      particle_idx++;
    }
      
    std::cout << "Number of particles: " << particle_idx << std::endl;

    for (const auto& vertex : primary_vertices) 
      ev->AddPrimaryVertex(vertex);

  }



// ***************************************************************************



}
