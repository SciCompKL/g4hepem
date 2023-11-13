#include "ad_type.h"
#ifndef G4HepEmDataJsonIOImpl_H
#define G4HepEmDataJsonIOImpl_H

#include <exception>

#include "G4HepEmParameters.hh"
#include "G4HepEmData.hh"
#include "G4HepEmMatCutData.hh"
#include "G4HepEmElementData.hh"
#include "G4HepEmMaterialData.hh"
#include "G4HepEmElectronData.hh"
#include "G4HepEmSBTableData.hh"
#include "G4HepEmGammaData.hh"
#include "G4HepEmState.hh"

#include "nlohmann/json.hpp"

// for convenience
using json = nlohmann::json;

// As G4HepEm has a lot of dynamic C arrays, a minimal non-owning dynamic arrary
// type helps with serialization....
template <typename T>
struct dynamic_array
{
  int N   = 0;
  T* data = nullptr;

  dynamic_array() = default;

  dynamic_array(int n, T* d)
    : N(n)
    , data(d)
  {}

  T* begin() const { return data; }

  T* end() const { return data + N; }

  size_t size() const { return static_cast<size_t>(N); }
};

// This makes dynamic_array dual purpose as not just a view but a
// non-owning holder, so not ideal, but is internal so we can be careful
template <typename T>
dynamic_array<T> make_array(int n)
{
  if(n == 0)
  {
    return { 0, nullptr };
  }
  return { n, new T[n] };
}

template <typename T>
dynamic_array<T> make_span(int n, T* d)
{
  return { n, d };
}

// Free functions for static C arrays, from
// https://stackoverflow.com/questions/60328339/json-to-an-array-of-structs-in-nlohmann-json-lib
template <typename T, size_t N>
void to_json(json& j, T (&t)[N])
{
  for(size_t i = 0; i < N; ++i)
  {
    j.push_back(t[i]);
  }
}

template <typename T, size_t N>
void from_json(const json& j, T (&t)[N])
{
  if(j.size() != N)
  {
    throw std::runtime_error("JSON array size is different than expected");
  }
  size_t index = 0;
  for(auto& item : j)
  {
    using T_unq = std::remove_cv_t<T>;
    using T_noad = std::conditional_t<std::is_same_v<T_unq,G4double>, double, T>;
    T_noad data_noad;
    from_json(item, data_noad);
    t[index++] = data_noad;
  }
}

namespace nlohmann
{
  template <typename T>
  struct adl_serializer<dynamic_array<T>>
  {
    static void to_json(json& j, const dynamic_array<T>& d)
    {
      if(d.N == 0 || d.data == nullptr)
      {
        j = nullptr;
      }

      // Assumes a to_json(j, T)
      for(auto& elem : d)
      {
        using T_unq = std::remove_cv_t<T>;
        if constexpr(std::is_same_v<T_unq,G4double> && !std::is_same_v<T_unq,double>){
          j.push_back(elem.getValue());
        } else {
          j.push_back(elem);
        }
      }
    }

    static dynamic_array<T> from_json(const json& j)
    {
      if(j.is_null() || j.size() == 0)
      {
        return {};
      }

      auto d = make_array<T>(j.size());
      for(size_t i=0; i<j.size(); i++){
        d.begin()[i] = j.begin()[i];
      }
      return d;
    }
  };
}  // namespace nlohmann

// ===========================================================================
// --- G4HepEmParameters
namespace nlohmann
{
  // We *can* have direct to/from_json functions for G4HepEmParameters
  // as it is simple. Use of adl_serializer is *purely* for consistency
  // with other structures!
  // We only support pointers as that's the form G4HepEmData expects
  template <>
  struct adl_serializer<G4HepEmParameters*>
  {
    static void to_json(json& j, const G4HepEmParameters* d)
    {
      if(d == nullptr)
      {
        j = nullptr;
      }
      else
      {
        j["fElectronTrackingCut"]  = d->fElectronTrackingCut.getValue();
        j["fMinLossTableEnergy"]   = d->fMinLossTableEnergy.getValue();
        j["fMaxLossTableEnergy"]   = d->fMaxLossTableEnergy.getValue();
        j["fNumLossTableBins"]     = d->fNumLossTableBins;
        j["fFinalRange"]           = d->fFinalRange.getValue();
        j["fDRoverRange"]          = d->fDRoverRange.getValue();
        j["fLinELossLimit"]        = d->fLinELossLimit.getValue();
        j["fElectronBremModelLim"] = d->fElectronBremModelLim.getValue();
        j["fMSCRangeFactor"]       = d->fMSCRangeFactor.getValue();
        j["fMSCSafetyFactor"]      = d->fMSCSafetyFactor.getValue();
      }
    }

