#include "ad_type.h"

#ifndef G4HepEmElectronTableBuilder_HH
#define G4HepEmElectronTableBuilder_HH

// computes the dedx for e-/e+ and builds the range, dedx and inverse range tables
// for all material-cuts couples

class G4VEmModel;
class G4MollerBhabhaModel;
class G4SeltzerBergerModel;
class G4eBremsstrahlungRelModel;
class G4ParticleDefinition;


struct G4HepEmData;
struct G4HepEmParameters;
struct G4HepEmMatData;


// Should receive pointers to G4 models that are already initialised for the
// particle i.e. either for e- or e+
void BuildELossTables(G4MollerBhabhaModel* mbModel, G4SeltzerBergerModel* sbModel,
                      G4eBremsstrahlungRelModel* rbModel, struct G4HepEmData* hepEmData,
                      struct G4HepEmParameters* hepEmParams, bool iselectron);

void BuildLambdaTables(G4MollerBhabhaModel* mbModel, G4SeltzerBergerModel* sbModel,
                      G4eBremsstrahlungRelModel* rbModel, struct G4HepEmData* hepEmData,
                      struct G4HepEmParameters* hepEmParams, bool iselectron);

void BuildTransportXSectionTables(G4VEmModel* mscModel, struct G4HepEmData* hepEmData,
                      struct G4HepEmParameters* hepEmParams, bool iselectron);

void BuildElementSelectorTables(G4MollerBhabhaModel* mbModel, G4SeltzerBergerModel* sbModel,
                      G4eBremsstrahlungRelModel* rbModel, struct G4HepEmData* hepEmData,
                      struct G4HepEmParameters* hepEmParams, bool iselectron);


void BuildElementSelector(G4double minEKin, G4double maxEKin, int numBinsPerDecade, G4double *data, int& indxCont, const struct G4HepEmMatData& matData, G4VEmModel* emModel, G4double cut, const G4ParticleDefinition* g4PartDef);

int InitElementSelectorEnergyGrid(int binsperdecade, G4double* egrid, G4double mine, G4double maxe,
                                  G4double& logMinEnergy, G4double& invLEDelta);

void BuildSBBremSTables(struct G4HepEmData* hepEmData, struct G4HepEmParameters* hepEmPars, G4SeltzerBergerModel* sbModel);

#endif  // G4HepEmElectronTableBuilder_HH
