
#ifndef G4HepEmElectronManager_HH
#define G4HepEmElectronManager_HH

#include "G4HepEmMacros.hh"

struct G4HepEmData;
struct G4HepEmParameters;
struct G4HepEmElectronData;

class  G4HepEmTLData;
class  G4HepEmElectronTrack;
class  G4HepEmMSCTrackData;
class  G4HepEmTrack;
class  G4HepEmRandomEngine;

/**
 * @file    G4HepEmElectronManager.hh
 * @struct  G4HepEmElectronManager
 * @author  M. Novak
 * @date    2020
 *
 * @brief The top level run-time manager for e-/e+ transport simulations.
 *
 * This manager can provide the information regarding how far a given e-/e+ particle
 * goes along its original direction till it's needed to be stopped again because
 * some physics interaction(s) needs to be performed. It is also responsible to
 * perform the required interaction(s) as well.
 *
 * The two methods, through wich this manager acts on the particles, are the
 * G4HepEmElectronManager::HowFar() and G4HepEmElectronManager::Perform(). The
 * first provides the information regarding how far the particle can go, along its
 * original direction, till its next stop due to physics interaction(s).
 * The second can be used to perform the corresponding physics interaction(s).
 * All physics interactions, relevant for HEP detector simulatios, such as
 * `ionisation`, `bremsstrahlung`, `Coulomb scattering` are considered for e-/e+
 * with `annihilation` in addition for e+, including both their continuous, discrete
 * and at-rest parts pespectively. The accuracy of the models, used to describe
 * these interactions, are also compatible to those used by HEP detector simulations.
 *
 * Each G4HepEmRunManager has its own member from this manager for e-/e+ transport.
 * However, a single object could alos be used and shared by all the worker run
 * managers since this G4HepEmElectronManager is stateless. All the state and
 * thread related infomation (e.g. primary/secondary tracks or the thread local
 * random engine) are stored in the G4HepEmTLData input argument, that is also
 * used to deliver the effect of the actions of this manager (i.e. written into
 * the tracks stored in the input G4HepEmTLData argument).
 */

class G4HepEmElectronManager {
private:
  G4HepEmElectronManager() = delete;

public:

  /** Functions that provides the information regarding how far a given e-/e+ particle goes.
    *
    * This functions provides the information regarding how far a given e-/e+ particle goes
    * till it's needed to be stopped again because some physics interaction(s) needs to be performed.
    * The input/primary e-/e+ particle track is provided through the G4HepEmTLData input argument. The
    * The computed physics step lenght is written directly into the input track. There is no any local
    * (state) variable used in the computation.
    *
    * @param hepEmData pointer to the top level, global, G4HepEmData structure.
    * @param hepEmPars pointer to the global, G4HepEmParameters structure.
    * @param tlData    pointer to a worker-local, G4HepEmTLData object. The corresonding object
    *   is assumed to contain all the required input information in its primary G4HepEmTLData::fElectronTrack
    *   member. This member is also used to deliver the results of the function call, i.e. the computed physics
    *   step limit is written into the G4HepEmTLData::fElectronTrack (in its fGStepLength member).
    */
  static void HowFar(struct G4HepEmData* hepEmData, struct G4HepEmParameters* hepEmPars, G4HepEmTLData* tlData);

  /** Function that provides the information regarding how far a given e-/e+ particle goes.
    *
    * This function provides the information regarding how far a given e-/e+ particle goes
    * till it's needed to be stopped again because a discrete interaction needs to be performed.
    * The input/primary e-/e+ particle track is provided as G4HepEmElectronTrack which must have sampled
    * `number-of-interaction-left`. The computed physics step length is written directly into the input
    * track. There is no local (state) variable used in the computation.
    *
    * Note: This function does *not* involve multiple scattering!
    *
    * @param hepEmData pointer to the top level, global, G4HepEmData structure.
    * @param hepEmPars pointer to the global, G4HepEmParameters structure.
    * @param theElTrack pointer to the input information of the track. The data structure must have all entries
    *   `number-of-interaction-left` sampled and is also used to deliver the results of the function call, i.e.
    *   the computed physics step limit is written into its fPStepLength member.
    */
  G4HepEmHostDevice
  static void HowFarToDiscreteInteraction(struct G4HepEmData* hepEmData, struct G4HepEmParameters* hepEmPars, G4HepEmElectronTrack* theElTrack);

