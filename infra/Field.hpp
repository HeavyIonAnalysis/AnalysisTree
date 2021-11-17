/* Copyright (C) 2019-2021 GSI, Universität Tübingen, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Eugeny Kashirin, Ilya Selyuzhenkov */

#ifndef ANALYSISTREE_INFRA_FIELD_H_
#define ANALYSISTREE_INFRA_FIELD_H_

#include <stdexcept>
#include <string>

#include "Constants.hpp"
#include "Utils.hpp"

namespace AnalysisTree {

class Configuration;
class Branch;

/**
 * @brief Field is a _pointer_ in a branch/field structure.
 */
class Field {
 public:
  Field() = default;
  Field(const Field&) = default;
  Field(Field&&) = default;
  Field& operator=(Field&&) = default;
  Field& operator=(const Field&) = default;
  virtual ~Field() = default;

  explicit Field(std::string name) : field_(std::move(name)){};

  Field(std::string branch, std::string field) : branch_(std::move(branch)),
                                                 field_(std::move(field)){};

  friend bool operator==(const Field& that, const Field& other);
  friend bool operator>(const Field& that, const Field& other);
  friend bool operator<(const Field& that, const Field& other);

  /**
   * @brief Initializes branchId and fieldId
   * @param conf Configuration
   */
  void Init(const Configuration& conf);
  void Init(const BranchConfig& conf);

  ANALYSISTREE_ATTR_NODISCARD const std::string& GetName() const { return field_; }
  ANALYSISTREE_ATTR_NODISCARD const std::string& GetBranchName() const { return branch_; }

  ANALYSISTREE_ATTR_NODISCARD size_t GetBranchId() const { return branch_id_; }
  ANALYSISTREE_ATTR_NODISCARD short GetFieldId() const { return field_id_; }

  ANALYSISTREE_ATTR_NODISCARD DetType GetBranchType() const { return branch_type_; }
  ANALYSISTREE_ATTR_NODISCARD Types GetFieldType() const { return field_type_; }

  /**
   * @brief Gets numerical value from data-object associated with TTree
   * @tparam T type of data-object (EventHeader, Track, etc)
   * @param object
   * @return value of the field
   */
  template<class T>
  double GetValue(const T& object) const {
    if (!is_init_) {
      throw std::runtime_error("Field::Fill - Field " + field_ + " is not initialized");
    }
    switch (field_type_) {
      case Types::kFloat: return object.template GetField<float>(field_id_);
      case (Types::kInteger): return object.template GetField<int>(field_id_);
      case Types::kBool: return object.template GetField<bool>(field_id_);
      default: throw std::runtime_error("Unknown field type");
    }
  }

  void Print() const;

  ANALYSISTREE_ATTR_NODISCARD const Branch* GetParentBranch() const { return parent_branch_; }
  ANALYSISTREE_ATTR_NODISCARD bool IsInitialized() const { return is_init_; }
  explicit operator bool() const { return IsInitialized(); }

 private:
  friend Branch;

  const Branch* parent_branch_{nullptr};///!

  std::string branch_;
  std::string field_;

  short field_id_{0};
  Types field_type_{Types::kNumberOfTypes};

  size_t branch_id_{0};
  DetType branch_type_{DetType(UndefValueShort)};

  bool is_init_{false};

  ClassDef(Field, 0);
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_FIELD_H_
