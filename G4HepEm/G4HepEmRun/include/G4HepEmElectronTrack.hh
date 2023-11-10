#include "ad_type.h"


#ifndef G4HepEmElectronTrack_HH
#define G4HepEmElectronTrack_HH


#include "G4HepEmMacros.hh"
#include "G4HepEmTrack.hh"
#include "G4HepEmMSCTrackData.hh"

// A simple track structure for e-/e+ particles.
//
// A G4HepEmElectronTrack contains several extra information beyond those base
// properties that are already available in the NEUTRAL G4HepEmTrack object.
// Such G4HepEmElectronTrack object buffers are stored in each G4HepEmTLData
// object, that are unique for each worker, to propagate and communicate
// primary/secondary e-/e+ track information between the different phases of a
// given step or between the G4HepEmElectron/GammaManager and their functions.

class G4HepEmElectronTrack {

public:
  G4HepEmHostDevice
  G4HepEmElectronTrack() {
    fTrack.ReSet();
    fTrack.SetCharge(-1.0);
    fMSCData.ReSet();
    fRange         =  0.0;
    fPStepLength   =  0.0;
  }

  G4HepEmHostDevice
  G4HepEmElectronTrack(const G4HepEmElectronTrack& o) {
    fTrack         = o.fTrack;
    fTrack.SetCharge(o.GetCharge());
    fMSCData       = o.fMSCData;
    fRange         = o.fRange;
    fPStepLength   = o.fPStepLength;
  }

  G4HepEmHostDevice
  G4HepEmTrack*  GetTrack()  { return &fTrack; }

  G4HepEmHostDevice
  G4HepEmMSCTrackData*  GetMSCTrackData()  { return &fMSCData; }

  G4HepEmHostDevice
  G4double  GetCharge() const { return fTrack.GetCharge(); }

  G4HepEmHostDevice
  void    SetRange(G4double r) { fRange = r; }
  G4HepEmHostDevice
  G4double  GetRange()         { return fRange; }

  G4HepEmHostDevice
  void    SetPStepLength(G4double psl)   { fPStepLength = psl;  }
  G4HepEmHostDevice
  G4double  GetPStepLength()             { return fPStepLength; }

  G4HepEmHostDevice
  void SavePreStepEKin()  {
    fPreStepEKin    = fTrack.GetEKin();
    fPreStepLogEKin = fTrack.GetLogEKin();
  }
  G4HepEmHostDevice
  void SetPreStepEKin(G4double ekin, G4double lekin) {
    fPreStepEKin    = ekin;
    fPreStepLogEKin = lekin;
  }

  G4HepEmHostDevice
  G4double GetPreStepEKin() const { return fPreStepEKin; }
  G4HepEmHostDevice
  G4double GetPreStepLogEKin() const { return fPreStepLogEKin; }

  // Reset all member values
  G4HepEmHostDevice
  void ReSet() {
    fTrack.ReSet();
    fTrack.SetCharge(-1.0);
    fMSCData.ReSet();
    fRange         = 0.0;
    fPStepLength   = 0.0;
  }

private:
  G4HepEmTrack        fTrack;
  G4HepEmMSCTrackData fMSCData;
  G4double              fRange;
  G4double              fPStepLength;  // physical step length >= fTrack.fGStepLength
  G4double              fPreStepEKin;
  G4double              fPreStepLogEKin;
};



#endif // G4HepEmElectronTrack_HH
