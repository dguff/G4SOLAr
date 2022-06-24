/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArBulkVertexGenerator
 * @created     : giovedì giu 23, 2022 15:34:13 CEST
 */

#include "SLArBulkVertexGenerator.hh"

#include "G4RandomTools.hh"

SLArBulkVertexGenerator::SLArBulkVertexGenerator()
{
  fBulkInverseRotation = fBulkRotation.inverse();
}

SLArBulkVertexGenerator::~SLArBulkVertexGenerator()
{
  std::clog << "[log] SLArBulkVertexGenerator::DTOR: counter=" << fCounter << " generated vertexes\n";
}
 
const G4LogicalVolume * SLArBulkVertexGenerator::GetBulkLogicalVolume() const
{
  return fLogVol;
}
  
void SLArBulkVertexGenerator::SetBulkLogicalVolume(const G4LogicalVolume * logvol_)
{
  fSolid = logvol_->GetSolid();
  fLogVol = logvol_;
  std::clog << "[log] SLArBulkVertexGenerator::SetBulkLogicalVolume: solid=" << fSolid << "\n";
}

const G4VSolid * SLArBulkVertexGenerator::GetSolid() const
{
  return fSolid;
}

const G4ThreeVector & SLArBulkVertexGenerator::GetSolidTranslation() const
{
  return fBulkTranslation;
}

void SLArBulkVertexGenerator::SetSolidTranslation(const G4ThreeVector & translation_)
{
  fBulkTranslation = translation_;
  std::clog << "[log] SLArBulkVertexGenerator::SetSolidTranslation: translation=" << (fBulkTranslation / CLHEP::mm) << " mm\n";
}

const G4RotationMatrix & SLArBulkVertexGenerator::GetSolidRotation() const
{
  return fBulkRotation;
}

const G4RotationMatrix & SLArBulkVertexGenerator::GetSolidInverseRotation() const
{
  return fBulkInverseRotation;
}
  
void SLArBulkVertexGenerator::SetSolidRotation(G4RotationMatrix* rotation_)
{
  fBulkRotation = *rotation_;
  fBulkInverseRotation = fBulkRotation.inverse();
  std::clog << "[log] SLArBulkVertexGenerator::SetSolidRotation: rotation=" << fBulkRotation << "\n";
}

double SLArBulkVertexGenerator::GetTolerance() const
{
  return fTolerance;
}
  
void SLArBulkVertexGenerator::SetTolerance(double tolerance_)
{
  if (tolerance_ <= 0.0) {
    throw std::range_error("SLArBulkVertexGenerator::SetTolerance: invalid tolerance!");
  }
  fTolerance = tolerance_;  
  std::clog << "[log] SLArBulkVertexGenerator::SetBoxRotation: tolerance=" << fTolerance / CLHEP::mm << " mm\n";
}

void SLArBulkVertexGenerator::SetRandomSeed(unsigned int seed_)
{
  fRandomSeed = seed_;
  std::clog << "[log] SLArBulkVertexGenerator::SetRandomSeed: seed=" << fRandomSeed << " after " << fCounter << " generated vertexes\n";
}

void SLArBulkVertexGenerator::SetNoDaughters(bool no_daughters_)
{
  fNoDaughters = no_daughters_;
}
 
void SLArBulkVertexGenerator::ShootVertex(G4ThreeVector & vertex_)
{

  // sample a random vertex inside the volume given by fVolumeName
  G4ThreeVector lo; 
  G4ThreeVector hi;
  fSolid->BoundingLimits(lo, hi);

  G4ThreeVector localVertex;
  G4int maxtries=10000, itry=1;
  do {
    localVertex.set(lo.x() + G4UniformRand()*(hi.x()-lo.x()),
        lo.y() + G4UniformRand()*(hi.y()-lo.y()),
        lo.z() + G4UniformRand()*(hi.z()-lo.z()));
  } while (!fSolid->Inside(localVertex) && ++itry < maxtries);

  vertex_ = fBulkInverseRotation(localVertex) + fBulkTranslation;
  fCounter++;
}
