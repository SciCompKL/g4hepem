
#ifndef G4HepEmElectronInteractionIoni_HH
#define G4HepEmElectronInteractionIoni_HH

#include "G4HepEmMacros.hh"

class  G4HepEmTLData;
class  G4HepEmRandomEngine;
struct G4HepEmData;


// Ionisation interaction for e-/e+ described by the Moller/Bhabha model.
// Used between 100 eV - 100 TeV primary e-/e+ kinetic energies.
class G4HepEmElectronInteractionIoni {
private:
  G4HepEmElectronInteractionIoni() = delete;

public:
  static void Perform(G4HepEmTLData* tlData, struct G4HepEmData* hepEmData, bool iselectron);

  // Sampling of the energy transferred to the secondary electron in case of e-
  // primary i.e. in case of Moller interaction.
  G4HepEmHostDevice
  static G4double SampleETransferMoller(const G4double elCut, const G4double primEkin, G4HepEmRandomEngine* rnge);

  // Sampling of the energy transferred to the secondary electron in case of e+
  // primary i.e. in case of Bhabha interaction.
  G4HepEmHostDevice
  static G4double SampleETransferBhabha(const G4double elCut, const G4double primEkin, G4HepEmRandomEngine* rnge);

  G4HepEmHostDevice
  static void SampleDirections(const G4double thePrimEkin, const G4double deltaEkin, G4double* theSecElecDir,
                               G4double* thePrimElecDir, G4HepEmRandomEngine* rnge);
};

#endif // G4HepEmElectronInteractionIoni_HH
