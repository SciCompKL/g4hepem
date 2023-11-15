#ifndef AD_TYPE_H
#define AD_TYPE_H

#if defined(CODI_FORWARD)
#include "codi.hpp"
using G4double = codi::RealForward;
#define GET_GRADIENT(var) ((var).getGradient())
#define GET_VALUE(var) (((var)).getValue())
#define SET_GRADIENT(var,dotval) (var).setGradient(dotval);
#endif

#if !defined(CODI_FORWARD)
using G4double = double;
#define GET_GRADIENT(var) 0.0
#define GET_VALUE(var) (var)
#define SET_GRADIENT(var, dotval)
#endif

#define ARR(ad_array,new_array,size) \
  double new_array[size]; \
  for(int i=0; i<size; i++) \
    new_array[i] = GET_VALUE(ad_array[i]);

#define BRR(ad_array,new_array,size) \
  double new_array[size];

#define CRR(ad_array,new_array,size) \
  for(int i=0; i<size; i++) \
    ad_array[i] = new_array[i];

#endif
