#include "ad_type.h"

#ifndef G4HepEmGammaInteractionConversion_HH
#define G4HepEmGammaInteractionConversion_HH

#include "G4HepEmMacros.hh"

#include <cmath>

class  G4HepEmTLData;
class  G4HepEmRandomEngine;
struct G4HepEmData;
struct G4HepEmElemData;

class G4HepEmGammaInteractionConversion {
private:
  G4HepEmGammaInteractionConversion() = delete;

public:
  static void Perform(G4HepEmTLData* tlData, struct G4HepEmData* hepEmData);

  G4HepEmHostDevice
  static void SampleKinEnergies(struct G4HepEmData* hepEmData, G4double thePrimEkin, G4double theLogEkin,
                                int theMCIndx, G4double& eKinEnergy, G4double& pKinEnergy, G4HepEmRandomEngine* rnge);


  G4HepEmHostDevice
  static void SampleDirections(const G4double* orgGammaDir, G4double* secElDir, G4double* secPosDir,
                               const G4double secElEkin, const G4double secPosEkin, G4HepEmRandomEngine* rnge);


  // Target atom selector for the above bremsstrahlung intercations in case of
  // materials composed from multiple elements.
  G4HepEmHostDevice
  static int SelectTargetAtom(const struct G4HepEmGammaData* gmData, const int imat, const G4double ekin,
                              const G4double lekin, const G4double urndn);

  G4HepEmHostDevice
  static G4double SampleEnergyRateNoLPM(const G4double normCond, const G4double epsMin, const G4double epsRange,
                                      const G4double deltaFactor, const G4double invF10, const G4double invF20,
                                      const G4double fz, G4HepEmRandomEngine* rnge);

  G4HepEmHostDevice
  static G4double SampleEnergyRateWithLPM(const G4double normCond, const G4double epsMin, const G4double epsRange,
                                        const G4double deltaFactor, const G4double invF10, const G4double invF20,
                                        const G4double fz, G4HepEmRandomEngine* rnge, const G4double eGamma,
                                        const G4double lpmEnergy, const struct G4HepEmElemData* elemData);

  G4HepEmHostDevice
  static void ComputePhi12(const G4double delta, G4double &phi1, G4double &phi2);


  // Compute the value of the screening function 3*PHI1(delta) - PHI2(delta):
  G4HepEmHostDevice
  static G4double ScreenFunction1(const G4double delta);

  // Compute the value of the screening function 1.5*PHI1(delta) +0.5*PHI2(delta):
  G4HepEmHostDevice
  static G4double ScreenFunction2(const G4double delta);

  // Same as ScreenFunction1 and ScreenFunction2 but computes them at once
  G4HepEmHostDevice
  static void ScreenFunction12(const G4double delta, G4double &f1, G4double &f2);
};

#endif  // G4HepEmGammaInteractionConversion_HH
