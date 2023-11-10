#ifndef AD_TYPE_H
#define AD_TYPE_H
#include "codi.hpp"


using G4double = codi::RealForward;


#define ARR(ad_array,new_array,size) \
  double new_array[size]; \
  for(int i=0; i<size; i++) \
    new_array[i] = ad_array[i].getValue(); 

#define BRR(ad_array,new_array,size) \
  double new_array[size]; 

#define CRR(ad_array,new_array,size) \
  for(int i=0; i<size; i++) \
    ad_array[i] = new_array[i];
  


#endif
