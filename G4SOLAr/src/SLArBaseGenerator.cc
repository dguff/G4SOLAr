/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArGenerator.cc
 * @created     : Saturday Mar 30, 2024 22:08:11 CET
 */

#include <SLArBaseGenerator.hh>
#include <SLArBulkVertexGenerator.hh>
#include <SLArBoxSurfaceVertexGenerator.hh>

namespace gen {
void SLArBaseGenerator::ConfigureVertexGenerator(const rapidjson::Value& config) {
  if (!config.HasMember("type")) {
    throw std::invalid_argument("vertex genrator missing mandatory \"type\" field\n");
  }
  
  G4String type = config["type"].GetString(); 
  G4cerr << "Building " << type.data() << " vertex generator" << G4endl;
  EVertexGenerator kGen = getVtxGenIndex( type ); 

  switch (kGen) {
    case (EVertexGenerator::kPoint) : 
      {
        fVtxGen = std::make_unique<SLArPointVertexGenerator>();
        try { fVtxGen->Config( config["config"] ); }        
        catch (const std::exception& e) {
          std::cerr << "ERROR configuring SLArPointVertexGenerator()" << std::endl;
          std::cerr << e.what() << std::endl;
          exit( EXIT_FAILURE );
        }
        break;
      }
    
    case (EVertexGenerator::kBulk) : 
      {
        fVtxGen = std::make_unique<SLArBulkVertexGenerator>(); 
        try {fVtxGen->Config( config["config"] );}
        catch (const std::exception& e) {
          std::cerr << "ERROR configuring SLArBulkVertexGenerator()" << std::endl;
          std::cerr << e.what() << std::endl;
          exit( EXIT_FAILURE );
        }
        break;
      }

    case (EVertexGenerator::kBoxVolSurface) : 
      {
        fVtxGen = std::make_unique<SLArBoxSurfaceVertexGenerator>();
        try {fVtxGen->Config( config["config"] );} 
        catch (const std::exception& e) {
          std::cerr << "ERROR configuring SLArBoxSurfaceVertexGenerator" << std::endl;
          std::cerr << e.what() << std::endl;
          exit( EXIT_FAILURE );
        }
        break;
      }

    default:
      {
        char err_msg[100];
        gen::printVtxGeneratorType();
        sprintf(err_msg, "Unable to find %s vertex generator among the available options\n", 
            type.data()); 
        std::cerr << err_msg << std::endl;
        exit( EXIT_FAILURE ); 
        break;      
      }
  }

  printf("[gen] %s vtx gen for generator %s: %p\n", 
      fVtxGen->GetType().data(), fLabel.data(), static_cast<void*>(fVtxGen.get())); 
  
  return;
}
}

