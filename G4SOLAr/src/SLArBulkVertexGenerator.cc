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

SLArBulkVertexGenerator::SLArBulkVertexGenerator(const SLArBulkVertexGenerator& origin)
{
  fLogVol = origin.fLogVol; 
  fBulkTranslation = origin.fBulkTranslation;
  fBulkRotation = origin.fBulkRotation;
  fTolerance = origin.fTolerance; 
  fRandomSeed = origin.fRandomSeed; 
  fNoDaughters = origin.fNoDaughters; 
  fFVFraction = origin.fFVFraction; 

  fSolid = origin.fSolid; 
  fBulkInverseRotation = origin.fBulkInverseRotation; 
  fCounter = origin.fCounter; 
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
  if (!rotation_) fBulkRotation = G4RotationMatrix(); 
  else fBulkRotation = *rotation_;
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

  //printf("lo: [%.1f, %.1f, %.1f]\nhi: [%.1f, %.1f, %.1f]\n", 
      //lo.x(), lo.y(), lo.z(), hi.x(), hi.y(), hi.z());

  double delta = 0.; 
  if (fFVFraction < 1.0) {
    delta = ComputeDeltaX(lo, hi); 
    printf("delta = %g\n", delta);
  }

  G4ThreeVector localVertex;
  G4int maxtries=10000, itry=1;
  do {
    localVertex.set(
        lo.x() + 0.5*delta + G4UniformRand()*(hi.x()-lo.x()-delta),
        lo.y() + 0.5*delta + G4UniformRand()*(hi.y()-lo.y()-delta),
        lo.z() + 0.5*delta + G4UniformRand()*(hi.z()-lo.z()-delta));
  } while (!fSolid->Inside(localVertex) && ++itry < maxtries);

  vertex_ = fBulkInverseRotation(localVertex) + fBulkTranslation;
  fCounter++;
}

double SLArBulkVertexGenerator::ComputeDeltaX(
    G4ThreeVector& lo, G4ThreeVector& hi, double fiducialf) {
  if (fiducialf == 0) fiducialf = fFVFraction; 

  double deltax = 0.; 
  double A = hi.x() - lo.x(); 
  double B = hi.y() - lo.y(); 
  double C = hi.z() - lo.z(); 

  printf("A: %g, B: %g, C: %g, f: %g\n", A, B, C, fiducialf);

  double a = 1.0; 
  double b = -1.0*(A+B+C); 
  double c = (A*B + A*C + B*C); 
  double d = -1.0*(A*B*C)*(1-fiducialf); 

  double D0 = b*b - 3*a*c; 
  double D1 = 2*b*b*b -9*a*b*c +27*a*a*d; 
  double DD = std::cbrt(0.5*(D1 + std::sqrt(D1*D1 -4*D0*D0*D0)));

  deltax = - (b + DD + D0/DD) / (3*a); 
  printf("deltax: %g\n", deltax); 

  return deltax; 
}
