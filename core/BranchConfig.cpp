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

BranchConfig::BranchConfig(std::string name, DetType type) : name_(std::move(name)), type_(type) {
  GenerateId();

  if (type_ == DetType::kTrack) {
    VectorConfig<float>::AddField("px", TrackFields::kPx, "GeV/c");
    VectorConfig<float>::AddField("py", TrackFields::kPy, "GeV/c");
    VectorConfig<float>::AddField("pz", TrackFields::kPz, "GeV/c");
    VectorConfig<float>::AddField("pT", TrackFields::kPt, "GeV/c");
    VectorConfig<float>::AddField("phi", TrackFields::kPhi, "azimuthal angle");
    VectorConfig<float>::AddField("eta", TrackFields::kEta, "pseudorapidity");
    VectorConfig<float>::AddField("p", TrackFields::kP, "GeV/c");
    VectorConfig<int>::AddField("id", TrackFields::kId, "unique id");
  } else if (type_ == DetType::kParticle) {
    VectorConfig<float>::AddField("px", ParticleFields::kPx, "GeV/c");
    VectorConfig<float>::AddField("py", ParticleFields::kPy, "GeV/c");
    VectorConfig<float>::AddField("pz", ParticleFields::kPz, "GeV/c");
    VectorConfig<float>::AddField("pT", ParticleFields::kPt, "GeV/c");
    VectorConfig<float>::AddField("phi", ParticleFields::kPhi, "azimuthal angle");
    VectorConfig<float>::AddField("eta", ParticleFields::kEta, "pseudorapidity");
    VectorConfig<float>::AddField("mass", ParticleFields::kMass, "GeV/c^2");
    VectorConfig<float>::AddField("p", ParticleFields::kP, "GeV/c");
    VectorConfig<float>::AddField("E", ParticleFields::kEnergy, "full energy, GeV");
    VectorConfig<float>::AddField("T", ParticleFields::kKineticEnergy, "kinetic energy, GeV");
    VectorConfig<float>::AddField("rapidity", ParticleFields::kRapidity, "in Lab. frame");
    VectorConfig<int>::AddField("pid", ParticleFields::kPid, "PDG code");
    VectorConfig<int>::AddField("id", ParticleFields::kId, "unique id");
  } else if (type_ == DetType::kHit) {
    VectorConfig<float>::AddField("x", HitFields::kX, "cm");
    VectorConfig<float>::AddField("y", HitFields::kY, "cm");
    VectorConfig<float>::AddField("z", HitFields::kZ, "cm");
    VectorConfig<float>::AddField("phi", HitFields::kPhi, "azimuthal angle");
    VectorConfig<float>::AddField("signal", HitFields::kSignal, "");
    VectorConfig<int>::AddField("id", HitFields::kId, "unique id");
  } else if (type_ == DetType::kModule) {
    VectorConfig<int>::AddField("number", ModuleFields::kNumber, "module number");
    VectorConfig<float>::AddField("signal", ModuleFields::kSignal, "");
    VectorConfig<int>::AddField("id", ModuleFields::kId, "unique id");
  } else if (type_ == DetType::kEventHeader) {
    VectorConfig<float>::AddField("vtx_x", EventHeaderFields::kVertexX, "cm");
    VectorConfig<float>::AddField("vtx_y", EventHeaderFields::kVertexY, "cm");
    VectorConfig<float>::AddField("vtx_z", EventHeaderFields::kVertexZ, "cm");
    VectorConfig<int>::AddField("id", EventHeaderFields::kId, "unique id");
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

  result.AddField<int>("matching_case", "0 - both present, 1 - only first present, 2 - only second present");

  return result;
}

template<typename T>
std::vector<std::string> VectorConfig<T>::SplitString(const std::string& input) {
  std::vector<std::string> result;
  std::vector<int> newlinepositions{-1};
  int it{0};
  while (it < std::string::npos) {
    it = input.find("\n", it+1);
    newlinepositions.emplace_back(it);
  }
  newlinepositions.back() = input.size();
  for(int ip=0; ip<newlinepositions.size()-1; ++ip) {
    result.emplace_back(input.substr(newlinepositions.at(ip)+1, newlinepositions.at(ip+1) - newlinepositions.at(ip) - 1));
  }

  return result;
}

void BranchConfig::Print() const {
  std::cout << "Branch " << name_ << " (id=" << id_ << ") consists of:" << std::endl;
  std::cout << "Floating fields:" << std::endl;
  VectorConfig<float>::Print();
  std::cout << "Integer fields:" << std::endl;
  VectorConfig<int>::Print();
  std::cout << "Boolean fields:" << std::endl;
  VectorConfig<bool>::Print();
  //  std::cout << std::endl;
}
}// namespace AnalysisTree
