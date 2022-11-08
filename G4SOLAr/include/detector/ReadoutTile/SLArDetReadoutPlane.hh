/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArDetReadoutPlane
 * @created     : lunedì lug 18, 2022 11:09:54 CEST
 */

#ifndef SLARDETREADOUTPLANE_HH

#define SLARDETREADOUTPLANE_HH

#include "detector/SLArBaseDetModule.hh"
#include "G4VPVParameterisation.hh"

class SLArDetReadoutTile;

class SLArDetReadoutPlane : public SLArBaseDetModule 
{       
  public:
    SLArDetReadoutPlane(); 
    SLArDetReadoutPlane(const SLArDetReadoutPlane& detReadoutPlane); 
    ~SLArDetReadoutPlane(); 

    void BuildMaterial(G4String materials_db); 
    void BuildDefalutGeoParMap(); 
    void BuildReadoutPlane(SLArDetReadoutTile*); 
    SLArBaseDetModule* GetTileRow() {return fTileRow;}

  protected:

  private: 
    SLArMaterial* fMatReadoutPlane; 
    SLArBaseDetModule* fTileRow;
    void BuildTileRow(SLArDetReadoutTile*);

  public: 
    class SLArMTileParametrization : public G4VPVParameterisation {
      public: 
        SLArMTileParametrization(EAxis, G4ThreeVector, G4double);
        ~SLArMTileParametrization() {};

        void ComputeTransformation(G4int, G4VPhysicalVolume*) const; 

        EAxis GetReplicationAxis() {return fReplicaAxis;}
        G4double GetSpacing() {return fSpacing;}
        G4ThreeVector GetStartPos() {return fStartPos;}

      private:
        EAxis fReplicaAxis; 
        G4ThreeVector fStartPos; 
        G4double fSpacing; 
        G4ThreeVector fAxisVector; 
    }; 
};



#endif /* end of include guard SLARDETREADOUTPLANE_HH */