    static G4HepEmParameters* from_json(const json& j)
    {
      if(j.is_null())
      {
        return nullptr;
      }
      else
      {
        auto* d = new G4HepEmParameters;

        d->fElectronTrackingCut  = j.at("fElectronTrackingCut").get<double>();
        d->fMinLossTableEnergy   = j.at("fMinLossTableEnergy").get<double>();
        d->fMaxLossTableEnergy   = j.at("fMaxLossTableEnergy").get<double>();
        d->fNumLossTableBins     = j.at("fNumLossTableBins").get<int>();
        d->fFinalRange           = j.at("fFinalRange").get<double>();
        d->fDRoverRange          = j.at("fDRoverRange").get<double>();
        d->fLinELossLimit        = j.at("fLinELossLimit").get<double>();
        d->fElectronBremModelLim = j.at("fElectronBremModelLim").get<double>();
        d->fMSCRangeFactor       = j.at("fMSCRangeFactor").get<double>();
        d->fMSCSafetyFactor      = j.at("fMSCSafetyFactor").get<double>();
        return d;
      }
    }
  };
}  // namespace nlohmann

// ===========================================================================
// --- G4HepEmElementData
// Helpers
G4HepEmElemData* begin(const G4HepEmElementData& d) { return d.fElementData; }

G4HepEmElemData* end(const G4HepEmElementData& d)
{
  // Element array is indexed by Z, so has one extra element (Z=0)
  return d.fElementData + d.fMaxZet + 1;
}

namespace nlohmann
{
  // We *can* have direct to/from_json functions for G4HepEmElemData
  // as it is simple. Use of adl_serializer is *purely* for consistency
  // with other structures!
  template <>
  struct adl_serializer<G4HepEmElemData>
  {
    // JSON
    static void to_json(json& j, const G4HepEmElemData& d)
    {
      j["fZet"]          = d.fZet.getValue();
      j["fZet13"]        = d.fZet13.getValue();
      j["fZet23"]        = d.fZet23.getValue();
      j["fCoulomb"]      = d.fCoulomb.getValue();
      j["fLogZ"]         = d.fLogZ.getValue();
      j["fZFactor1"]     = d.fZFactor1.getValue();
      j["fDeltaMaxLow"]  = d.fDeltaMaxLow.getValue();
      j["fDeltaMaxHigh"] = d.fDeltaMaxHigh.getValue();
      j["fILVarS1"]      = d.fILVarS1.getValue();
      j["fILVarS1Cond"]  = d.fILVarS1Cond.getValue();
      j["fSandiaEnergies"] =
        make_span(d.fNumOfSandiaIntervals, d.fSandiaEnergies);
      j["fSandiaCoefficients"] =
        make_span(4 * d.fNumOfSandiaIntervals, d.fSandiaCoefficients);
      j["fKShellBindingEnergy"] = d.fKShellBindingEnergy.getValue();
    }

    static G4HepEmElemData from_json(const json& j)
    {
      G4HepEmElemData d;

      d.fZet = j.at("fZet").get<double>();
      d.fZet13 = j.at("fZet13").get<double>();
      d.fZet23 = j.at("fZet23").get<double>();
      d.fCoulomb = j.at("fCoulomb").get<double>();
      d.fLogZ = j.at("fLogZ").get<double>();
      d.fZFactor1 = j.at("fZFactor1").get<double>();
      d.fDeltaMaxLow = j.at("fDeltaMaxLow").get<double>();
      d.fDeltaMaxHigh = j.at("fDeltaMaxHigh").get<double>();
      d.fILVarS1 = j.at("fILVarS1").get<double>();
      d.fILVarS1Cond = j.at("fILVarS1Cond").get<double>();

      auto tmpSandiaEnergies =
        j.at("fSandiaEnergies").get<dynamic_array<G4double>>();
      d.fNumOfSandiaIntervals = tmpSandiaEnergies.N;
      d.fSandiaEnergies       = tmpSandiaEnergies.data;

      auto tmpSandiaCoefficients =
        j.at("fSandiaCoefficients").get<dynamic_array<G4double>>();
      d.fSandiaCoefficients = tmpSandiaCoefficients.data;

      d.fKShellBindingEnergy = j.at("fKShellBindingEnergy").get<double>();

      return d;
    }
  };

