#include "ad_type.h"

#include "G4HepEmSBTableData.hh"

void AllocateSBTableData(struct G4HepEmSBTableData** theSBTableData, int numHepEmMatCuts, int numElemsInMC, int numSBData) {
  FreeSBTableData(theSBTableData);
  *theSBTableData = MakeSBTableData(numHepEmMatCuts, numElemsInMC, numSBData);
}

G4HepEmSBTableData* MakeSBTableData(int numHepEmMatCuts, int numElemsInMC, int numSBData) {
  auto* tmp = new G4HepEmSBTableData;

  tmp->fNumHepEmMatCuts             = numHepEmMatCuts;
  tmp->fGammaCutIndxStartIndexPerMC = new int[numHepEmMatCuts];
  for (int i=0; i<numHepEmMatCuts; ++i) {
    tmp->fGammaCutIndxStartIndexPerMC[i] = -1;
  }

  tmp->fNumElemsInMatCuts = numElemsInMC;
  tmp->fGammaCutIndices   = new int[numElemsInMC];
  for (int i=0; i<numElemsInMC; ++i) {
    tmp->fGammaCutIndices[i] = -1;
  }

  tmp->fNumSBTableData = numSBData;
  tmp->fSBTableData    = new G4double[numSBData];

  return tmp;
}


void FreeSBTableData(struct G4HepEmSBTableData** theSBTableData) {
  if (*theSBTableData) {
    delete[] (*theSBTableData)->fGammaCutIndxStartIndexPerMC;
    delete[] (*theSBTableData)->fGammaCutIndices;
    delete[] (*theSBTableData)->fSBTableData;
    delete (*theSBTableData);
    *theSBTableData = nullptr;
  }
}

#ifdef G4HepEm_CUDA_BUILD
#include <cuda_runtime.h>
#include "G4HepEmCuUtils.hh"

#include <cstring>

void CopySBTableDataToDevice(struct G4HepEmSBTableData* onHOST, struct G4HepEmSBTableData** onDEVICE) {
  if ( !onHOST ) return;
  // clean away previous (if any)
  if ( *onDEVICE ) {
    FreeSBTableDataOnDevice ( onDEVICE );
  }
  // Create a G4HepEmSBTableData structure on the host to store pointers to _d
  // side arrays on the _h side.
  struct G4HepEmSBTableData* sbTablesHTo_d = new G4HepEmSBTableData;
  // Set non-pointer members via a memcpy of the entire structure.
  memcpy(sbTablesHTo_d, onHOST, sizeof(G4HepEmSBTableData));
  const int numHepEmMatCuts         = onHOST->fNumHepEmMatCuts;
  const int numElemsInMatCuts       = onHOST->fNumElemsInMatCuts;
  const int numSBTableData          = onHOST->fNumSBTableData;
  //
  // allocate device side memory for the dynamic arrys
  gpuErrchk ( cudaMalloc ( &(sbTablesHTo_d->fGammaCutIndxStartIndexPerMC), sizeof( int )    * numHepEmMatCuts   ) );
  gpuErrchk ( cudaMalloc ( &(sbTablesHTo_d->fGammaCutIndices),             sizeof( int )    * numElemsInMatCuts ) );
  gpuErrchk ( cudaMalloc ( &(sbTablesHTo_d->fSBTableData),                 sizeof( G4double ) * numSBTableData    ) );
  //
  gpuErrchk ( cudaMemcpy (   sbTablesHTo_d->fGammaCutIndxStartIndexPerMC,  onHOST->fGammaCutIndxStartIndexPerMC, sizeof( int )    * numHepEmMatCuts,   cudaMemcpyHostToDevice ) );
  gpuErrchk ( cudaMemcpy (   sbTablesHTo_d->fGammaCutIndices,              onHOST->fGammaCutIndices,             sizeof( int )    * numElemsInMatCuts, cudaMemcpyHostToDevice ) );
  gpuErrchk ( cudaMemcpy (   sbTablesHTo_d->fSBTableData,                  onHOST->fSBTableData,                 sizeof( G4double ) * numSBTableData ,   cudaMemcpyHostToDevice ) );
  //
  // Finaly copy the top level, i.e. the main struct with the already
  // appropriate pointers to device side memory locations but stored on the host
  gpuErrchk ( cudaMalloc (  onDEVICE,                sizeof(  struct G4HepEmSBTableData ) ) );
  gpuErrchk ( cudaMemcpy ( *onDEVICE, sbTablesHTo_d, sizeof(  struct G4HepEmSBTableData ), cudaMemcpyHostToDevice ) );
  // and clean
  delete sbTablesHTo_d;
}


void FreeSBTableDataOnDevice(struct G4HepEmSBTableData** onDEVICE) {
  if (*onDEVICE) {
    // copy the on-device data back to host in order to be able to free the device
    // side dynamically allocated memories
    struct G4HepEmSBTableData* onHostTo_d = new G4HepEmSBTableData;
    gpuErrchk ( cudaMemcpy( onHostTo_d, *onDEVICE, sizeof( struct G4HepEmSBTableData ), cudaMemcpyDeviceToHost ) );
    // ELoss data
    cudaFree( onHostTo_d->fGammaCutIndxStartIndexPerMC );
    cudaFree( onHostTo_d->fGammaCutIndices             );
    cudaFree( onHostTo_d->fSBTableData                 );
    //
    // free the remaining device side electron data and set the host side ptr to null
    cudaFree( *onDEVICE );
    *onDEVICE = nullptr;

    delete onHostTo_d;
  }
}
#endif // G4HepEm_CUDA_BUILD
