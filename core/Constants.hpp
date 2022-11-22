/* Copyright (C) 2019-2021 GSI, Universität Tübingen, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Eugeny Kashirin, Ilya Selyuzhenkov */
#ifndef BASE_CONSTANTS_H
#define BASE_CONSTANTS_H

#include <Rtypes.h>

#ifndef __has_cpp_attribute// if we don't have __has_attribute, ignore it
#define __has_cpp_attribute(x) 0
#endif

#if __has_cpp_attribute(deprecated)
#define ANALYSISTREE_ATTR_DEPRECATED(MESSAGE) [[deprecated]]
#else
#define ANALYSISTREE_ATTR_DEPRECATED(MESSAGE)// attribute "deprecated" not available
#endif

#if __has_cpp_attribute(nodiscard)
#define ANALYSISTREE_ATTR_NODISCARD [[nodiscard]]
#else
#define ANALYSISTREE_ATTR_NODISCARD
#endif

namespace AnalysisTree {

typedef Float_t Floating_t;
typedef Int_t Integer_t;
typedef UInt_t UInteger_t;
typedef Short_t ShortInt_t;
typedef Long64_t PdgCode_t;

constexpr Floating_t UndefValueFloat = -999.;
constexpr ShortInt_t UndefValueShort = -999;
constexpr Integer_t UndefValueInt = -999;
constexpr double SmallNumber = 1e-6;

namespace Exyz {
enum Exyz : ShortInt_t {
  kX = 0,
  kY,
  kZ
};
}

enum class DetType : ShortInt_t {
  kHit = 0,
  kModule,
  kTrack,
  kEventHeader,
  kParticle
};

enum class Types : ShortInt_t {
  kFloat = 0,
  kInteger,
  kBool,
  kNumberOfTypes
};

namespace TrackFields {
enum TrackFields : ShortInt_t {
  kPhi = -1,
  kPt = -2,
  kEta = -3,
  kPx = -4,
  kPy = -5,
  kPz = -6,
  kP = -7,
  kId = -8
};
}

namespace ParticleFields {
enum ParticleFields : ShortInt_t {
  kPhi = -1,
  kPt = -2,
  kRapidity = -3,
  kPid = -4,
  kMass = -5,
  kEta = -6,
  kPx = -7,
  kPy = -8,
  kPz = -9,
  kP = -10,
  kEnergy = -11,
  kKineticEnergy = -12,
  kId = -13
};
}

namespace HitFields {
enum HitFields : ShortInt_t {
  kPhi = -1,
  kSignal = -2,
  kX = -3,
  kY = -4,
  kZ = -5,
  kId = -6
};
}

namespace ModuleFields {
enum ModuleFields : ShortInt_t {
  kNumber = -1,
  kSignal = -2,
  kId = -3
};
}

namespace EventHeaderFields {
enum EventHeaderFields : ShortInt_t {
  kVertexX = -1,
  kVertexY = -2,
  kVertexZ = -3,
  kId = -4
};
}

}// namespace AnalysisTree

#endif