  template <>
  struct adl_serializer<G4HepEmElementData*>
  {
    static void to_json(json& j, const G4HepEmElementData* d)
    {
      if(d == nullptr)
      {
        j = nullptr;
      }
      else
      {
        // G4HepEmElementData stores *all* elements in memory, but
        // only those with fZet +ve are used in this setup so we just persist
        // those
        for(auto& elem : *d)
        {
          if(elem.fZet > 0.0)
          {
            j.push_back(elem);
          }
        }
      }
    }

    static G4HepEmElementData* from_json(const json& j)
    {
      if(j.is_null())
      {
        return nullptr;
      }
      else
      {
        G4HepEmElementData* p = nullptr;
        AllocateElementData(&p);
        if(j.size() > p->fMaxZet + 1)
        {
          FreeElementData(&p);
          throw std::runtime_error(
            "size of JSON array larger than G4HepEmElementData array");
        }
        // Read in by element and assign to the right index
        for(const auto& e : j)
        {
          auto tmpElem       = e.get<G4HepEmElemData>();
          int i              = static_cast<int>(tmpElem.fZet.getValue());
          p->fElementData[i] = tmpElem;
        }
        return p;
      }
    }
  };
}  // namespace nlohmann

// --- G4HepEmMaterialData
namespace nlohmann
{
  // We *can* have direct to/from_json functions for G4HepEmMatData
  // as it is simple. Use of adl_serializer is *purely* for consistency
  // with other structures! (Though it also helps in construction of
  // the dynamic arrays G4HepEmMatData holds on from_json)
  template <>
  struct adl_serializer<G4HepEmMatData>
  {
    static void to_json(json& j, const G4HepEmMatData& d)
    {
      j["fG4MatIndex"]  = d.fG4MatIndex;
      j["fElementVect"] = make_span(d.fNumOfElement, d.fElementVect);
      j["fNumOfAtomsPerVolumeVect"] =
        make_span(d.fNumOfElement, d.fNumOfAtomsPerVolumeVect);
      j["fDensity"]          = d.fDensity.getValue();
      j["fDensityCorfactor"] = d.fDensityCorFactor.getValue();
      j["fElectronDensity"]  = d.fElectronDensity.getValue();
      j["fRadiationLength"]  = d.fRadiationLength.getValue();
      j["fMeanExEnergy"]     = d.fMeanExEnergy.getValue();
      j["fSandiaEnergies"] =
        make_span(d.fNumOfSandiaIntervals, d.fSandiaEnergies);
      j["fSandiaCoefficients"] =
        make_span(4 * d.fNumOfSandiaIntervals, d.fSandiaCoefficients);

      j["fZeff"]      = d.fZeff.getValue();
      j["fZeff23"]    = d.fZeff23.getValue();
      j["fZeffSqrt"]  = d.fZeffSqrt.getValue();

      j["fUMSCPar"]         = d.fUMSCPar.getValue();
      ARR(d.fUMSCStepMinPars,tmp10,2);
      j["fUMSCStepMinPars"] = tmp10;
      ARR(d.fUMSCTailCoeff,tmp11,4);
      j["fUMSCTailCoeff"]   = tmp11;
      ARR(d.fUMSCThetaCoeff,tmp12,2);
      j["fUMSCThetaCoeff"]  = tmp12;

    }

    static G4HepEmMatData from_json(const json& j)
    {
      G4HepEmMatData d;

      d.fG4MatIndex = j.at("fG4MatIndex").get_to(d.fG4MatIndex);
      auto tmpElemVect = j.at("fElementVect").get<dynamic_array<int>>();
      d.fNumOfElement  = tmpElemVect.N;
      d.fElementVect   = tmpElemVect.data;

      auto tmpAtomsPerVolumeVect =
        j.at("fNumOfAtomsPerVolumeVect").get<dynamic_array<G4double>>();
      d.fNumOfAtomsPerVolumeVect = tmpAtomsPerVolumeVect.data;

      d.fDensity = j.at("fDensity").get<double>();
      d.fDensityCorFactor = j.at("fDensityCorfactor").get<double>();
      d.fElectronDensity = j.at("fElectronDensity").get<double>();
      d.fRadiationLength = j.at("fRadiationLength").get<double>();
      d.fMeanExEnergy = j.at("fMeanExEnergy").get<double>();

      auto tmpSandiaEnergies =
        j.at("fSandiaEnergies").get<dynamic_array<G4double>>();
      d.fNumOfSandiaIntervals = tmpSandiaEnergies.N;
      d.fSandiaEnergies       = tmpSandiaEnergies.data;

      auto tmpSandiaCoefficients =
        j.at("fSandiaCoefficients").get<dynamic_array<G4double>>();
      d.fSandiaCoefficients = tmpSandiaCoefficients.data;

      d.fZeff = j.at("fZeff").get<double>();
      d.fZeff23 = j.at("fZeff23").get<double>();
      d.fZeffSqrt = j.at("fZeffSqrt").get<double>();

      d.fUMSCPar = j.at("fUMSCPar").get<double>();
      BRR(d.fUMSCStepMinPars,tmp10,2);
      BRR(d.fUMSCTailCoeff,tmp11,4);
      BRR(d.fUMSCThetaCoeff,tmp12,2);
      j.at("fUMSCStepMinPars").get_to(tmp10);
      j.at("fUMSCTailCoeff").get_to(tmp11);
      j.at("fUMSCThetaCoeff").get_to(tmp12);
      CRR(d.fUMSCStepMinPars,tmp10,2);
      CRR(d.fUMSCTailCoeff,tmp11,4);
      CRR(d.fUMSCThetaCoeff,tmp12,2);

      return d;
    }
  };

