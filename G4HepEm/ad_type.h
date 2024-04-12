#ifndef AD_TYPE_H
#define AD_TYPE_H

#if defined(EASYAD_FORWARD)
#include "easyAD.hpp"
using G4double = Forward;
#define GET_DOTVALUE(var) ((var).dot)
#define GET_VALUE(var) ((var).val)
#define SET_DOTVALUE(var,dotval) (var).dot = (dotval);
#endif

#if defined(CODI_FORWARD)
#include "codi.hpp"
using G4double = codi::RealForward;
#define GET_DOTVALUE(var) ((var).getGradient())
#define GET_VALUE(var) (((var)).getValue())
#define SET_DOTVALUE(var,dotval) (var).setGradient(dotval);
#endif

#if defined(CODI_REVERSE)
#include "codi.hpp"
using G4double = codi::RealReverse;
#define GET_VALUE(var) (((var)).getValue())
#endif

#if !defined(EASYAD_FORWARD) && !defined(CODI_FORWARD) && !defined(CODI_REVERSE)
using G4double = double;
#define GET_VALUE(var) (var)
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
