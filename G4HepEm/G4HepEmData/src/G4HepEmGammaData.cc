#include "G4HepEmGammaData.hh"

// NOTE: allocates only the main data structure but not the dynamic members
void AllocateGammaData (struct G4HepEmGammaData** theGammaData) {
  // clean away previous (if any)
  FreeGammaData(theGammaData);
  *theGammaData   = MakeGammaData();
}

G4HepEmGammaData* MakeGammaData() {
  // Default construction handles everything we need, but add
  // additional initialization here if required
  return new G4HepEmGammaData;
}

void FreeGammaData (struct G4HepEmGammaData** theGammaData)  {
  if (*theGammaData != nullptr) {
    delete[] (*theGammaData)->fConvEnergyGrid ;
    delete[] (*theGammaData)->fCompEnergyGrid;
    delete[] (*theGammaData)->fConvCompMacXsecData;
    delete[] (*theGammaData)->fElemSelectorConvStartIndexPerMat;
    delete[] (*theGammaData)->fElemSelectorConvEgrid;
    delete[] (*theGammaData)->fElemSelectorConvData;
    delete *theGammaData;
    *theGammaData = nullptr;
  }
}


#ifdef G4HepEm_CUDA_BUILD
#include <cuda_runtime.h>
#include "G4HepEmCuUtils.hh"

#include <cstring>

void CopyGammaDataToDevice(struct G4HepEmGammaData* onHOST, struct G4HepEmGammaData** onDEVICE) {
  if ( !onHOST ) return;
  // clean away previous (if any)
  if ( *onDEVICE ) {
    FreeGammaDataOnDevice ( onDEVICE );
  }
  // Create a G4HepEmGammaData structure on the host to store pointers to _d
  // side arrays on the _h side.
  struct G4HepEmGammaData* gmDataHTo_d = new G4HepEmGammaData;
  // Set non-pointer members via a memcpy of the entire structure.
  memcpy(gmDataHTo_d, onHOST, sizeof(G4HepEmGammaData));
  // get and set number of materials
  int numHepEmMat = onHOST->fNumMaterials;
  // -- go for the conversion related data
  int numConvData = onHOST->fConvEnergyGridSize;
  // allocate memory on _d for the conversion energy grid and copy them form _h
  gpuErrchk ( cudaMalloc ( &(gmDataHTo_d->fConvEnergyGrid), sizeof( G4double ) * numConvData ) );
  gpuErrchk ( cudaMemcpy (   gmDataHTo_d->fConvEnergyGrid,  onHOST->fConvEnergyGrid, sizeof( G4double ) * numConvData, cudaMemcpyHostToDevice ) );
  // -- go for the Compton related data
  int numCompData = onHOST->fCompEnergyGridSize;
  // allocate memory on _d for the Compton energy grid and copy them form _h
  gpuErrchk ( cudaMalloc ( &(gmDataHTo_d->fCompEnergyGrid), sizeof( G4double ) * numCompData ) );
  gpuErrchk ( cudaMemcpy (   gmDataHTo_d->fCompEnergyGrid,  onHOST->fCompEnergyGrid, sizeof( G4double ) * numCompData, cudaMemcpyHostToDevice ) );
  // allocate memory on _d for the conversion and Compton macroscopic x-section data and copy them form _h
  int numConvCompData = numHepEmMat*2*(numConvData+numCompData);
  gpuErrchk ( cudaMalloc ( &(gmDataHTo_d->fConvCompMacXsecData), sizeof( G4double ) * numConvCompData ) );
  gpuErrchk ( cudaMemcpy (   gmDataHTo_d->fConvCompMacXsecData,  onHOST->fConvCompMacXsecData, sizeof( G4double ) * numConvCompData, cudaMemcpyHostToDevice ) );
  //
  // -- go for the conversion element selector related data
  int numElSelE   = onHOST->fElemSelectorConvEgridSize;
  int numElSelDat = onHOST->fElemSelectorConvNumData;
  if (numElSelDat > 0) {
    gpuErrchk ( cudaMalloc ( &(gmDataHTo_d->fElemSelectorConvStartIndexPerMat), sizeof( int ) * numHepEmMat ) );
    gpuErrchk ( cudaMemcpy (   gmDataHTo_d->fElemSelectorConvStartIndexPerMat,  onHOST->fElemSelectorConvStartIndexPerMat, sizeof( int ) * numHepEmMat, cudaMemcpyHostToDevice ) );
    gpuErrchk ( cudaMalloc ( &(gmDataHTo_d->fElemSelectorConvEgrid), sizeof( G4double ) * numElSelE ) );
    gpuErrchk ( cudaMemcpy (   gmDataHTo_d->fElemSelectorConvEgrid,  onHOST->fElemSelectorConvEgrid, sizeof( G4double ) * numElSelE,   cudaMemcpyHostToDevice ) );
    gpuErrchk ( cudaMalloc ( &(gmDataHTo_d->fElemSelectorConvData),  sizeof( G4double ) * numElSelDat ) );
    gpuErrchk ( cudaMemcpy (   gmDataHTo_d->fElemSelectorConvData,   onHOST->fElemSelectorConvData,  sizeof( G4double ) * numElSelDat, cudaMemcpyHostToDevice ) );
  } else {
    gmDataHTo_d->fElemSelectorConvStartIndexPerMat = nullptr;
    gmDataHTo_d->fElemSelectorConvEgrid = nullptr;
    gmDataHTo_d->fElemSelectorConvData = nullptr;
  }
  //
  // Finaly copy the top level, i.e. the main struct with the already
  // appropriate pointers to device side memory locations but stored on the host
  gpuErrchk ( cudaMalloc (  onDEVICE,              sizeof(  struct G4HepEmGammaData ) ) );
  gpuErrchk ( cudaMemcpy ( *onDEVICE, gmDataHTo_d, sizeof(  struct G4HepEmGammaData ), cudaMemcpyHostToDevice ) );
  // and clean
  delete gmDataHTo_d;
}

void FreeGammaDataOnDevice(struct G4HepEmGammaData** onDEVICE) {
  if (*onDEVICE) {
    // copy the on-device data back to host in order to be able to free the device
    // side dynamically allocated memories
    struct G4HepEmGammaData* onHostTo_d = new G4HepEmGammaData;
    gpuErrchk ( cudaMemcpy( onHostTo_d, *onDEVICE, sizeof( struct G4HepEmGammaData ), cudaMemcpyDeviceToHost ) );
    // conversion and Compton macroscopic x-section related data
    cudaFree( onHostTo_d->fConvEnergyGrid );
    cudaFree( onHostTo_d->fCompEnergyGrid );
    cudaFree( onHostTo_d->fConvCompMacXsecData );
    // conversion element selector related data
    cudaFree( onHostTo_d->fElemSelectorConvStartIndexPerMat );
    cudaFree( onHostTo_d->fElemSelectorConvEgrid );
    cudaFree( onHostTo_d->fElemSelectorConvData );
    //
    // free the remaining device side gamma data and set the host side ptr to null
    cudaFree( *onDEVICE );
    *onDEVICE = nullptr;
    // delete auxiliary object
    delete onHostTo_d;
  }
}

#endif