  template <>
  struct adl_serializer<G4HepEmMaterialData*>
  {
    static void to_json(json& j, const G4HepEmMaterialData* d)
    {
      if(d == nullptr)
      {
        j = nullptr;
      }
      else
      {
        j["fNumG4Material"]   = d->fNumG4Material;
        j["fNumMaterialData"] = d->fNumMaterialData;
        j["fG4MatIndexToHepEmMatIndex"] =
          make_span(d->fNumG4Material, d->fG4MatIndexToHepEmMatIndex);
        j["fMaterialData"] = make_span(d->fNumMaterialData, d->fMaterialData);
      }
    }

    static G4HepEmMaterialData* from_json(const json& j)
    {
      if(j.is_null())
      {
        return nullptr;
      }
      else
      {
        auto tmpNumG4Mat   = j.at("fNumG4Material").get<int>();
        auto tmpNumMatData = j.at("fNumMaterialData").get<int>();

        // Allocate data with enough memory to hold the read in data
        G4HepEmMaterialData* d = nullptr;
        AllocateMaterialData(&d, tmpNumG4Mat, tmpNumMatData);

        auto tmpMatIndexVect = j.at("fG4MatIndexToHepEmMatIndex");
        std::copy(tmpMatIndexVect.begin(), tmpMatIndexVect.end(),
                  d->fG4MatIndexToHepEmMatIndex);
        auto tmpMatData = j.at("fMaterialData");
        std::copy(tmpMatData.begin(), tmpMatData.end(), d->fMaterialData);

        return d;
      }
    }
  };
}  // namespace nlohmann

// --- G4HepEmMatCutData
namespace nlohmann
{
  template <>
  struct adl_serializer<G4HepEmMCCData>
  {
    static void to_json(json& j, const G4HepEmMCCData& d)
    {
      j["fSecElProdCutE"]  = d.fSecElProdCutE.getValue();
      j["fSecGamProdCutE"] = d.fSecGamProdCutE.getValue();
      j["fLogSecGamCutE"]  = d.fLogSecGamCutE.getValue();
      j["fHepEmMatIndex"]  = d.fHepEmMatIndex;
      j["fG4MatCutIndex"]  = d.fG4MatCutIndex;
    }

    static G4HepEmMCCData from_json(const json& j)
    {
      G4HepEmMCCData d;

      d.fSecElProdCutE = j.at("fSecElProdCutE").get<double>();
      d.fSecGamProdCutE = j.at("fSecGamProdCutE").get<double>();
      d.fLogSecGamCutE = j.at("fLogSecGamCutE").get<double>();
      j.at("fHepEmMatIndex").get_to(d.fHepEmMatIndex);
      j.at("fG4MatCutIndex").get_to(d.fG4MatCutIndex);

      return d;
    }
  };

  template <>
  struct adl_serializer<G4HepEmMatCutData*>
  {
    static void to_json(json& j, const G4HepEmMatCutData* d)
    {
      if(d == nullptr)
      {
        j = nullptr;
      }
      else
      {
        j["fNumG4MatCuts"]  = d->fNumG4MatCuts;
        j["fNumMatCutData"] = d->fNumMatCutData;
        j["fG4MCIndexToHepEmMCIndex"] =
          make_span(d->fNumG4MatCuts, d->fG4MCIndexToHepEmMCIndex);
        j["fMatCutData"] = make_span(d->fNumMatCutData, d->fMatCutData);
      }
    }

