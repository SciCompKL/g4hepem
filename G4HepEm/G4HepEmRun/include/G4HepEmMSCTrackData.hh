#ifndef G4HepEmMSCTrackData_HH
#define G4HepEmMSCTrackData_HH

#include "G4HepEmMacros.hh"

// A simple structure that encapsulates MSC related data for a single e-/e+ track.
//
// This object stores all the MSC related information that an e-/e+ a track needs
// to keep and propagate between the different parts of the simulation step related
// to MSC. G4HepEmElectronTrack contains an instance of this.

#include <cmath>

class G4HepEmMSCTrackData {

public:
  G4HepEmHostDevice
  G4HepEmMSCTrackData() { ReSet(); }

  G4HepEmHostDevice
  G4HepEmMSCTrackData(const G4HepEmMSCTrackData& o) {
    fLambtr1              = o.fLambtr1;

    fTrueStepLength       = o.fTrueStepLength;
    fZPathLength          = o.fZPathLength;
    fDisplacement[0]      = o.fDisplacement[0];
    fDisplacement[1]      = o.fDisplacement[1];
    fDisplacement[2]      = o.fDisplacement[2];
    fDirection[0]         = o.fDirection[0];
    fDirection[1]         = o.fDirection[1];
    fDirection[2]         = o.fDirection[2];

    fInitialRange         = o.fInitialRange;
    fDynamicRangeFactor   = o.fDynamicRangeFactor;
    fTlimitMin            = o.fTlimitMin;

    fPar1                 = o.fPar1;
    fPar2                 = o.fPar2;
    fPar3                 = o.fPar3;

    fIsNoScatteringInMSC  = o.fIsNoScatteringInMSC;
    fIsDisplace           = o.fIsDisplace;
    fIsFirstStep          = o.fIsFirstStep;
    fIsActive             = o.fIsActive;
  }

  G4HepEmHostDevice
  void SetDisplacement(G4double x, G4double y, G4double z) {
    fDisplacement[0] = x;
    fDisplacement[1] = y;
    fDisplacement[2] = z;
  }
  G4HepEmHostDevice
  G4double* GetDisplacement() { return fDisplacement; }

  G4HepEmHostDevice
  void SetNewDirection(G4double x, G4double y, G4double z) {
    fDirection[0] = x;
    fDirection[1] = y;
    fDirection[2] = z;
  }
  G4HepEmHostDevice
  G4double* GetDirection() { return fDirection; }


  // reset all member values
  G4HepEmHostDevice
  void ReSet() {
    fLambtr1              = 0.;

    fTrueStepLength       = 0.;
    fZPathLength          = 0.;
    fDisplacement[0]      = 0.;
    fDisplacement[1]      = 0.;
    fDisplacement[2]      = 0.;
    fDirection[0]         = 0.;
    fDirection[1]         = 0.;
    fDirection[2]         = 1.;

    fInitialRange         = 1.0e+21;
    fDynamicRangeFactor   = 0.04;   // fr will be set in the MSC step limit
    fTlimitMin            = 1.0E-7; // tlimitmin 10*0.01 [nm] 1.0E-7[mm]

    fPar1                 = -1.;
    fPar2                 =  0.;
    fPar3                 =  0.;

    fIsNoScatteringInMSC  = false;
    fIsDisplace           = false;
    fIsFirstStep          = true;
    fIsActive             = false;
  }

public:
  G4double fLambtr1;            // first transport mfp

  G4double fTrueStepLength;     // the true, i.e. physical step Length
  G4double fZPathLength;        // projection of the transport distance along the org. dir.
  G4double fDisplacement[3];    // the displacement vector
  G4double fDirection[3];       // direction proposed by MSC

  G4double fInitialRange;       // initial range value (entering in the volume)
  G4double fDynamicRangeFactor; // dynamic range factor i.e. `fr`
  G4double fTlimitMin;          // minimum true step length i.e. `tlimitmin`

  G4double fPar1;               // parameters used in the true - > geom conversion
  G4double fPar2;
  G4double fPar3;

  bool   fIsNoScatteringInMSC; // indicates that no scattering happend
  bool   fIsDisplace;          // indicates that displacement needs to be done
  bool   fIsFirstStep;         // first step with this particle

  bool   fIsActive;

};

#endif // G4HepEmMSCTrackData
