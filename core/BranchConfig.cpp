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
  } else if (type_ == DetType::kParticle) {
    VectorConfig<float>::AddField("px", ParticleFields::kPx, "GeV/c");
    VectorConfig<float>::AddField("py", ParticleFields::kPy, "GeV/c");
    VectorConfig<float>::AddField("pz", ParticleFields::kPz, "GeV/c");
    VectorConfig<float>::AddField("pT", ParticleFields::kPt, "GeV/c");
    VectorConfig<float>::AddField("phi", ParticleFields::kPhi, "azimuthal angle");
    VectorConfig<float>::AddField("eta", ParticleFields::kEta, "pseudorapidity");
    VectorConfig<float>::AddField("mass", ParticleFields::kMass, "GeV/c^2");
    VectorConfig<float>::AddField("p", ParticleFields::kP, "GeV/c");
    VectorConfig<float>::AddField("rapidity", ParticleFields::kRapidity, "in Lab. frame");
    VectorConfig<int>::AddField("pid", ParticleFields::kPid, "PDG code");
  } else if (type_ == DetType::kHit) {
    VectorConfig<float>::AddField("x", HitFields::kX, "cm");
    VectorConfig<float>::AddField("y", HitFields::kY, "cm");
    VectorConfig<float>::AddField("z", HitFields::kZ, "cm");
    VectorConfig<float>::AddField("phi", HitFields::kPhi, "azimuthal angle");
    VectorConfig<float>::AddField("signal", HitFields::kSignal);
  } else if (type_ == DetType::kModule) {
    VectorConfig<int>::AddField("number", ModuleFields::kNumber);
    VectorConfig<float>::AddField("signal", ModuleFields::kSignal);
  } else if (type_ == DetType::kEventHeader) {
    VectorConfig<float>::AddField("vtx_x", EventHeaderFields::kVertexX, "cm");
    VectorConfig<float>::AddField("vtx_y", EventHeaderFields::kVertexY, "cm");
    VectorConfig<float>::AddField("vtx_z", EventHeaderFields::kVertexZ, "cm");
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
  std::cout << "Floating fields:" << std::endl;
  VectorConfig<float>::Print();
  std::cout << "Integer fields:" << std::endl;
  VectorConfig<int>::Print();
  std::cout << "Boolean fields:" << std::endl;
  VectorConfig<bool>::Print();
  //  std::cout << std::endl;
}
}// namespace AnalysisTree