#include "ad_type.h"
#ifndef G4HepEmElectronInteractionUMSC_HH
#define G4HepEmElectronInteractionUMSC_HH

#include "G4HepEmMacros.hh"

struct G4HepEmData;
struct G4HepEmParameters;

class  G4HepEmMSCTrackData;
class  G4HepEmRandomEngine;

/**
 * @file    G4HepEmElectronInteractionUMSC.hh
 * @class   G4HepEmElectronInteractionUMSC
 * @author  M. Novak
 * @date    2022
 *
 * @brief Urban model for multiple scattering of e-/e+ for HEP applications.
 */


class G4HepEmElectronInteractionUMSC {
private:
  G4HepEmElectronInteractionUMSC() = delete;

public:

  G4HepEmHostDevice
  static void StepLimit(G4HepEmData* hepEmData, G4HepEmParameters* hepEmPars, G4HepEmMSCTrackData* mscData,
                        G4double ekin, int imat, G4double range, G4double presafety,
                        bool onBoundary, bool iselectron, G4HepEmRandomEngine* rnge);

  G4HepEmHostDevice
  static void SampleScattering(G4HepEmData* hepEmData, G4HepEmMSCTrackData* mscData, G4double pStepLength,
                               G4double preStepEkin, G4double preStepTr1mfp, G4double postStepEkin, G4double postStepTr1mfp,
                               int imat, bool isElectron, G4HepEmRandomEngine* rnge);




  // auxilary method for sampling Urban MSC cos(theta) in the given step (used in the above `SampleScattering`)
  G4HepEmHostDevice
  static G4double SampleCosineTheta(G4double pStepLengt, G4double preStepEkin, G4double preStepTr1mfp,
                                  G4double postStepEkin, G4double postStepTr1mfp, G4double umscTlimitMin,
                                  G4double radLength, G4double zeff, const G4double* umscTailCoeff, const G4double* umscThetaCoeff,
                                  bool isElectron, G4HepEmRandomEngine* rnge);

  // auxilary method for sampling cos(theta) in a simplified way: using an arbitrary pdf with correct mean and stdev
  // (used in the above `SampleCosineTheta`)
  G4HepEmHostDevice
  static G4double SimpleScattering(G4double xmeanth, G4double x2meanth, G4HepEmRandomEngine* rnge);

  // auxilary method for computing theta0 (used in the above `SampleCosineTheta`)
  G4HepEmHostDevice
  static G4double ComputeTheta0(G4double stepInRadLength, G4double postStepEkin, G4double preStepEkin,
                              G4double zeff, const G4double* umscThetaCoeff, bool isElectron);

  // auxilary method for computing the e+ correction to theta0 (used in the above `ComputeTheta0` but only in case of e+)
  G4HepEmHostDevice
  static G4double Theta0PositronCorrection(G4double eekin, G4double zeff);

  // auxilary method for sampling the lateral displacement vector (x,y,0) on a rather approximate way
  G4HepEmHostDevice
  static void   SampleDisplacement(G4double pStepLengt, G4double thePhi, G4HepEmMSCTrackData* mscData, G4HepEmRandomEngine* rnge);

};

#endif // G4HepEmElectronInteractionUMSC_HH