    static G4HepEmMatCutData* from_json(const json& j)
    {
      if(j.is_null())
      {
        return nullptr;
      }
      else
      {
        auto tmpNumG4Cuts  = j.at("fNumG4MatCuts").get<int>();
        auto tmpNumMatCuts = j.at("fNumMatCutData").get<int>();

        // Allocate the new object using this info
        G4HepEmMatCutData* d = nullptr;
        AllocateMatCutData(&d, tmpNumG4Cuts, tmpNumMatCuts);

        auto tmpG4CutIndex = j.at("fG4MCIndexToHepEmMCIndex");
        std::copy(tmpG4CutIndex.begin(), tmpG4CutIndex.end(),
                  d->fG4MCIndexToHepEmMCIndex);

        auto tmpMCData = j.at("fMatCutData");
        std::copy(tmpMCData.begin(), tmpMCData.end(), d->fMatCutData);

        return d;
      }
    }
  };
}  // namespace nlohmann

// --- G4HepEmElectronData
namespace nlohmann
{
  template <>
  struct adl_serializer<G4HepEmElectronData*>
  {
    static void to_json(json& j, G4HepEmElectronData* d)
    {
      if(d == nullptr)
      {
        j = nullptr;
      }
      else
      {
        j["fNumMatCuts"]      = d->fNumMatCuts;
        j["fNumMaterials"]    = d->fNumMaterials;
        j["fELossLogMinEkin"] = d->fELossLogMinEkin.getValue();
        j["fELossEILDelta"]   = d->fELossEILDelta.getValue();

        j["fELossEnergyGrid"] =
          make_span(d->fELossEnergyGridSize, d->fELossEnergyGrid);

        const int nELoss = 5 * (d->fELossEnergyGridSize) * (d->fNumMatCuts);
        j["fELossData"]  = make_span(nELoss, d->fELossData);

        j["fResMacXSecStartIndexPerMatCut"] =
          make_span(d->fNumMatCuts, d->fResMacXSecStartIndexPerMatCut);
        j["fResMacXSecData"] =
          make_span(d->fResMacXSecNumData, d->fResMacXSecData);

        const int nTr1MacXsec = 2 * (d->fELossEnergyGridSize) * (d->fNumMaterials);
        j["fTr1MacXSecData"] =
          make_span(nTr1MacXsec, d->fTr1MacXSecData);

        j["fElemSelectorIoniStartIndexPerMatCut"] =
          make_span(d->fNumMatCuts, d->fElemSelectorIoniStartIndexPerMatCut);
        j["fElemSelectorIoniData"] =
          make_span(d->fElemSelectorIoniNumData, d->fElemSelectorIoniData);

        j["fElemSelectorBremSBStartIndexPerMatCut"] =
          make_span(d->fNumMatCuts, d->fElemSelectorBremSBStartIndexPerMatCut);
        j["fElemSelectorBremSBData"] =
          make_span(d->fElemSelectorBremSBNumData, d->fElemSelectorBremSBData);

        j["fElemSelectorBremRBStartIndexPerMatCut"] =
          make_span(d->fNumMatCuts, d->fElemSelectorBremRBStartIndexPerMatCut);
        j["fElemSelectorBremRBData"] =
          make_span(d->fElemSelectorBremRBNumData, d->fElemSelectorBremRBData);
      }
    }

