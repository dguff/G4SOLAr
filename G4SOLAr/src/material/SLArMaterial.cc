/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArMaterial.cc
 * @created     : lunedì ago 08, 2022 17:49:24 CEST
 */

#include "SLArUserPath.hh"
#include "material/SLArMaterial.hh"

#include "rapidjson/document.h"
#include "rapidjson/encodings.h"
#include "rapidjson/allocators.h"
#include "rapidjson/filereadstream.h"

#include "G4UIcommand.hh"
#include "G4NistManager.hh"

SLArMaterial::SLArMaterial() : 
  fMaterialID(""), fMaterial(nullptr), optSurf(nullptr)
{}

SLArMaterial::SLArMaterial(const SLArMaterial &mat)
{
  fMaterialID    = mat.fMaterialID   ; 
  fMaterial      = mat.fMaterial     ; 
  optSurf        = mat.optSurf       ; 
}

SLArMaterial::SLArMaterial(G4String matID)
{
  SetMaterialID(matID);
}

SLArMaterial::~SLArMaterial()
{}

G4Material* SLArMaterial::GetMaterial() 
{
  return fMaterial;
}

G4MaterialPropertiesTable* SLArMaterial::GetMaterialPropTable() {
  return fMaterial->GetMaterialPropertiesTable(); 
}

G4String SLArMaterial::GetMaterialID()
{
  return fMaterialID;
}

void SLArMaterial::BuildMaterialFromDB(G4String mat_id) {

  if (mat_id.empty()) mat_id = fMaterialID;
  else SetMaterialID(mat_id);
   
  auto mtable = G4Material::GetMaterialTable(); 
  for (const auto &m : *mtable) {
    if (std::strcmp(mat_id, m->GetName()) == 0) {
      printf("SLArMaterial::BuildMaterialFromDB(%s) Material already defined. Using stored object.\n", 
          mat_id.c_str()); 
      fMaterial = m; 
      return; 
    }
  }
  
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  // open material description file
  char mat_cfg_file_path[100]; 
  sprintf(mat_cfg_file_path, "%s/%s.json", SLAR_MATERIAL_DIR, mat_id.c_str());
  FILE* mat_cfg_file = std::fopen(mat_cfg_file_path, "r");
  char readBuffer[65536];
  rapidjson::FileReadStream is(mat_cfg_file, readBuffer, sizeof(readBuffer));

  rapidjson::Document d;
  d.ParseStream(is);
  assert(d.IsObject());
  assert(d.HasMember("name")); 

  auto nistManager = G4NistManager::Instance(); 

  printf("SLArMaterial::BuildMaterialFromDB(%s)\n", mat_id.c_str()); 
  if (d.HasMember("NIST")) {
    printf("Building %s from NIST database\n", mat_id.c_str()); 
    fMaterial = nistManager->FindOrBuildMaterial(d["NIST"].GetString(), true); 
    fMaterial->SetName(d["name"].GetString()); 
  } else if (d.HasMember("components")) {
    printf("Building %s based on listed components\n", mat_id.c_str()); 
    assert(d.HasMember("name"));
    assert(d.HasMember("density"));
    assert(d["components"].IsArray());

    auto components = d["components"].GetArray();
    auto density = d["density"].GetObj(); 
    double vunit = 1.0; 
    if (density.HasMember("unit")) 
      vunit = G4UIcommand::ValueOf(density["unit"].GetString()); 

    if (d.HasMember("temperature")) {
      auto temperature = d["temperature"].GetObj();
      G4double tunit = 1.0; 
      if (temperature.HasMember("unit")) 
        tunit = G4UIcommand::ValueOf(temperature["unit"].GetString()); 

      fMaterial = new G4Material(
          d["name"].GetString(), 
          density["val"].GetDouble()*vunit, 
          components.Size(), G4State::kStateLiquid, 
          temperature["val"].GetDouble()*tunit);
    } else {
      fMaterial = new G4Material(
          d["name"].GetString(), 
          density["val"].GetDouble()*vunit, 
          components.Size()); 
    }

    for (const auto& comp : components) {
      G4Element* el = nullptr;
      if ( (el = nistManager->FindOrBuildElement(comp["Z"].GetInt())) ) {}
      else {
        el = new G4Element(
            comp["name"].GetString(), 
            comp["symb"].GetString(),
            comp["Z"].GetDouble(), 
            comp["A"].GetDouble()); 
      } 
      if (comp.HasMember("fraction")) {
        fMaterial->AddElement(el, comp["fraction"].GetDouble()); 
      } else if (comp.HasMember("nAtom")) {
        fMaterial->AddElement(el, comp["nAtom"].GetInt()); 
      } else {
        printf("SLArMaterial::BuildMaterial() %s weight is not specified\n", 
               comp["name"].GetString());
      }
    }
  }

  fclose(mat_cfg_file);
}
