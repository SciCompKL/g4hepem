#include "ad_type.h"
#ifndef G4HepEmGammaInteractionCompton_HH
#define G4HepEmGammaInteractionCompton_HH

#include "G4HepEmMacros.hh"

class  G4HepEmTLData;
class  G4HepEmRandomEngine;
struct G4HepEmData;


// Compton scattering for gamma described by the simple Klein-Nishina model.
// Used between 100 eV - 100 TeV primary gamma kinetic energies.
class G4HepEmGammaInteractionCompton {
private:
  G4HepEmGammaInteractionCompton() = delete;

public:
  static void Perform(G4HepEmTLData* tlData, struct G4HepEmData* hepEmData);

  // Sampling of the post interaction photon energy and direction (already in the lab. frame)
  G4HepEmHostDevice
  static G4double SamplePhotonEnergyAndDirection(const G4double primEkin, G4double* primDir,
                                               const G4double* theOrgPrimGmDir, G4HepEmRandomEngine* rnge);
};

#endif  // G4HepEmGammaInteractionCompton_HH
