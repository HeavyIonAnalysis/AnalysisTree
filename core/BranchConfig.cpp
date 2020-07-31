#include "BranchConfig.hpp"

namespace AnalysisTree {

BranchConfig::BranchConfig(std::string name, DetType type) : name_(std::move(name)), type_(type) {

  if (type_ == DetType::kTrack) {
    VectorConfig<float>::AddField("px", TrackFields::kPx);
    VectorConfig<float>::AddField("py", TrackFields::kPy);
    VectorConfig<float>::AddField("pz", TrackFields::kPz);
    VectorConfig<float>::AddField("pT", TrackFields::kPt);
    VectorConfig<float>::AddField("phi", TrackFields::kPhi);
    VectorConfig<float>::AddField("eta", TrackFields::kEta);
    VectorConfig<float>::AddField("p", TrackFields::kP);
  }
  if (type_ == DetType::kParticle) {
    VectorConfig<float>::AddField("px", ParticleFields::kPx);
    VectorConfig<float>::AddField("py", ParticleFields::kPy);
    VectorConfig<float>::AddField("pz", ParticleFields::kPz);
    VectorConfig<float>::AddField("pT", ParticleFields::kPt);
    VectorConfig<float>::AddField("phi", ParticleFields::kPhi);
    VectorConfig<float>::AddField("eta", ParticleFields::kEta);
    VectorConfig<float>::AddField("mass", ParticleFields::kMass);
    VectorConfig<float>::AddField("p", ParticleFields::kP);
    VectorConfig<float>::AddField("rapidity", ParticleFields::kRapidity);
    VectorConfig<int>::AddField("pid", ParticleFields::kPid);
  }
  if (type_ == DetType::kHit) {
    VectorConfig<float>::AddField("x", HitFields::kX);
    VectorConfig<float>::AddField("y", HitFields::kY);
    VectorConfig<float>::AddField("z", HitFields::kZ);
    VectorConfig<float>::AddField("phi", HitFields::kPhi);
    VectorConfig<float>::AddField("signal", HitFields::kSignal);
  }
  if (type_ == DetType::kModule) {
    VectorConfig<int>::AddField("id", ModuleFields::kId);
    VectorConfig<float>::AddField("signal", ModuleFields::kSignal);
  }
  if (type_ == DetType::kEventHeader) {
    VectorConfig<float>::AddField("vtx_x", EventHeaderFields::kVertexX);
    VectorConfig<float>::AddField("vtx_y", EventHeaderFields::kVertexY);
    VectorConfig<float>::AddField("vtx_z", EventHeaderFields::kVertexZ);
  }
}

Types BranchConfig::GetFieldType(const std::string& sField) const {
  ShortInt_t id = VectorConfig<int>::GetId(sField);
  if (id != UndefValueShort) return Types::kInteger;

  id = VectorConfig<float>::GetId(sField);
  if (id != UndefValueShort) return Types::kFloat;

  id = VectorConfig<bool>::GetId(sField);
  if (id != UndefValueShort) return Types::kBool;

  return (Types) UndefValueShort;
}

ShortInt_t BranchConfig::GetFieldId(const std::string& sField) const {
  ShortInt_t id = VectorConfig<int>::GetId(sField);
  if (id != UndefValueShort) return id;

  id = VectorConfig<float>::GetId(sField);
  if (id != UndefValueShort) return id;

  id = VectorConfig<bool>::GetId(sField);
  if (id != UndefValueShort) return id;

  return UndefValueShort;
}

void BranchConfig::Print() const {
  std::cout << "Branch " << name_ << " (id=" << id_ << ") consists of:" << std::endl;

  std::cout << "  floating fields:" << std::endl;
  for (const auto &entry : VectorConfig<float>::GetMap())
    std::cout << "   " << entry.first << " (id=" << entry.second << ")" << std::endl;

  std::cout << "  integer fields:" << std::endl;
  for (const auto &entry : VectorConfig<int>::GetMap())
    std::cout << "   " << entry.first << " (id=" << entry.second << ")" << std::endl;

  std::cout << "  boolean fields:" << std::endl;
  for (const auto &entry : VectorConfig<bool>::GetMap())
    std::cout << "   " << entry.first << " (id=" << entry.second << ")" << std::endl;
}
}