#include "ad_type.h"

#ifndef G4HepEmMath_HH
#define G4HepEmMath_HH

#include <cmath>

#include "G4HepEmExp.hh"
#include "G4HepEmLog.hh"

#include "G4HepEmMacros.hh"

#define G4HepEmMax std::max
#define G4HepEmMin std::min

template <typename T>
G4HepEmHostDevice static inline
auto G4HepEmX13(T x) -> decltype(std::pow(x,1./3.)) {
 return std::pow(x, 1./3.);
}

// --- Log function with VDT (G4Log) specialisations for G4double and float
template <typename T>
G4HepEmHostDevice inline
auto G4HepEmLog(T x) -> decltype(std::log(x)) {
 return std::log(x);
}
// use the specialisations only on the host
#if 0
template < >
inline
G4double G4HepEmLog(G4double x) {
 return VDTLog(x);
}
template < >
inline
float G4HepEmLog(float x) {
 return VDTLogf(x);
}
#endif // ndef __CUDA_ARCH__

// --- Exp function with VDT (G4Exp) specialisations for G4double and float
template <typename T>
G4HepEmHostDevice inline
auto G4HepEmExp(T x) -> decltype(std::exp(x)) {
 return std::exp(x);
}
// use the specialisations only on the host
#if 0
template < >
inline
G4double G4HepEmExp(G4double x) {
 return VDTExp(x);
}
template < >
inline
float G4HepEmExp(float x) {
 return VDTExpf(x);
}
#endif // ndef __CUDA_ARCH__

// --- Pow(x,a) function with the VDT (G4) Exp and Log specialisations for G4double and float
template <typename S, typename T>
G4HepEmHostDevice inline
auto G4HepEmPow(S x, T a) -> decltype(std::pow(x,a)) {
 return std::pow(x, a);
}
// use the specialisations only on the host
#if 0
template < >
inline
G4double G4HepEmPow(G4double x, G4double a) {
 return VDTExp(a*VDTLog(x));
}
template < >
inline
float G4HepEmPow(float x, float a) {
 return VDTExpf(a*VDTLogf(x));
}
#endif // ndef __CUDA_ARCH__

#endif // G4HepEmMath_HH