    static G4HepEmElectronData* from_json(const json& j)
    {
      if(j.is_null())
      {
        return nullptr;
      }
      else
      {
        G4HepEmElectronData* d = nullptr;
        AllocateElectronData(&d);

        j.at("fNumMatCuts").get_to(d->fNumMatCuts);
        j.at("fNumMaterials").get_to(d->fNumMaterials);
        d->fELossLogMinEkin = j.at("fELossLogMinEkin").get<double>();
        d->fELossEILDelta = j.at("fELossEILDelta").get<double>();

        auto tmpElossGrid =
          j.at("fELossEnergyGrid").get<dynamic_array<G4double>>();
        d->fELossEnergyGridSize = tmpElossGrid.N;
        d->fELossEnergyGrid     = tmpElossGrid.data;

        auto tmpELossData = j.at("fELossData").get<dynamic_array<G4double>>();
        d->fELossData     = tmpELossData.data;
        // To validate, tmpELossData == 5 * (d->fELossEnergyGridSize) *
        // (d->fNumMatCuts);
        {
          auto tmpIndex =
            j.at("fResMacXSecStartIndexPerMatCut").get<dynamic_array<int>>();
          d->fResMacXSecStartIndexPerMatCut = tmpIndex.data;
          // To validate, tmpIndex.N == d->fNumMatCuts;

          auto tmpData = j.at("fResMacXSecData").get<dynamic_array<G4double>>();
          d->fResMacXSecNumData = tmpData.N;
          d->fResMacXSecData    = tmpData.data;

          auto tmpTr1Data = j.at("fTr1MacXSecData").get<dynamic_array<G4double>>();
          d->fTr1MacXSecData    = tmpTr1Data.data;
        }

        {
          auto tmpIndex = j.at("fElemSelectorIoniStartIndexPerMatCut")
                            .get<dynamic_array<int>>();
          d->fElemSelectorIoniStartIndexPerMatCut = tmpIndex.data;
          // To validate, tmpIndex.N == d->fNumMatCuts;

          auto tmpData =
            j.at("fElemSelectorIoniData").get<dynamic_array<G4double>>();
          d->fElemSelectorIoniNumData = tmpData.N;
          d->fElemSelectorIoniData    = tmpData.data;
        }

        {
          auto tmpIndex = j.at("fElemSelectorBremSBStartIndexPerMatCut")
                            .get<dynamic_array<int>>();
          d->fElemSelectorBremSBStartIndexPerMatCut = tmpIndex.data;
          // To validate, tmpIndex.N == d->fNumMatCuts;

          auto tmpData =
            j.at("fElemSelectorBremSBData").get<dynamic_array<G4double>>();
          d->fElemSelectorBremSBNumData = tmpData.N;
          d->fElemSelectorBremSBData    = tmpData.data;
        }

        {
          auto tmpIndex = j.at("fElemSelectorBremRBStartIndexPerMatCut")
                            .get<dynamic_array<int>>();
          d->fElemSelectorBremRBStartIndexPerMatCut = tmpIndex.data;
          // To validate, tmpIndex.N == d->fNumMatCuts;

          auto tmpData =
            j.at("fElemSelectorBremRBData").get<dynamic_array<G4double>>();
          d->fElemSelectorBremRBNumData = tmpData.N;
          d->fElemSelectorBremRBData    = tmpData.data;
        }

        return d;
      }
    }
  };
}  // namespace nlohmann

// --- G4HepEmSBTableData
namespace nlohmann
{
  template <>
  struct adl_serializer<G4HepEmSBTableData*>
  {
    static void to_json(json& j, const G4HepEmSBTableData* d)
    {
      if(d == nullptr)
      {
        j = nullptr;
      }
      else
      {
        j["fLogMinElEnergy"]  = d->fLogMinElEnergy.getValue();
        j["fILDeltaElEnergy"] = d->fILDeltaElEnergy.getValue();
        ARR(d->fElEnergyVect,tmp10,65);
        j["fElEnergyVect"]    = tmp10;
        ARR(d->fLElEnergyVect,tmp11,65);
        j["fLElEnergyVect"]   = tmp11;
        ARR(d->fKappaVect,tmp12,54);
        j["fKappaVect"]       = tmp12;
        ARR(d->fLKappaVect,tmp13,54);
        j["fLKappaVect"]      = tmp13;

        j["fGammaCutIndxStartIndexPerMC"] =
          make_span(d->fNumHepEmMatCuts, d->fGammaCutIndxStartIndexPerMC);

        j["fGammaCutIndices"] =
          make_span(d->fNumElemsInMatCuts, d->fGammaCutIndices);

        j["fSBStartTablesStartPerZ"] = d->fSBTablesStartPerZ;
        j["fSBTableData"] = make_span(d->fNumSBTableData, d->fSBTableData);
      }
    }

