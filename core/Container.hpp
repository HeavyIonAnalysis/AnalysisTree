/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_BASECONTAINER_H
#define ANALYSISTREE_BASECONTAINER_H

#include "BranchConfig.hpp"
#include "IndexedObject.hpp"

namespace AnalysisTree {

/// A class to store any number of integers, floats and bools.
/**
 * Consists of IndexedObject and separate std::vector<T>, for T={float, int, bool}.
 * Intended to be used as a base class for all AnalysysTree objects.
 */

class Container : public IndexedObject {
 public:
  Container() = default;

  explicit Container(size_t id) : IndexedObject(id) {}
  Container(size_t id, const BranchConfig& branch)
      : IndexedObject(id),
        floats_(branch.GetSize<float>()),
        ints_(branch.GetSize<int>()),
        bools_(branch.GetSize<bool>()) {}

  Container(const Container& container) = default;
  Container(Container&& container) = default;
  Container& operator=(Container&&) = default;
  Container& operator=(const Container& part) = default;
  ~Container() override = default;

  template<class T>
  std::vector<T>& Vector();

  template<class T>
  const std::vector<T>& GetVector() const;

  template<typename T>
  void SetField(T value, Integer_t field_id) {
    Vector<T>().at(field_id) = value;
  }

  template<typename T>
  ANALYSISTREE_ATTR_NODISCARD T GetField(Integer_t field_id) const {
    return GetVector<T>().at(field_id);
  }

  template<typename T>
  ANALYSISTREE_ATTR_NODISCARD size_t GetSize() const {
    return GetVector<T>().size();
  }

  void Init(const BranchConfig& branch);
  virtual void Print() const noexcept;

 protected:
  std::vector<float> floats_{};
  std::vector<int> ints_{};
  std::vector<bool> bools_{};

  ClassDefOverride(Container, 2);
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_BASECONTAINER_H
