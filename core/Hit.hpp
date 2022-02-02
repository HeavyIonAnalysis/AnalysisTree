/* Copyright (C) 2019-2021 GSI, Universität Tübingen, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Eugeny Kashirin, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_SRC_HIT_H_
#define ANALYSISTREE_SRC_HIT_H_

#include <stdexcept>

#include <TVector3.h>

#include "Constants.hpp"
#include "Container.hpp"

namespace AnalysisTree {

class Hit : public Container {

 public:
  Hit() = default;
  explicit Hit(const Container& cont) : Container(cont) {}
  Hit(const Hit& otherHit) = default;
  Hit(Hit&& otherHit) = default;
  Hit& operator=(Hit&&) = default;
  Hit& operator=(const Hit& part) = default;

  explicit Hit(size_t id) : Container(id) {}
  Hit(size_t id, const BranchConfig& branch) noexcept : Container(id, branch) {}

  friend bool operator==(const Hit& that, const Hit& other);

  void SetPosition(const TVector3& position) {
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

  ANALYSISTREE_ATTR_NODISCARD TVector3 GetPosition() const { return TVector3(x_, y_, z_); }
  ANALYSISTREE_ATTR_NODISCARD Floating_t GetSignal() const { return signal_; }
  ANALYSISTREE_ATTR_NODISCARD Floating_t GetX() const { return x_; }
  ANALYSISTREE_ATTR_NODISCARD Floating_t GetY() const { return y_; }
  ANALYSISTREE_ATTR_NODISCARD Floating_t GetZ() const { return z_; }
  ANALYSISTREE_ATTR_NODISCARD Floating_t GetPhi() const;

  template<typename T>
  ANALYSISTREE_ATTR_NODISCARD T GetField(Integer_t iField) const {
    if (iField >= 0)
      return Container::GetField<T>(iField);
    else {
      switch (iField) {
        case HitFields::kX: return GetX();
        case HitFields::kY: return GetY();
        case HitFields::kZ: return GetZ();
        case HitFields::kPhi: return GetPhi();
        case HitFields::kSignal: return GetSignal();
        case HitFields::kId: return GetId();
        default: throw std::out_of_range("Hit::GetField - Index " + std::to_string(iField) + " is not found");
      }
    }
  }

  template<typename T>
  void SetField(T value, Int_t field_id) {
    if (field_id >= 0) {
      Container::SetField(value, field_id);
    } else {
      switch (field_id) {
        case HitFields::kX: x_ = value; break;
        case HitFields::kY: y_ = value; break;
        case HitFields::kZ: z_ = value; break;
        case HitFields::kSignal: signal_ = value; break;
        case HitFields::kId: break;
        case HitFields::kPhi: /*throw std::runtime_error("Cannot set transient fields");*/ break;
        default: throw std::runtime_error("Unknown field");
      }
    }
  }

  void Print() const noexcept override;

 protected:
  Floating_t x_{UndefValueFloat};
  Floating_t y_{UndefValueFloat};
  Floating_t z_{UndefValueFloat};
  Floating_t signal_{UndefValueFloat};

  ClassDefOverride(Hit, 2)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_SRC_HIT_H_
