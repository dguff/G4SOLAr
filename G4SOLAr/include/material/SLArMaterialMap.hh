/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMaterialMap
 * @created     : mercoledì set 25, 2019 15:26:31 CEST
 */

#ifndef SLArMATERIALMAP_HH

#define SLArMATERIALMAP_HH

#include <ostream>
#include <map>

#include "material/SLArMaterialInfo.h"

#include "G4ThreeVector.hh"

class SLArMaterialMap {

  public:
    SLArMaterialMap();
    SLArMaterialMap(const SLArMaterialMap &mat);
    ~SLArMaterialMap();

    void            RegisterMaterial(G4String str, G4String val);
    void                 SetMaterial(G4String str, G4String val);
    bool                    Contains(G4String str);
    void           DumpMaterialIDMap();
    G4String           GetMaterialID(G4String str);
    SLArMaterialInfo*  GetMaterialInfo(G4String str);
    std::map<G4String, SLArMaterialInfo*>& GetMaterialMap()
                                            {return fMatIDMap;}

  private:
    std::map<G4String, SLArMaterialInfo*>  fMatIDMap;
};

#endif /* end of include guard SLArMATERIALMAP_HH */

