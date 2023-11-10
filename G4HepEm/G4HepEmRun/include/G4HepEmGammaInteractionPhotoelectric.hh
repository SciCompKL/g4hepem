#include "ad_type.h"
#ifndef G4HepEmGammaInteractionPhotoelectric_HH
#define G4HepEmGammaInteractionPhotoelectric_HH

#include "G4HepEmConstants.hh"
#include "G4HepEmMacros.hh"

class  G4HepEmTLData;
class  G4HepEmRandomEngine;
struct G4HepEmData;


class G4HepEmGammaInteractionPhotoelectric {
private:
  G4HepEmGammaInteractionPhotoelectric() = delete;

public:
  static void Perform(G4HepEmTLData* tlData, struct G4HepEmData* hepEmData);

  G4HepEmHostDevice
  static G4double SelectElementBindingEnergy(const struct G4HepEmData* hepEmData, const int imc, const G4double mxsec, const G4double ekin, G4HepEmRandomEngine* rnge);

  G4HepEmHostDevice
  static void SamplePhotoElectronDirection(const G4double theGammaE, const G4double* theGammaDir, G4double* theDir, G4HepEmRandomEngine* rnge);
};

#endif // G4HepEmGammaInteractionPhotoelectric_HH
