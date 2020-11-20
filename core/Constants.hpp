#ifndef BASE_CONSTANTS_H
#define BASE_CONSTANTS_H

#include <iostream>
#include <map>

#include <Rtypes.h>

namespace AnalysisTree {

typedef Float_t Floating_t;
typedef Int_t Integer_t;
typedef UInt_t UInteger_t;
typedef Short_t ShortInt_t;
typedef Long64_t PdgCode_t;

constexpr Floating_t UndefValueFloat = -999.;
constexpr ShortInt_t UndefValueShort = -999;
constexpr Integer_t UndefValueInt = -999;
constexpr Floating_t SmallNumber = 1e-6f;

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
  kP = -7
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
  kP = -10
};
}

namespace HitFields {
enum HitFields : ShortInt_t {
  kPhi = -1,
  kSignal = -2,
  kX = -3,
  kY = -4,
  kZ = -5
};
}

namespace ModuleFields {
enum ModuleFields : ShortInt_t {
  kNumber = -1,
  kSignal = -2
};
}

namespace EventHeaderFields {
enum EventHeaderFields : ShortInt_t {
  kVertexX = -1,
  kVertexY = -2,
  kVertexZ = -3
};
}

}// namespace AnalysisTree

#endif