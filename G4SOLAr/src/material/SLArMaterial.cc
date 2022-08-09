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
#include <cassert>

SLArMaterial::SLArMaterial() : 
  fMaterialID(""), fMaterial(nullptr), fOpticalSurf(nullptr)
{}

SLArMaterial::SLArMaterial(const SLArMaterial &mat)
{
  fMaterialID    = mat.fMaterialID   ; 
  fMaterial      = mat.fMaterial     ; 
  fOpticalSurf   = mat.fOpticalSurf       ; 
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

  if (d.HasMember("PropertiesTable")) {
    auto ptable = new G4MaterialPropertiesTable(); 
    ParseMPT(d["PropertiesTable"], ptable); 
    fMaterial->SetMaterialPropertiesTable(ptable); 
  }

  if (d.HasMember("SurfaceProperties")) {
  }

  fclose(mat_cfg_file);
}

void SLArMaterial::ParseMPT(const rapidjson::Value& jptable, G4MaterialPropertiesTable* mpt) {

  assert(jptable.IsArray());
  const auto properties = jptable.GetArray(); 

  for (const auto& p : properties) {
    assert(p.HasMember("property")); 
    assert(p.HasMember("value")); 

    G4String pname = p["property"].GetString(); 

    if (p["value"].IsArray()) {
      assert(p["value"].GetArray().Size() == 2); 
      std::vector<double> vE; vE.reserve(500); 
      std::vector<double> vP; vP.reserve(500); 
      for (const auto &v : p["value"].GetArray()) {
        assert(v.HasMember("var")); 
        assert(v.HasMember("val")); 
        assert(v["val"].IsArray()); 
        G4double vunit = 1.0; 
        if (v.HasMember("unit")) {
          vunit = G4UIcommand::ValueOf(v["unit"].GetString());
        }
        if (strcmp("Energy", v["var"].GetString()) == 0) {
          for (const auto &val : v["val"].GetArray()) {
            vE.push_back( val.GetDouble()*vunit ); 
          }
        } else {
          for (const auto &val : v["val"].GetArray()) {
            vP.push_back( val.GetDouble()*vunit ); 
          } 
        }
      }
      G4bool is_custom = false; 
      if (p.HasMember("custom")) is_custom = p["custom"].GetBool(); 
      mpt->AddProperty(pname, vE, vP, is_custom); 
    } else {
      G4double punit = 1.0; 
      if (p.HasMember("unit")) 
        punit = G4UIcommand::ValueOf(p["unit"].GetString()); 
      G4double pvalue = p["value"].GetDouble(); 
      G4bool is_custom = false; 
      if (p.HasMember("custom")) is_custom = p["custom"].GetBool(); 


      if (pname == "BIRKSCONSTANT") {
        fMaterial->GetIonisation()->SetBirksConstant(pvalue*CLHEP::mm/CLHEP::MeV); 
      } else {
        mpt->AddConstProperty(pname, pvalue*punit, is_custom);
      }
    }
  }
  return;
}

void SLArMaterial::ParseSurfaceProperties(const rapidjson::Value& jptable) {
  assert(jptable.IsObject());
  auto surfcfg = jptable.GetObject(); 
  assert(surfcfg.HasMember("name"));
  G4String surfName = surfcfg["name"].GetString();
  fOpticalSurf = new G4OpticalSurface(surfcfg["name"].GetString());

  assert(surfcfg.HasMember("model"));
  assert(surfcfg.HasMember("type"));
  assert(surfcfg.HasMember("finished")); 

  G4String model_str = surfcfg["model"].GetString(); 
  G4String type_str  = surfcfg["type"].GetString(); 
  G4String finished_str = surfcfg["finished"].GetString(); 

  if (G4StrUtil::contains(model_str, "unified")) {
    fOpticalSurf->SetModel(G4OpticalSurfaceModel::unified); 
  } else { 
    fOpticalSurf->SetModel(G4OpticalSurfaceModel::glisur); 
  }

  if (G4StrUtil::contains(type_str, "dielectric_metal")) {
    fOpticalSurf->SetType( G4SurfaceType::dielectric_metal );
  } else {
    fOpticalSurf->SetType( G4SurfaceType::dielectric_dielectric );
  }

  if (G4StrUtil::contains(finished_str, "polished")) {
    fOpticalSurf->SetFinish( G4OpticalSurfaceFinish::polished );
  } else { 
    fOpticalSurf->SetFinish( G4OpticalSurfaceFinish::ground ); 
  }

  if (surfcfg.HasMember("polished")) {
    fOpticalSurf->SetPolish( surfcfg["polished"].GetDouble() );
  }


  if (surfcfg.HasMember("PropertiesTable")) {
    G4MaterialPropertiesTable* srf_ptable = new G4MaterialPropertiesTable();
    ParseMPT(surfcfg["PropertiesTable"], srf_ptable);
    fOpticalSurf->SetMaterialPropertiesTable(srf_ptable);
  }

  return;
}
