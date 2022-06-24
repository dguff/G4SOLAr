/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArBulkVertexGenerator
 * @created     : giovedì giu 23, 2022 15:31:58 CEST
 */

#ifndef SLARBULKVERTEXGENERATOR_HH

#define SLARBULKVERTEXGENERATOR_HH

// Standard library
#include <random>

// BxDecay0/G4:
#include "bxdecay0_g4/vertex_generator_interface.hh"

#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

class SLArBulkVertexGenerator
  : public bxdecay0_g4::VertexGeneratorInterface
{
public:

  SLArBulkVertexGenerator();

  ~SLArBulkVertexGenerator() override;
  
  const G4LogicalVolume * GetBulkLogicalVolume() const;
  
  void SetBulkLogicalVolume(const G4LogicalVolume *);
   
  const G4VSolid * GetSolid() const;
 
  const G4ThreeVector & GetSolidTranslation() const;
  
  void SetSolidTranslation(const G4ThreeVector &);
 
  const G4RotationMatrix & GetSolidRotation() const;
 
  const G4RotationMatrix & GetSolidInverseRotation() const;
  
  void SetSolidRotation(G4RotationMatrix* );
 
  double GetTolerance() const;
  
  void SetTolerance(double tolerance_);

  void SetRandomSeed(unsigned int seed_);

  void SetNoDaughters(bool no_daughters_);

  // From the VertexGeneratorInterface abstract class:
  void ShootVertex(G4ThreeVector & vertex_) override;
    
private:

  // Configuration:
  const G4LogicalVolume * fLogVol = nullptr; ///< Reference to the logical volume
  G4ThreeVector fBulkTranslation; ///< The box position in world coordinate frame
  G4RotationMatrix fBulkRotation; ///< The box rotation in world coordinate frame
  double fTolerance{1.0 * CLHEP::um}; ///< Geometrical tolerance (length)
  unsigned int fRandomSeed{0}; ///< Seed for the random number generator
  bool fNoDaughters = false; ///< Flag to reject vertexes generated from daughter volumes
  
  // Working internals:
  const G4VSolid * fSolid = nullptr; ///< Reference to the solid volume from which are generated vertexes
  G4RotationMatrix fBulkInverseRotation; ///< The inverse box rotation
  unsigned int fCounter = 0.0; // Internal vertex counter
     
};



#endif /* end of include guard SLARBULKVERTEXGENERATOR_HH */

