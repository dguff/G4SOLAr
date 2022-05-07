/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMaterialMap
 * @created     : mercoledì set 25, 2019 15:29:45 CEST
 */

#include <utility>
#include "material/SLArMaterialMap.hh"

SLArMaterialMap::SLArMaterialMap() { }

SLArMaterialMap::SLArMaterialMap(const SLArMaterialMap &mat)
{
  fMatIDMap = mat.fMatIDMap;
}

SLArMaterialMap::~SLArMaterialMap()
{
  fMatIDMap.clear();
  //fMatPar.clear();  
}

void SLArMaterialMap::RegisterMaterial( G4String str, G4String val)
{
  fMatIDMap.insert( std::pair<G4String, SLArMaterialInfo*>
      (str, 
       new SLArMaterialInfo(val)) );
  return;
}


void SLArMaterialMap::SetMaterial(G4String str, G4String val) 
{
  if (Contains(str))
    fMatIDMap.find(str)->second = new SLArMaterialInfo(val);
  else 
    RegisterMaterial(str, val);

  return;
}


bool SLArMaterialMap::Contains(G4String str)
{
  int n = 0;
  bool out = false;
  n = fMatIDMap.count( str );
  if ( n > 0) out = true;
  else        out = false;

  return out;
}

G4String SLArMaterialMap::GetMaterialID(G4String str)
{
  G4String out = "";
  if (!Contains(str))
  {
    G4cerr << "SLArMaterialMap::GetMaterialID Par " 
           << str << " not found in parameter map" << G4endl;
  }
  else {
    out = fMatIDMap.find(str)->second->GetMaterialID();
  }

  return out;
}

SLArMaterialInfo* SLArMaterialMap::GetMaterialInfo(G4String str)
{
  return fMatIDMap.find(str)->second;
}

void SLArMaterialMap::DumpMaterialIDMap()
{
  G4cout << "SLArMaterialMap::DumpMaterialIDMap()" << G4endl;
  G4cout << "- - - - - - - - - - - - - - - - - - - -" << G4endl;
  for (auto const& it : fMatIDMap)
  {
    G4cout << "- " << it.first << ":\n" 
           << it.second->GetMaterialBuilder()->GetMaterial() 
           << "\n" << G4endl;
  }
  G4cout << "- - - - - - - - - - - - - - - - - - - -\n" << G4endl;
   
}


