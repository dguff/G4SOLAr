/**
 * @author      Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        SLArBoxSurfaceVertexGenerator
 * @created     Tue Apr 11, 2023 09:58:41 CEST
 */

#include "SLArBoxSurfaceVertexGenerator.hh"
#include "G4RandomTools.hh"

std::map<SLArBoxSurfaceVertexGenerator::EBoxFace, G4ThreeVector> 
SLArBoxSurfaceVertexGenerator::BoxFaceNormal  = {
  {SLArBoxSurfaceVertexGenerator::kXplus, G4ThreeVector(-1, 0, 0)},
  {SLArBoxSurfaceVertexGenerator::kXminus, G4ThreeVector(+1, 0, 0)},
  {SLArBoxSurfaceVertexGenerator::kYplus, G4ThreeVector(0, -1, 0)},
  {SLArBoxSurfaceVertexGenerator::kYminus, G4ThreeVector(0, +1, 0)},
  {SLArBoxSurfaceVertexGenerator::kZplus, G4ThreeVector(0, 0, -1)},
  {SLArBoxSurfaceVertexGenerator::kZminus, G4ThreeVector(0, 0, +1)}
};

SLArBoxSurfaceVertexGenerator::SLArBoxSurfaceVertexGenerator()
{
  fBulkInverseRotation = fBulkRotation.inverse(); 
}

SLArBoxSurfaceVertexGenerator::SLArBoxSurfaceVertexGenerator(const SLArBoxSurfaceVertexGenerator& origin)
{
  fLogVol = origin.fLogVol; 
  fBulkTranslation = origin.fBulkTranslation;
  fBulkRotation = origin.fBulkRotation;
  fTolerance = origin.fTolerance; 
  fRandomSeed = origin.fRandomSeed; 
  fNoDaughters = origin.fNoDaughters; 

  fSolid = origin.fSolid; 
  fBulkInverseRotation = origin.fBulkInverseRotation; 
  fCounter = origin.fCounter; 

}

SLArBoxSurfaceVertexGenerator::~SLArBoxSurfaceVertexGenerator()
{
  std::clog << "[log] SLArBoxSurfaceVertexGenerator::DTOR: counter=" << fCounter << " generated vertexes\n";
}
 
void SLArBoxSurfaceVertexGenerator::SetBoxLogicalVolume(const G4LogicalVolume * logvol_)
{
  fSolid = logvol_->GetSolid();
  fLogVol = logvol_;
  std::clog << "[log] SLArBoxSurfaceVertexGenerator::SetBoxLogicalVolume: solid=" << fSolid << "\n";
}

const G4VSolid * SLArBoxSurfaceVertexGenerator::GetSolid() const
{
  return fSolid;
}

const G4ThreeVector & SLArBoxSurfaceVertexGenerator::GetSolidTranslation() const
{
  return fBulkTranslation;
}

void SLArBoxSurfaceVertexGenerator::SetSolidTranslation(const G4ThreeVector & translation_)
{
  fBulkTranslation = translation_;
  std::clog << "[log] SLArBoxSurfaceVertexGenerator::SetSolidTranslation: translation=" << (fBulkTranslation / CLHEP::mm) << " mm\n";
}

const G4RotationMatrix & SLArBoxSurfaceVertexGenerator::GetSolidRotation() const
{
  return fBulkRotation;
}

const G4RotationMatrix & SLArBoxSurfaceVertexGenerator::GetSolidInverseRotation() const
{
  return fBulkInverseRotation;
}
  
void SLArBoxSurfaceVertexGenerator::SetSolidRotation(G4RotationMatrix* rotation_)
{
  if (!rotation_) fBulkRotation = G4RotationMatrix(); 
  else fBulkRotation = *rotation_;
  fBulkInverseRotation = fBulkRotation.inverse();
  std::clog << "[log] SLArBoxSurfaceVertexGenerator::SetSolidRotation: rotation=" << fBulkRotation << "\n";
}

