/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArMCEvent.hh
 * @created     : mercoledì ago 10, 2022 11:52:04 CEST
 */

#ifndef SLArMCEVENT_HH

#define SLArMCEVENT_HH

#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include "event/SLArMCPrimaryInfo.hh"
#include "event/SLArEventReadoutTileSystem.hh"
#include "event/SLArEventSuperCellSystem.hh"
#include "config/SLArCfgSystemPix.hh"
#include "config/SLArCfgBaseSystem.hh"
#include "config/SLArCfgMegaTile.hh"
#include "config/SLArCfgSuperCellArray.hh"

/**
 * @brief       : G4SOLAr MC event 
 * @details     : Object storing the full MC Event, including the trajectories 
 * of all the tracks (with the exception of optical photons), the number of 
 * scintillation photons and ionization electrons produced, and the
 * detected hits for each detector sub-system (Tile SiPMs, Tile pixels, SuperCell)
 *              
 */
class SLArMCEvent : public TObject
{
  public: 

    //! Empty constructor
    SLArMCEvent();
    //! Destructuor
    ~SLArMCEvent();

    //! Set the event number
    int SetEvNumber(int nEv);
    //! Return the event number
    int GetEvNumber() {return fEvNumber;}

    //! Set the event direction
    void SetDirection(double* dir = nullptr); 
    //! Set the event direction
    void SetDirection(double px, double py, double pz); 
    //! Get the event direction
    std::array<double, 3> GetDirection() {return fDirection;}
    int ConfigReadoutTileSystem (SLArCfgSystemPix* pixSysCfg);
    int ConfigSuperCellSystem (SLArCfgSystemSuperCell* supercellSysCfg); 

    SLArEventReadoutTileSystem* GetReadoutTileSystem() {return fEvSystemTile;}
    SLArEventSuperCellSystem* GetSuperCellSystem() {return fEvSystemSuperCell;}

    std::vector<SLArMCPrimaryInfo*>& GetPrimaries() {return fSLArPrimary ;}
    SLArMCPrimaryInfo* GetPrimary(int ip) {return fSLArPrimary.at(ip);}
    bool  CheckIfPrimary(int trkId);

    inline size_t RegisterPrimary(SLArMCPrimaryInfo* p) 
      {fSLArPrimary.push_back(p); return fSLArPrimary.size();}

    void  Reset();

  private:
    int fEvNumber; //!< Event number
    std::array<double, 3>  fDirection; //!< Event Direction 
    //! Event's primary particles (and associated secondaries)
    std::vector<SLArMCPrimaryInfo*> fSLArPrimary;  
    //! Event data structure of the readout tile system
    SLArEventReadoutTileSystem* fEvSystemTile;
    //! Event data structure of the super-cell system
    SLArEventSuperCellSystem* fEvSystemSuperCell; 

  public:
    ClassDef(SLArMCEvent, 2);
};


#endif /* end of include guard SLArEVENT_HH */

