
#ifndef G4HepEmPositronInteractionAnnihilation_HH
#define G4HepEmPositronInteractionAnnihilation_HH

#include "G4HepEmMacros.hh"

class  G4HepEmTLData;
class  G4HepEmRandomEngine;

// e+ annihilation to two gamma interaction described by the Heitler model.
// Used between 0 eV - 100 TeV primary e+ kinetic energies i.e. 
// covers both in-flight and at-rest annihilation.
class G4HepEmPositronInteractionAnnihilation {
private:
  G4HepEmPositronInteractionAnnihilation() = delete;

public:
  static void Perform(G4HepEmTLData* tlData, bool isatrest);

  // e+ is already at rest case
  static void AnnihilateAtRest(G4HepEmTLData* tlData);
  // e+ is in-flight case
  static void AnnihilateInFlight(G4HepEmTLData* tlData);

  G4HepEmHostDevice
  static void SampleEnergyAndDirectionsInFlight(const G4double thePrimEkin, const G4double *thePrimDir,
                                                G4double *theGamma1Ekin, G4double *theGamma1Dir,
                                                G4double *theGamma2Ekin, G4double *theGamma2Dir,
                                                G4HepEmRandomEngine* rnge);
};

#endif // G4HepEmPositronInteractionAnnihilation_HH
