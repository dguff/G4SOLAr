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
#include <memory>

#include "event/SLArMCPrimaryInfo.hh"
#include "event/SLArEventAnode.hh"
#include "event/SLArEventSuperCellArray.hh"
#include "config/SLArCfgAnode.hh"
#include "config/SLArCfgBaseSystem.hh"
#include "config/SLArCfgMegaTile.hh"
#include "config/SLArCfgSuperCellArray.hh"

/**
 * @brief   G4SOLAr MC event 
 * @details Object storing the full MC Event, including the trajectories 
 * of all the tracks (with the exception of optical photons), the number of 
 * scintillation photons and ionization electrons produced, and the
 * detected hits for each detector sub-system (Tile SiPMs, Tile pixels, SuperCell)
 *              
 */
template<class P, class A, class X>
class SLArMCEvent : public TObject
{
  public: 

    //! Empty constructor
    SLArMCEvent();
    //! Copy constructor
    SLArMCEvent(const SLArMCEvent&);
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
    inline std::array<double, 3> GetDirection() {return fDirection;}
    int ConfigAnode (std::map<int, SLArCfgAnode*> anodeCfg);
    int ConfigSuperCellSystem (SLArCfgSystemSuperCell* supercellSysCfg); 

    inline std::map<int, A>& GetEventAnode() {return fEvAnode;}
    inline A& GetEventAnodeByTPCID(const int& id) {return fEvAnode.find(id)->second;}
    A& GetEventAnodeByID(const int& id); 
    inline std::map<int, X>& GetEventSuperCellArray() {return fEvSuperCellArray;}
    inline X& GetEventSuperCellArray(const int& id) {return fEvSuperCellArray.find(id)->second;}

    inline std::vector<P>& GetPrimaries() {return fSLArPrimary ;}
    inline P& GetPrimary(int ip) {return fSLArPrimary.at(ip);}
    bool  CheckIfPrimary(int trkId) const;

    size_t RegisterPrimary(P p);
    void  Reset();

  private:
    int fEvNumber; //!< Event number
    std::array<double, 3>  fDirection; //!< Event Direction 
    //! Event's primary particles (and associated secondaries)
    std::vector<P> fSLArPrimary;  
    //! Event data structure of the readout tile system
    std::map<int, A> fEvAnode;
    //! Event data structure of the super-cell system
    std::map<int, X> fEvSuperCellArray; 

  public:
    ClassDef(SLArMCEvent, 2);
};

typedef SLArMCEvent<SLArMCPrimaryInfoPtr*, SLArEventAnodePtr*, SLArEventSuperCellArrayPtr*> SLArMCEventPtr;
typedef SLArMCEvent<std::unique_ptr<SLArMCPrimaryInfoUniquePtr>, std::unique_ptr<SLArEventAnodeUniquePtr>, std::unique_ptr<SLArEventSuperCellArrayUniquePtr>> SLArMCEventUniquePtr;


#endif /* end of include guard SLArEVENT_HH */