    static G4HepEmSBTableData* from_json(const json& j)
    {
      if(j.is_null())
      {
        return nullptr;
      }
      else
      {
        G4HepEmSBTableData* d = nullptr;

        // Reading arrays first so we can allocate/copy directly
        // fNumHepEmMatCuts
        auto tmpGammaCutStartIndices = j.at("fGammaCutIndxStartIndexPerMC");
        // fNumElemsInMatCuts
        auto tmpGammaCutIndices = j.at("fGammaCutIndices");
        // fNumSBTableData
        auto tmpSBTableData = j.at("fSBTableData");

        AllocateSBTableData(&d, tmpGammaCutStartIndices.size(),
                            tmpGammaCutIndices.size(), tmpSBTableData.size());

        // copy JSON arrays to newly allocated SB arrays
        std::copy(tmpGammaCutStartIndices.begin(),
                  tmpGammaCutStartIndices.end(),
                  d->fGammaCutIndxStartIndexPerMC);
        std::copy(tmpGammaCutIndices.begin(), tmpGammaCutIndices.end(),
                  d->fGammaCutIndices);
        std::copy(tmpSBTableData.begin(), tmpSBTableData.end(),
                  d->fSBTableData);

        // Now remaining data
        d->fLogMinElEnergy = j.at("fLogMinElEnergy").get<double>();
        d->fILDeltaElEnergy = j.at("fILDeltaElEnergy").get<double>();
        BRR(d->fElEnergyVect,tmp10,65);
        j.at("fElEnergyVect").get_to(tmp10);
        CRR(d->fElEnergyVect,tmp10,65);
        BRR(d->fLElEnergyVect,tmp11,65);
        j.at("fLElEnergyVect").get_to(tmp11);
        CRR(d->fLElEnergyVect,tmp11,65);
        BRR(d->fKappaVect,tmp12,54);
        j.at("fKappaVect").get_to(tmp12);
        CRR(d->fKappaVect,tmp12,54);
        BRR(d->fLKappaVect,tmp13,54);
        j.at("fLKappaVect").get_to(tmp13);
        CRR(d->fLKappaVect,tmp13,54);

        j.at("fSBStartTablesStartPerZ").get_to(d->fSBTablesStartPerZ);

        return d;
      }
    }
  };
}  // namespace nlohmann

// --- G4HepEmGammaData
namespace nlohmann
{
  template <>
  struct adl_serializer<G4HepEmGammaData*>
  {
    static void to_json(json& j, const G4HepEmGammaData* d)
    {
      if(d == nullptr)
      {
        j = nullptr;
      }
      else
      {
        /** Number of G4HepEm materials: number of G4HepEmMatData structures
         * stored in the G4HepEmMaterialData::fMaterialData array. */
        j["fNumMaterials"] = d->fNumMaterials;

        //// === conversion related data. Grid: 146 bins form 2mc^2 - 100 TeV
        j["fConvLogMinEkin"] = d->fConvLogMinEkin.getValue();
        j["fConvEILDelta"]   = d->fConvEILDelta.getValue();
        j["fConvEnergyGrid"] =
          make_span(d->fConvEnergyGridSize, d->fConvEnergyGrid);

        //// === compton related data. 84 bins (7 per decades) from 100 eV - 100
        /// TeV
        j["fCompLogMinEkin"] = d->fCompLogMinEkin.getValue();
        j["fCompEILDelta"]   = d->fCompEILDelta.getValue();
        j["fCompEnergyGrid"] =
          make_span(d->fCompEnergyGridSize, d->fCompEnergyGrid);

        const int macXsecDataSize =
          d->fNumMaterials * 2 *
          (d->fConvEnergyGridSize + d->fCompEnergyGridSize);
        j["fConvCompMacXsecData"] =
          make_span(macXsecDataSize, d->fConvCompMacXsecData);

        //// === element selector for conversion (note: KN compton interaction
        /// do not know anything about Z)
        j["fElemSelectorConvLogMinEkin"] = d->fElemSelectorConvLogMinEkin.getValue();
        j["fElemSelectorConvEILDelta"]   = d->fElemSelectorConvEILDelta.getValue();
        j["fElemSelectorConvStartIndexPerMat"] =
          make_span(d->fNumMaterials, d->fElemSelectorConvStartIndexPerMat);

        j["fElemSelectorConvEgrid"] =
          make_span(d->fElemSelectorConvEgridSize, d->fElemSelectorConvEgrid);

        j["fElemSelectorConvData"] =
          make_span(d->fElemSelectorConvNumData, d->fElemSelectorConvData);
      }
    }

