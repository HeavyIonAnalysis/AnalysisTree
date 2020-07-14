#ifndef ANALYSISTREE_SRC_HIT_H_
#define ANALYSISTREE_SRC_HIT_H_

#include <TVector3.h>

#include "Constants.hpp"
#include "Container.hpp"

namespace AnalysisTree {

class Hit : public Container {

 public:
  Hit() = default;
  Hit(const Hit &otherHit) = default;
  Hit(Hit &&otherHit) = default;
  Hit &operator=(Hit &&) = default;
  Hit &operator=(const Hit &part) = default;

  explicit Hit(Integer_t id) : Container(id) {}

  TVector3 GetPosition() const {
    return TVector3(x_, y_, z_);
  }

  Floating_t GetSignal() const { return signal_; }

  void SetPosition(const TVector3 &position) {
    x_ = position.X();
    y_ = position.Y();
    z_ = position.Z();
  }

  void SetPosition(Double_t x, Double_t y, Double_t z) {
    x_ = x;
    y_ = y;
    z_ = z;
  }

  void SetSignal(Double_t signal) {
    signal_ = signal;
  }

  Floating_t GetX() const { return x_; }
  Floating_t GetY() const { return y_; }
  Floating_t GetZ() const { return z_; }
  Floating_t GetPhi() const;

  friend bool operator==(const Hit &that, const Hit &other);

  template<typename T>
  T GetField(Integer_t iField) const {
    if (iField >= 0)
      return Container::GetField<T>(iField);
    else {
      switch (iField) {
        case HitFields::kX: return GetX();
        case HitFields::kY: return GetY();
        case HitFields::kZ: return GetZ();
        case HitFields::kPhi: return GetPhi();
        case HitFields::kSignal: return GetSignal();
        default: throw std::out_of_range("Hit::GetField - Index " + std::to_string(iField) + " is not found");
      }
    }
  }

  void Print() const;

 protected:
  Floating_t x_{UndefValueFloat};
  Floating_t y_{UndefValueFloat};
  Floating_t z_{UndefValueFloat};
  Floating_t signal_{UndefValueFloat};

  //  ClassDefOverride(AnalysisTree::Base::Hit, 1)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_SRC_HIT_H_
