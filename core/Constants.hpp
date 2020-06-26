#ifndef BASE_CONSTANTS_H
#define BASE_CONSTANTS_H

#include <map>
#include <iostream>

#include <Rtypes.h>

//#include <TDatabasePDG.h>
//class TDataBasePDG;

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

const std::map<int, float> MassMap = { {11, 0.000511},        // e
                                       {211, 0.13957},        // pi^{+,-}
                                       {111, 0.13498},        // pi^0
                                       {130, 0.4976},
                                       {310, 0.4976},
                                       {311, 0.4976},
                                       {321, 0.493677},       // K^{+,-}
                                       {2212, 0.938272},      // p, anti-p
                                       {2112, 0.939565},      // n
                                       {3122, 1.115683} };    // Lambda(-bar)

[[maybe_unused]] static float GetMassByPdgId(PdgCode_t pid) {
  /* 100ZZZAAA0 */
  if (pid > 1000000000) {
    auto A = (pid % 10000) / 10;
    return A * 0.938f /* GeV */;
  }
  auto it = MassMap.find(std::abs(pid));
  if(it != MassMap.end()){
    return MassMap.find(std::abs(pid))->second;
  }
  else{
    //for not only print warning not to break all converters //TODO exeption
    std::cout << "WARINING! Mass of " + std::to_string(pid) + " is not known" << std::endl;
    return 0.f;
    throw std::runtime_error("Mass of " + std::to_string(pid) + " is not known");
  }
//TODO wtf??
//  auto db = TDatabasePDG::Instance();
//  auto particle = db->GetParticle(pid);
//
//  if (particle) {
//    return particle->Mass();
//  }
}




namespace Exyz{
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

enum class Types : ShortInt_t
{
  kFloat = 0,
  kInteger,
  kBool,
  kNumberOfTypes
};

namespace TrackFields{
  enum TrackFields : ShortInt_t
  {
    kPhi = -1,
    kPt = -2,
    kEta = -3,
    kPx = -4,
    kPy = -5,
    kPz = -6,
    kP = -7
  };
}

namespace ParticleFields{
enum ParticleFields : ShortInt_t
{
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

namespace HitFields{
  enum HitFields : ShortInt_t
  {
    kPhi = -1,
    kSignal = -2,
    kX = -3,
    kY = -4,
    kZ = -5
  };
}

namespace ModuleFields{
  enum ModuleFields : ShortInt_t
  {
    kId = -1,
    kSignal = -2
  };
}

namespace EventHeaderFields{
  enum EventHeaderFields : ShortInt_t
  {
    kVertexX = -1,
    kVertexY = -2,
    kVertexZ = -3
  };
}


}

#endif