  /** Function that provides the information regarding how far a given e-/e+ particle goes.
    *
    * This function provides the information regarding how far a given e-/e+ particle goes
    * till it's needed to be stopped again because of a MSC step limit.
    * The input/primary e-/e+ particle track is provided as G4HepEmElectronTrack which must have sampled
    * `number-of-interaction-left`. The computed physics step length is written directly into the input
    * track. There is no local (state) variable used in the computation.
    *
    * Note: This function does *not* involve multiple scattering!
    *
    * @param hepEmData pointer to the top level, global, G4HepEmData structure.
    * @param hepEmPars pointer to the global, G4HepEmParameters structure.
    * @param theElTrack pointer to the input information of the track, used to deliver the results of
    *   the function call, i.e.the computed physics step limit is written into its fPStepLength and
    *   fGStepLength member.
    */
  G4HepEmHostDevice
  static void HowFarToMSC(struct G4HepEmData* hepEmData, struct G4HepEmParameters* hepEmPars, G4HepEmElectronTrack* theElTrack, G4HepEmRandomEngine* rnge);

  /** Function that provides the information regarding how far a given e-/e+ particle goes.
    *
    * This function provides the information regarding how far a given e-/e+ particle goes
    * till it's needed to be stopped again because some physics interaction(s) needs to be performed.
    * The input/primary e-/e+ particle track is provided as G4HepEmElectronTrack which must have sampled
    * `number-of-interaction-left`. The computed physics step length is written directly into the input
    * track. There is no local (state) variable used in the computation.
    *
    * @param hepEmData pointer to the top level, global, G4HepEmData structure.
    * @param hepEmPars pointer to the global, G4HepEmParameters structure.
    * @param theElTrack pointer to the input information of the track. The data structure must have all entries
    *   `number-of-interaction-left` sampled and is also used to deliver the results of the function call, i.e.
    *   the computed physics step limit is written into its fGStepLength member.
    */
  G4HepEmHostDevice
  static void HowFar(struct G4HepEmData* hepEmData, struct G4HepEmParameters* hepEmPars, G4HepEmElectronTrack* theElTrack, G4HepEmRandomEngine* rnge);

  /** Function that updates the physical step length after the geometry step.
    *
    * If MSC is active and we hit a boundary, convert the geometry step length
    * to a true step length.
    */
  G4HepEmHostDevice
  static void UpdatePStepLength(G4HepEmElectronTrack* theElTrack);

  /** Update the number-of-interaction-left according to the physical step length.
    *
    * @param theElTrack pointer to the input and output information of the track.
    */
  G4HepEmHostDevice
  static void UpdateNumIALeft(G4HepEmElectronTrack* theElTrack);

  /** Apply the mean energy loss along the physical step length.
    *
    * @param hepEmData pointer to the top level, global, G4HepEmData structure.
    * @param hepEmPars pointer to the global, G4HepEmParameters structure.
    * @param theElTrack pointer to the input and output information of the track.
    */
  G4HepEmHostDevice
  static bool ApplyMeanEnergyLoss(struct G4HepEmData* hepEmData, struct G4HepEmParameters* hepEmPars, G4HepEmElectronTrack* theElTrack);

  /** Sample MSC direction change and displacement.
    *
    * @param hepEmData pointer to the top level, global, G4HepEmData structure.
    * @param hepEmPars pointer to the global, G4HepEmParameters structure.
    * @param theElTrack pointer to the input and output information of the track.
    */
  G4HepEmHostDevice
  static void SampleMSC(struct G4HepEmData* hepEmData, struct G4HepEmParameters* hepEmPars, G4HepEmElectronTrack* theElTrack, G4HepEmRandomEngine* rnge);

  /** Sample loss fluctuations for the mean energy loss.
    * 
    * @param hepEmData pointer to the top level, global, G4HepEmData structure.
    * @param hepEmPars pointer to the global, G4HepEmParameters structure.
    * @param theElTrack pointer to the input and output information of the track.
    */
  G4HepEmHostDevice
  static bool SampleLossFluctuations(struct G4HepEmData* hepEmData, struct G4HepEmParameters* hepEmPars, G4HepEmElectronTrack* theElTrack, G4HepEmRandomEngine* rnge);

  /** Functions that performs all continuous physics interactions for a given e-/e+ particle.
    *
    * This functions can be invoked when the particle is propagated to its post-step point to perform all
    * continuous physics interactions. The input/primary e-/e+ particle track is provided through as
    * G4HepEmElectronTrack. There is no local (state) variable used in the computation.
    *
    * @param hepEmData pointer to the top level, global, G4HepEmData structure.
    * @param hepEmPars pointer to the global, G4HepEmParameters structure.
    * @param theElTrack pointer to the input information of the track. All the results of this function call,
    *   i.e. the primary particle's energy updated to its post-interaction(s), are also delivered through this
    *   object.
    * @return boolean whether the particle was stopped
    */
  G4HepEmHostDevice
  static bool PerformContinuous(struct G4HepEmData* hepEmData, struct G4HepEmParameters* hepEmPars, G4HepEmElectronTrack* theElTrack, G4HepEmRandomEngine* rnge);

