/* Copyright (C) 2019-2021 GSI, Universität Tübingen, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Eugeny Kashirin, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_SRC_MODULE_H_
#define ANALYSISTREE_SRC_MODULE_H_

#include <stdexcept>

#include <TVector3.h>

#include "Container.hpp"

namespace AnalysisTree {

class Module : public Container {

 public:
  Module() = default;
  explicit Module(const Container& cont) : Container(cont) {}
  Module(const Module& otherModule) = default;
  Module(Module&& otherModule) = default;
  Module& operator=(Module&&) = default;
  Module& operator=(const Module& part) = default;

  explicit Module(size_t id) : Container(id) {}
  Module(size_t id, const BranchConfig& branch) noexcept : Container(id, branch) {}

  ANALYSISTREE_ATTR_NODISCARD Floating_t GetSignal() const { return signal_; }
  ANALYSISTREE_ATTR_NODISCARD ShortInt_t GetNumber() const { return number_; }

  void SetSignal(Floating_t signal) { signal_ = signal; }
  void SetNumber(ShortInt_t number) { number_ = number; }

  friend bool operator==(const Module& that, const Module& other);

  template<typename T>
  T GetField(Integer_t iField) const {
    if (iField >= 0)
      return Container::GetField<T>(iField);
    else {
      switch (iField) {
        case ModuleFields::kNumber: return GetNumber();
        case ModuleFields::kSignal: return GetSignal();
        case ModuleFields::kId: return GetId();
        default: throw std::out_of_range("Module::GetField - Index " + std::to_string(iField) + " is not found");
      }
    }
  }

  template<typename T>
  void SetField(T value, Int_t field_id) {
    if (field_id >= 0) {
      Container::SetField(value, field_id);
    } else {
      switch (field_id) {
        case ModuleFields::kSignal: signal_ = value; break;
        case ModuleFields::kNumber: number_ = value; break;
        case ModuleFields::kId: break;
        default: throw std::runtime_error("Unknown field");
      }
    }
  }

  void Print() const noexcept override;

 protected:
  Floating_t signal_{0.f};
  ShortInt_t number_{UndefValueShort};

  ClassDefOverride(Module, 2)
};

class ModulePosition : public IndexedObject {

 public:
  ModulePosition() noexcept = default;
  ModulePosition(const ModulePosition& otherModulePosition) = default;
  ModulePosition(ModulePosition&& otherModulePosition) noexcept = default;
  ModulePosition& operator=(ModulePosition&&) noexcept = default;
  ModulePosition& operator=(const ModulePosition& part) = default;

  explicit ModulePosition(size_t id) : IndexedObject(id) {}
  ModulePosition(size_t id, Double_t x, Double_t y, Double_t z) : IndexedObject(id), x_(x), y_(y), z_(z) {}

  ANALYSISTREE_ATTR_NODISCARD TVector3 GetPosition() const noexcept {
    return TVector3(x_, y_, z_);
  }

  void SetPosition(const TVector3& position) noexcept {
    x_ = position.X();
    y_ = position.Y();
    z_ = position.Z();
  }

  void SetPosition(Double_t x, Double_t y, Double_t z) noexcept {
    x_ = x;
    y_ = y;
    z_ = z;
  }

  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetX() const noexcept { return x_; }
  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetY() const noexcept { return y_; }
  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetZ() const noexcept { return z_; }
  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetPhi() const noexcept { return atan2(y_, x_); }

  void Print() const;

 protected:
  Floating_t x_{UndefValueFloat};
  Floating_t y_{UndefValueFloat};
  Floating_t z_{UndefValueFloat};
};
}// namespace AnalysisTree

#endif