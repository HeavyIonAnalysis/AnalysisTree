/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_SRC_VARIABLE_H_
#define ANALYSISTREE_SRC_VARIABLE_H_

#include <cassert>
#include <functional>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>

#include "BranchChannel.hpp"
#include "Field.hpp"

namespace AnalysisTree {

/**
 * @brief Variable is a wrapper object for Field. In contrary to Field Variable is not bound to the data.
 * It could represent a Field itself or, if specified, a result of operation on two or more fields
 */
class Variable {
 public:
  Variable() = default;
  Variable(const Variable&) = default;
  Variable(Variable&&) = default;
  Variable& operator=(Variable&&) = default;
  Variable& operator=(const Variable&) = default;
  virtual ~Variable() = default;

  explicit Variable(const Field& field)
      : name_(field.GetBranchName() + "." + field.GetName()),
        fields_({field}),
        n_branches_(1){};

  /**
   * Generic constructor for complicated variables
   * @param fields vector of Fields
   * @param lambda function to calculate variable
   * TODO maybe move to Variable::FromFunction()
   */
  Variable(std::string name, std::vector<Field> fields, std::function<double(std::vector<double>&)> lambda);

  Variable(const std::string& branch, const std::string& field) : Variable(Field(branch, field)) {}

  /**
   * @brief
   * @param full_name
   * @return
   */
  static Variable FromString(const std::string& full_name);

  friend bool operator==(const Variable& that, const Variable& other);
  friend bool operator>(const Variable& that, const Variable& other);
  friend bool operator<(const Variable& that, const Variable& other);

  void Init(const Configuration& conf);

  ANALYSISTREE_ATTR_NODISCARD const std::string& GetName() const { return name_; }
  ANALYSISTREE_ATTR_NODISCARD const std::vector<Field>& GetFields() const { return fields_; }
  ANALYSISTREE_ATTR_NODISCARD short GetNumberOfBranches() const { return n_branches_; }
  ANALYSISTREE_ATTR_NODISCARD std::set<std::string> GetBranches() const;
  ANALYSISTREE_ATTR_NODISCARD std::string GetBranchName() const;

  double GetValue(std::vector<const BranchChannel*>& bch, std::vector<size_t>& id) const;
  [[deprecated]] double GetValue(const BranchChannel& a, size_t a_id, const BranchChannel& b, size_t b_id) const;

  template<class T>
  double GetValue(const T& object) const {
    assert(is_init_ && n_branches_ == 1);
    vars_.clear();
    for (const auto& field : fields_) {
      vars_.emplace_back(field.template GetValue(object));
    }
    return lambda_(vars_);
  }

  double GetValue(const BranchChannel& object) const;

  void SetName(std::string name) { name_ = std::move(name); }

  void Print() const;

 protected:
  std::string name_;
  std::vector<Field> fields_{};
  mutable std::vector<double> vars_{};                                                                    //!
  std::function<double(std::vector<double>&)> lambda_{[](std::vector<double>& var) { return var.at(0); }};//!
  short n_branches_{0};
  bool is_init_{false};
  ClassDef(Variable, 0);
};

}// namespace AnalysisTree

#endif