  /** Function to check if a delta interaction happens instead of the discrete process.
    *
    * @param hepEmData pointer to the top level, global, G4HepEmData structure.
    * @param hepEmPars pointer to the global, G4HepEmParameters structure.
    * @param theTrack pointer to the input information of the track.
    * @param rand number drawn at random
    * @return boolean whether a delta interaction happens
    */
  G4HepEmHostDevice
  static bool CheckDelta(struct G4HepEmData* hepEmData, G4HepEmTrack* theTrack, G4double rand);

  /** Functions that performs the discrete interaction for a given e-/e+ particle.
    *
    * @param hepEmData pointer to the top level, global, G4HepEmData structure.
    * @param hepEmPars pointer to the global, G4HepEmParameters structure.
    * @param tlData    pointer to a worker-local, G4HepEmTLData object. The corresonding object
    *   is assumed to contain all the required input information in its primary G4HepEmTLData::fElectronTrack
    *   member. All the results of this function call, i.e. the primary particle updated to its post-interaction(s)
    *   state as well as the possible secondary particles, are also delivered through this G4HepEmTLData.
    */
  static void PerformDiscrete(struct G4HepEmData* hepEmData, struct G4HepEmParameters* hepEmPars, G4HepEmTLData* tlData);

  /** Functions that performs all physics interactions for a given e-/e+ particle.
    *
    * This functions can be invoked when the particle is propagated to its post-step point to perform all
    * physics interactions. The input/primary e-/e+ particle track is provided through the G4HepEmTLData input
    * argument. The post-interaction(s) primary track and the secondary tracks are also provided through this
    * G4HepEmTLData input argument. There is no any local (state) variable used in the computation.
    *
    * @param hepEmData pointer to the top level, global, G4HepEmData structure.
    * @param hepEmPars pointer to the global, G4HepEmParameters structure.
    * @param tlData    pointer to a worker-local, G4HepEmTLData object. The corresonding object
    *   is assumed to contain all the required input information in its primary G4HepEmTLData::fElectronTrack
    *   member. All the results of this function call, i.e. the primary particle updated to its post-interaction(s)
    *   state as well as the possible secondary particles, are also delivered through this G4HepEmTLData.
    */
  static void Perform(struct G4HepEmData* hepEmData, struct G4HepEmParameters* hepEmPars, G4HepEmTLData* tlData);

  /// The following functions are not meant to be called directly by clients, only from tests.

  /**
    * Auxiliary function that evaluates and provides the `restricted range` for the given kinetic energy
    * and material-cuts combination.
    *
    * @param elData pointer to the global e-/e+ data structure that contains the corresponding `Energy Loss` related data.
    * @param imc    index of the ``G4HepEm`` material-cuts in which the range is required
    * @param ekin   kinetic energy of the e-/e+ at which the range is required
    * @param lekin  logarithm of the above kinetic energy
    * @return `Restricted range` value, interpolated at the given e-/e+ kinetic energy in the given material-cuts based on
    *   the corresponding (discrete) `Energy Loss` data provded as input.
    */

  G4HepEmHostDevice
  static G4double GetRestRange(const struct G4HepEmElectronData* elData, const int imc, const G4double ekin, const G4double lekin);

  G4HepEmHostDevice
  static G4double GetRestDEDX(const struct G4HepEmElectronData* elData, const int imc, const G4double ekin, const G4double lekin);

  G4HepEmHostDevice
  static G4double GetInvRange(const struct G4HepEmElectronData* elData, int imc, G4double range);

  G4HepEmHostDevice
  static G4double GetRestMacXSec(const struct G4HepEmElectronData* elData, const int imc, const G4double ekin,
                               const G4double lekin, bool isioni);

  G4HepEmHostDevice
  static G4double GetRestMacXSecForStepping(const struct G4HepEmElectronData* elData, const int imc, G4double ekin,
                                          G4double lekin, bool isioni);

  G4HepEmHostDevice
  static G4double GetTransportMFP(const struct G4HepEmElectronData* elData, const int im, const G4double ekin, const G4double lekin);

  G4HepEmHostDevice
  static G4double ComputeMacXsecAnnihilation(const G4double ekin, const G4double electronDensity);

  G4HepEmHostDevice
  static G4double ComputeMacXsecAnnihilationForStepping(const G4double ekin, const G4double electronDensity);

  G4HepEmHostDevice
  static void   ConvertTrueToGeometricLength(const G4HepEmData* hepEmData, G4HepEmMSCTrackData* mscData,
                                             G4double ekin, G4double range, int imc, bool iselectron);

  G4HepEmHostDevice
  static void   ConvertGeometricToTrueLength(G4HepEmMSCTrackData* mscData, G4double range, G4double gStepToConvert);
};


#endif // G4HepEmElectronManager_HH
