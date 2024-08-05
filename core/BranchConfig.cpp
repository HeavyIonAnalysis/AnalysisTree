/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "BranchConfig.hpp"

#include <functional>//for std::hash

ClassImp(AnalysisTree::BranchConfig);

namespace AnalysisTree {

void BranchConfig::GenerateId() {
  std::hash<std::string> id_hasher;
  id_ = id_hasher(name_);
}

BranchConfig::BranchConfig(std::string name, DetType type, std::string title) : name_(std::move(name)), type_(type), title_(std::move(title)) {
  GenerateId();

  if (type_ == DetType::kTrack) {
    VectorConfig<float>::AddField("px", TrackFields::kPx, "X-projection of the momentum of the track, GeV/c");
    VectorConfig<float>::AddField("py", TrackFields::kPy, "Y-projection of the momentum of the track, GeV/c");
    VectorConfig<float>::AddField("pz", TrackFields::kPz, "Z-projection of the momentum of the track, GeV/c");
    VectorConfig<float>::AddField("pT", TrackFields::kPt, "Transverse momentum of the track, GeV/c");
    VectorConfig<float>::AddField("phi", TrackFields::kPhi, "Azimuthal angle of the track, rad");
    VectorConfig<float>::AddField("eta", TrackFields::kEta, "Pseudorapidity of the track");
    VectorConfig<float>::AddField("p", TrackFields::kP, "Full momentum of the track, GeV/c");
    VectorConfig<int>::AddField("q", TrackFields::kQ, "Charge of the track (or its sign when absolute value unknown)");
    VectorConfig<int>::AddField("id", TrackFields::kId, "Unique id of the track within current event; assigned automatically (not by user)");
  } else if (type_ == DetType::kParticle) {
    VectorConfig<float>::AddField("px", ParticleFields::kPx, "X-projection of the momentum of the particle, GeV/c");
    VectorConfig<float>::AddField("py", ParticleFields::kPy, "Y-projection of the momentum of the particle, GeV/c");
    VectorConfig<float>::AddField("pz", ParticleFields::kPz, "Z-projection of the momentum of the particle, GeV/c");
    VectorConfig<float>::AddField("pT", ParticleFields::kPt, "Transverse momentum of the particle, GeV/c");
    VectorConfig<float>::AddField("phi", ParticleFields::kPhi, "Azimuthal angle of the particle, rad");
    VectorConfig<float>::AddField("eta", ParticleFields::kEta, "Pseudorapidity of the particle");
    VectorConfig<float>::AddField("mass", ParticleFields::kMass, "Mass of the particle, GeV/c^2 (true mass of certain particle species; deprecated to use for invariant mass, mass from TOF etc.)");
    VectorConfig<float>::AddField("p", ParticleFields::kP, "Full momentum of the particle, GeV/c");
    VectorConfig<float>::AddField("E", ParticleFields::kEnergy, "Full energy of the particle, GeV");
    VectorConfig<float>::AddField("T", ParticleFields::kKineticEnergy, "Kinetic energy of the particle, GeV");
    VectorConfig<float>::AddField("rapidity", ParticleFields::kRapidity, "Rapidity of the particle");
    VectorConfig<int>::AddField("q", ParticleFields::kQ, "Charge of the particle (true charge of certain particle species)");
    VectorConfig<int>::AddField("pid", ParticleFields::kPid, "PDG code of the particle (e.g. by simulation for MC-particles or the most probable one for reconstructed particles with performed PID etc.)");
    VectorConfig<int>::AddField("id", ParticleFields::kId, "Unique id of the particle within current event; assigned automatically (not by user)");
  } else if (type_ == DetType::kHit) {
    VectorConfig<float>::AddField("x", HitFields::kX, "X coordinate of the hit, cm");
    VectorConfig<float>::AddField("y", HitFields::kY, "Y coordinate of the hit, cm");
    VectorConfig<float>::AddField("z", HitFields::kZ, "Z coordinate of the hit, cm");
    VectorConfig<float>::AddField("phi", HitFields::kPhi, "Azimuthal angle of the hit, rad");
    VectorConfig<float>::AddField("signal", HitFields::kSignal, "Energy deposit collected in the hit");
    VectorConfig<int>::AddField("id", HitFields::kId, "Unique id of the hit within current event; assigned automatically (not by user)");
  } else if (type_ == DetType::kModule) {
    VectorConfig<int>::AddField("number", ModuleFields::kNumber, "Module number");
    VectorConfig<float>::AddField("signal", ModuleFields::kSignal, "Energy deposit collected in the module");
    VectorConfig<int>::AddField("id", ModuleFields::kId, "Unique id of the hit within current event; assigned automatically (not by user)");
  } else if (type_ == DetType::kEventHeader) {
    VectorConfig<float>::AddField("vtx_x", EventHeaderFields::kVertexX, "X coordinate of the vertex, cm");
    VectorConfig<float>::AddField("vtx_y", EventHeaderFields::kVertexY, "Y coordinate of the vertex, cm");
    VectorConfig<float>::AddField("vtx_z", EventHeaderFields::kVertexZ, "Z coordinate of the vertex, cm");
    VectorConfig<int>::AddField("id", EventHeaderFields::kId, "Always 0, this field is not used for EventHeader and is needed for compatibility with other Container types");
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

BranchConfig BranchConfig::Clone(const std::string& name, DetType type) const {
  BranchConfig result(name, type);
  for (const auto& field : AnalysisTree::VectorConfig<float>::map_) {
    if (field.second.id_ >= 0) {
      result.AddField<float>(field.first, field.second.id_, field.second.title_);
    }
  }
  for (const auto& field : AnalysisTree::VectorConfig<int>::map_) {
    if (field.second.id_ >= 0) {
      result.AddField<int>(field.first, field.second.id_, field.second.title_);
    }
  }
  for (const auto& field : AnalysisTree::VectorConfig<bool>::map_) {
    if (field.second.id_ >= 0) {
      result.AddField<bool>(field.first, field.second.id_, field.second.title_);
    }
  }
  result.AnalysisTree::VectorConfig<bool>::size_ = AnalysisTree::VectorConfig<bool>::size_;
  result.AnalysisTree::VectorConfig<int>::size_ = AnalysisTree::VectorConfig<int>::size_;
  result.AnalysisTree::VectorConfig<float>::size_ = AnalysisTree::VectorConfig<float>::size_;

  return result;
}

BranchConfig BranchConfig::CloneAndMerge(const BranchConfig& attached) const {
  const std::string name1 = GetName();
  const std::string name2 = attached.GetName();
  const DetType type1 = GetType();

  auto result = Clone(name1 + "_" + name2, type1);

  for (const auto& field : attached.AnalysisTree::VectorConfig<float>::map_) {
    result.AddField<float>(name2 + "_" + field.first, name2 + ": " + field.second.title_);
  }
  for (const auto& field : attached.AnalysisTree::VectorConfig<int>::map_) {
    result.AddField<int>(name2 + "_" + field.first, name2 + ": " + field.second.title_);
  }
  for (const auto& field : attached.AnalysisTree::VectorConfig<bool>::map_) {
    result.AddField<bool>(name2 + "_" + field.first, name2 + ": " + field.second.title_);
  }

  if (type1 != DetType::kEventHeader && attached.GetType() != DetType::kEventHeader) {
    result.AddField<int>("matching_case", "0 - both present, 1 - only first present, 2 - only second present");
  }

  return result;
}

template<typename T>
std::vector<std::string> VectorConfig<T>::SplitString(const std::string& input) {
  std::vector<std::string> result;
  std::vector<int> newlinepositions{-1};
  int it{0};
  while (it < std::string::npos) {
    it = input.find("\n", it + 1);
    newlinepositions.emplace_back(it);
  }
  newlinepositions.back() = input.size();
  for (int ip = 0; ip < newlinepositions.size() - 1; ++ip) {
    result.emplace_back(input.substr(newlinepositions.at(ip) + 1, newlinepositions.at(ip + 1) - newlinepositions.at(ip) - 1));
  }

  return result;
}

template<typename T>
void VectorConfig<T>::RemoveField(const std::string& name, int id) {
  auto iter = map_.find(name);
  map_.erase(iter);
  for (auto& m : map_) {
    if (m.second.id_ > id) {
      m.second.id_--;
    }
  }
}

void BranchConfig::RemoveField(const std::string& name) {
  if (!HasField(name)) {
    throw std::runtime_error("BranchConfig::RemoveField(): no field " + name + " to be removed");
  }
  auto field_type = GetFieldType(name);
  auto field_id = GetFieldId(name);
  if (field_id < 0) {
    throw std::runtime_error("BranchConfig::RemoveField(): default field " + name + " cannot be removed");
  }
  if (field_type == Types::kInteger) VectorConfig<int>::RemoveField(name, field_id);
  if (field_type == Types::kFloat) VectorConfig<float>::RemoveField(name, field_id);
  if (field_type == Types::kBool) VectorConfig<bool>::RemoveField(name, field_id);
}

void BranchConfig::GuaranteeFieldNameVacancy(const std::string& name) const {
  if (HasField(name)) {
    throw std::runtime_error("BranchConfig::GuaranteeFieldNameVacancy(): field " + name + " already exists");
  }
}

void BranchConfig::Print() const {
  std::cout << "Branch " << name_ << " (" << title_ << ") consists of:" << std::endl;
  std::cout << "\nFloating fields:" << std::endl;
  VectorConfig<float>::Print();
  std::cout << "\nInteger fields:" << std::endl;
  VectorConfig<int>::Print();
  std::cout << "\nBoolean fields:" << std::endl;
  VectorConfig<bool>::Print();
  //  std::cout << std::endl;
}

void BranchConfig::PrintBranchId() const {
  std::cout << "Branch " << name_ << " (id=" << id_ << ")" << std::endl;
}

}// namespace AnalysisTree
