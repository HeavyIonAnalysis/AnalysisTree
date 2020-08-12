#ifndef ANALYSISTREE_SRC_PARTICLE_H_
#define ANALYSISTREE_SRC_PARTICLE_H_

#include "Track.hpp"

namespace AnalysisTree {

class Particle : public Track {

 public:
  Particle() = default;
  explicit Particle(Integer_t id) : Track(id) {}
  explicit Particle(const Track &track) : Track(track) {}

  Particle(const Particle &particle) = default;
  Particle(Particle &&particle) = default;
  Particle &operator=(Particle &&) = default;
  Particle &operator=(const Particle &particle) = default;

  [[nodiscard]] Floating_t GetRapidity() const { return Track::GetRapidityByMass(mass_); }
  [[nodiscard]] PdgCode_t GetPid() const { return pid_; }
  [[nodiscard]] Floating_t GetMass() const { return mass_; }

  void SetMass(Floating_t mass) {
    mass_ = mass;
  }

  void SetPid(PdgCode_t pid);

  [[nodiscard]] Floating_t GetEnergy() const { return sqrt(mass_ * mass_ + GetP() * GetP()); }
  [[nodiscard]] Floating_t GetKineticEnergy() const { return GetEnergy() - mass_; }

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
        case ParticleFields::kPx: return GetPx();
        case ParticleFields::kPy: return GetPy();
        case ParticleFields::kPz: return GetPz();
        default: throw std::out_of_range("Particle::GetField - Index " + std::to_string(iField) + " is not found");
      }
    }
  }

 protected:
  Floating_t mass_{-1000.f};
  PdgCode_t pid_{0};
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_SRC_PARTICLE_H_
