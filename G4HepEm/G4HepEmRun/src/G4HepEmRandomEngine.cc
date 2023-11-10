#include "G4HepEmRandomEngine.hh"

#include "CLHEP/Random/RandomEngine.h"

G4double G4HepEmRandomEngine::flat() {
  return ((CLHEP::HepRandomEngine*)fObject)->flat();
}

void  G4HepEmRandomEngine::flatArray(const int size, G4double* vect) {
  ((CLHEP::HepRandomEngine*)fObject)->flatArray(size, vect);
}