double SLArBoxSurfaceVertexGenerator::GetTolerance() const
{
  return fTolerance;
}
  
void SLArBoxSurfaceVertexGenerator::SetTolerance(double tolerance_)
{
  if (tolerance_ <= 0.0) {
    throw std::range_error("SLArBoxSurfaceVertexGenerator::SetTolerance: invalid tolerance!");
  }
  fTolerance = tolerance_;  
  std::clog << "[log] SLArBoxSurfaceVertexGenerator::SetBoxRotation: tolerance=" << fTolerance / CLHEP::mm << " mm\n";
}

void SLArBoxSurfaceVertexGenerator::SetRandomSeed(unsigned int seed_)
{
  fRandomSeed = seed_;
  std::clog << "[log] SLArBoxSurfaceVertexGenerator::SetRandomSeed: seed=" << fRandomSeed << " after " << fCounter << " generated vertexes\n";
}

void SLArBoxSurfaceVertexGenerator::SetNoDaughters(bool no_daughters_)
{
  fNoDaughters = no_daughters_;
}
 
void SLArBoxSurfaceVertexGenerator::ShootVertex(G4ThreeVector & vertex_)
{
  // sample a random vertex inside the volume given by fVolumeName
  G4ThreeVector lo; 
  G4ThreeVector hi;
  G4ThreeVector dim; 
  std::vector<G4ThreeVector> axis = 
  {G4ThreeVector(1, 0, 0), G4ThreeVector(0, 1, 0), G4ThreeVector(0, 0, 1)}; 
  fSolid->BoundingLimits(lo, hi);

  for (int i=0; i<3; i++) dim[i] = fabs(hi[i] - lo[i]); 

  if (fFixFace == false) {
    G4double total_area = 2*(dim.x()*dim.y() + dim.x()*dim.z() + dim.y()*dim.z()); 
    std::map<EBoxFace, G4double> area_fraction; 

    for (int i=0; i<6; i++) {
      EBoxFace kFace = EBoxFace(i); 
      G4double area = 1; 
      for (int  j=0; j < 3; j++) {
        if (BoxFaceNormal[kFace].dot(axis[j]) == 0) area *= dim[j]; 
      }

      area_fraction.insert( std::make_pair(kFace, area / total_area)); 
    }

    G4double face_sample = G4UniformRand(); 
    G4double face_prob = 0.; 
    G4int iface = -1;
    //printf("face_sample = %g\n", face_sample);
    while (face_sample > face_prob) {
      iface++;
      face_prob += area_fraction[(EBoxFace)iface]; 
    }

    fVtxFace = (EBoxFace)iface;
  }

  //printf("vertex generator face is: %i\n", fVtxFace);
  //G4cout << "face normal is: " << BoxFaceNormal[fVtxFace] << G4endl;
  G4ThreeVector face_axis = G4ThreeVector(0, 0, 0); 
  for (int j=0; j<3; j++) {
    if (axis[j].dot(BoxFaceNormal[fVtxFace]) != 0) {
      face_axis = axis[j]; 
      break;
    }
  }

  G4ThreeVector face_center_local = 
    -0.5*dim.dot(BoxFaceNormal[fVtxFace])*face_axis;
  //G4cout << "face_center_local: " << face_center_local << G4endl; 
  G4ThreeVector local_displacement = G4ThreeVector(0, 0, 0); 
  for (int j=0; j<3; j++) {
    double rnd = G4UniformRand() - 0.5; 
    local_displacement[j] += 
    BoxFaceNormal[fVtxFace].cross( axis[j] ).mag()*rnd*dim[j];
  }
  //G4cout << "local_displacement: " << local_displacement << G4endl;

  G4ThreeVector localVertex = face_center_local + local_displacement;

  vertex_ = fBulkInverseRotation(localVertex) + fBulkTranslation;
  //G4cout << "vertex = " << vertex_ << G4endl;
  //getchar(); 
  fCounter++;
}


