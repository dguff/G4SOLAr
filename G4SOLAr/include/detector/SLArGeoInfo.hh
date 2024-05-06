/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArGeoInfo
 * @created     : giovedì ago 01, 2019 09:58:31 CEST
 */

#ifndef SLArGEOINFO_HH
                   
#define SLArGEOINFO_HH

#include <ostream>
#include <map>
#include <G4ThreeVector.hh>

#include <rapidjson/document.h>
#include <rapidjson/allocators.h>

class SLArGeoInfo {
  public:
    SLArGeoInfo();
    SLArGeoInfo(const SLArGeoInfo &geo);
    SLArGeoInfo(G4String mat);
    ~SLArGeoInfo();

    void     RegisterGeoPar  (G4String str, G4double val);
    void     RegisterGeoPar  (std::pair<G4String, G4double> p);
    void     SetGeoPar       (G4String str, G4double val);
    void     SetGeoPar       (std::pair<G4String, G4double> p);
    bool     Contains        (G4String str);
    void     DumpParMap      ();
    G4double GetGeoPar       (G4String str);
    std::pair<G4String, G4double>
             GetGeoPair      (G4String str);
    bool     ReadFromJSON    (const rapidjson::Value::ConstArray&); 
    bool     ReadFromJSON    (const rapidjson::Value::ConstObject&, const char* prefix = ""); 

  private:
    std::map<G4String, G4double        >  fGeoPar;
};



#endif /* end of include guard SLArGEOINFO_H */

