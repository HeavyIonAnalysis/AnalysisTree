/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_SRC_PARTICLE_H_
#define ANALYSISTREE_SRC_PARTICLE_H_

#include "Track.hpp"

namespace AnalysisTree {

class Particle : public Track {

 public:
  Particle() = default;
  explicit Particle(size_t id) : Track(id) {}
  Particle(size_t id, const BranchConfig& branch) noexcept : Track(id, branch) {}

  explicit Particle(const Container& cont) : Track(cont) {}
  explicit Particle(const Track& track) : Track(track) {}

  Particle(const Particle& particle) = default;
  Particle(Particle&& particle) = default;
  Particle& operator=(Particle&&) = default;
  Particle& operator=(const Particle& particle) = default;
  ~Particle() override = default;

  ANALYSISTREE_ATTR_NODISCARD Floating_t GetRapidity() const { return Track::GetRapidityByMass(mass_); }
  ANALYSISTREE_ATTR_NODISCARD PdgCode_t GetPid() const { return pid_; }
  ANALYSISTREE_ATTR_NODISCARD Floating_t GetMass() const { return mass_; }

  void SetMass(Floating_t mass) {
    mass_ = mass;
  }

  void SetPid(PdgCode_t pid);

  ANALYSISTREE_ATTR_NODISCARD Floating_t GetEnergy() const { return sqrt(mass_ * mass_ + GetP() * GetP()); }
  ANALYSISTREE_ATTR_NODISCARD Floating_t GetKineticEnergy() const { return GetEnergy() - mass_; }

  template<typename T>
  T GetField(Integer_t iField) const {
    if (iField >= 0)
      return Container::GetField<T>(iField);
    else {//TODO fix for T=int
      switch (iField) {
        case ParticleFields::kPhi: return GetPhi();
        case ParticleFields::kPt: return GetPt();
        case ParticleFields::kRapidity: return GetRapidity();
        case ParticleFields::kPid: return GetPid();
        case ParticleFields::kMass: return GetMass();
        case ParticleFields::kEta: return GetEta();
        case ParticleFields::kP: return GetP();
        case ParticleFields::kEnergy: return GetEnergy();
        case ParticleFields::kKineticEnergy: return GetKineticEnergy();
        case ParticleFields::kPx: return GetPx();
        case ParticleFields::kPy: return GetPy();
        case ParticleFields::kPz: return GetPz();
        case ParticleFields::kId: return GetId();
        default: throw std::out_of_range("Particle::GetField - Index " + std::to_string(iField) + " is not found");
      }
    }
  }

  template<typename T>
  void SetField(T value, Int_t field_id) {
    if (field_id >= 0) {
      Container::SetField(value, field_id);
    } else {
      switch (field_id) {
        case ParticleFields::kPx: px_ = value; break;
        case ParticleFields::kPy: py_ = value; break;
        case ParticleFields::kPz: pz_ = value; break;
        case ParticleFields::kMass: mass_ = value; break;
        case ParticleFields::kPid: SetPid(value); break;
        case ParticleFields::kId: break;
        case ParticleFields::kP: /*throw std::runtime_error("Cannot set transient fields");*/ break;
        case ParticleFields::kEnergy: /*throw std::runtime_error("Cannot set transient fields");*/ break;
        case ParticleFields::kKineticEnergy: /*throw std::runtime_error("Cannot set transient fields");*/ break;
        case ParticleFields::kPt: /*throw std::runtime_error("Cannot set transient fields");*/ break;
        case ParticleFields::kEta: /*throw std::runtime_error("Cannot set transient fields");*/ break;
        case ParticleFields::kPhi: /*throw std::runtime_error("Cannot set transient fields");*/ break;
        case ParticleFields::kRapidity: /*throw std::runtime_error("Cannot set transient fields");*/ break;
        default: throw std::runtime_error("Particle::SetField - Index " + std::to_string(field_id) + " is not found");
      }
    }
  }

 protected:
  Floating_t mass_{-1000.f};
  PdgCode_t pid_{0};

  ClassDefOverride(Particle, 2);
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_SRC_PARTICLE_H_