    static G4HepEmGammaData* from_json(const json& j)
    {
      if(j.is_null())
      {
        return nullptr;
      }
      else
      {
        G4HepEmGammaData* d = nullptr;
        AllocateGammaData(&d);

        j.at("fNumMaterials").get_to(d->fNumMaterials);

        d->fConvLogMinEkin = j.at("fConvLogMinEkin").get<double>();
        d->fConvEILDelta = j.at("fConvEILDelta").get<double>();
        // Get the array but ignore the size (fConvEnergyGridSize) as this is a
        // const (at time of writing)
        auto tmpConvEnergyGrid =
          j.at("fConvEnergyGrid").get<dynamic_array<G4double>>();
        d->fConvEnergyGrid = tmpConvEnergyGrid.data;

        d->fCompLogMinEkin = j.at("fCompLogMinEkin").get<double>();
        d->fCompEILDelta = j.at("fCompEILDelta").get<double>();
        // Get the array but ignore the size (fCompEnergyGridSize) as this is a
        // const (at time of writing)
        auto tmpCompEnergyGrid =
          j.at("fCompEnergyGrid").get<dynamic_array<G4double>>();
        d->fCompEnergyGrid = tmpCompEnergyGrid.data;

        // We don't store the size of the following array, rather should
        // validate that it is expected size: d->fNumMaterials * 2 *
        // (d->fConvEnergyGridSize + d->fCompEnergyGridSize);
        auto tmpConvCompXsecData =
          j.at("fConvCompMacXsecData").get<dynamic_array<G4double>>();
        d->fConvCompMacXsecData = tmpConvCompXsecData.data;

        d->fElemSelectorConvLogMinEkin = j.at("fElemSelectorConvLogMinEkin")
          .get<double>();
        d->fElemSelectorConvEILDelta = j.at("fElemSelectorConvEILDelta").get<double>();

        // size of this array is d->fNumMaterial, which we store separately for
        // now
        auto tmpConvStartIndexPerMat =
          j.at("fElemSelectorConvStartIndexPerMat").get<dynamic_array<int>>();
        d->fElemSelectorConvStartIndexPerMat = tmpConvStartIndexPerMat.data;

        auto tmpConvEgrid =
          j.at("fElemSelectorConvEgrid").get<dynamic_array<G4double>>();
        d->fElemSelectorConvEgridSize = tmpConvEgrid.N;
        d->fElemSelectorConvEgrid     = tmpConvEgrid.data;

        auto tmpConvData =
          j.at("fElemSelectorConvData").get<dynamic_array<G4double>>();
        d->fElemSelectorConvNumData = tmpConvData.N;
        d->fElemSelectorConvData    = tmpConvData.data;

        return d;
      }
    }
  };
}  // namespace nlohmann

// --- G4HepEmData
namespace nlohmann
{
  template <>
  struct adl_serializer<G4HepEmData*>
  {
    static void to_json(json& j, const G4HepEmData* d)
    {
      if(d == nullptr)
      {
        j = nullptr;
      }
      else
      {
        j["fTheMatCutData"]   = d->fTheMatCutData;
        j["fTheMaterialData"] = d->fTheMaterialData;
        j["fTheElementData"]  = d->fTheElementData;
        j["fTheElectronData"] = d->fTheElectronData;
        j["fThePositronData"] = d->fThePositronData;
        j["fTheSBTableData"]  = d->fTheSBTableData;
        j["fTheGammaData"]    = d->fTheGammaData;
      }
    }

    static G4HepEmData* from_json(const json& j)
    {
      if(j.is_null())
      {
        return nullptr;
      }
      else
      {
        G4HepEmData* d    = new G4HepEmData;
        d->fTheMatCutData = j.at("fTheMatCutData").get<G4HepEmMatCutData*>();
        d->fTheMaterialData =
          j.at("fTheMaterialData").get<G4HepEmMaterialData*>();
        d->fTheElementData = j.at("fTheElementData").get<G4HepEmElementData*>();
        d->fTheElectronData =
          j.at("fTheElectronData").get<G4HepEmElectronData*>();
        d->fThePositronData =
          j.at("fThePositronData").get<G4HepEmElectronData*>();
        d->fTheSBTableData = j.at("fTheSBTableData").get<G4HepEmSBTableData*>();
        d->fTheGammaData   = j.at("fTheGammaData").get<G4HepEmGammaData*>();
        return d;
      }
    }
  };
}  // namespace nlohmann

// --- G4HepEmState
namespace nlohmann
{
  template <>
  struct adl_serializer<G4HepEmState*>
  {
    static void to_json(json& j, const G4HepEmState* d)
    {
      if(d == nullptr)
      {
        j = nullptr;
      }
      else
      {
        j["fParameters"] = d->fParameters;
        j["fData"]       = d->fData;
      }
    }

    static G4HepEmState* from_json(const json& j)
    {
      if(j.is_null())
      {
        return nullptr;
      }
      else
      {
        G4HepEmState* d = new G4HepEmState;
        d->fParameters  = j.at("fParameters").get<G4HepEmParameters*>();
        d->fData        = j.at("fData").get<G4HepEmData*>();
        return d;
      }
    }
  };
}  // namespace nlohmann

#endif  // G4HepEmJsonSerialization_H
