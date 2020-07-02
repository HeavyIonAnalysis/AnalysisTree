#ifndef ANALYSISTREE_SRC_VARIABLE_H_
#define ANALYSISTREE_SRC_VARIABLE_H_

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <set>
#include <string>
#include <utility>

#include "Configuration.hpp"
#include "Constants.hpp"
#include "Field.hpp"

namespace AnalysisTree {

class Variable {
 public:
  Variable() = default;
  Variable(const Variable &) = default;
  Variable(Variable &&) = default;
  Variable &operator=(Variable &&) = default;
  Variable &operator=(const Variable &) = default;
  ~Variable() = default;

  Variable(std::string name) : name_(std::move(name)) {
    fields_.emplace_back(Field(name_));
  };

  Variable(std::string name, std::string branch, std::string field, short size = 1) : name_(std::move(name)),
                                                                                      branch_names_({branch}),
                                                                                      size_(size) {
    fields_.emplace_back(Field(std::move(branch), std::move(field)));
  };

  Variable(std::string branch, std::string field) : name_(branch + "_" + field), branch_names_({branch}) {
    fields_.emplace_back(Field(std::move(branch), std::move(field)));
  };

  /**
* Generic constructor for complicated variables
* @param fields vector of Fields
* @param lambda function to calculate variable
*/
  Variable(std::string name, std::vector<Field> fields, std::function<double(std::vector<double> &)> lambda) : name_(std::move(name)),
                                                                                                               fields_(std::move(fields)),
                                                                                                               lambda_(std::move(lambda)) {
    for (const auto &f : fields_) {
      branch_names_.emplace_back(f.GetBranchName());
    }
    auto ip = std::unique(branch_names_.begin(), branch_names_.end());
    branch_names_.resize(std::distance(branch_names_.begin(), ip));
  };

  /**
* To be removed soon. Only for backward compatibility
*/
  Variable(std::string name, const std::string &branch, const std::vector<std::string> &fields,
           std::function<double(std::vector<double> &)> lambda) : name_(std::move(name)),
                                                                  branch_names_({branch}),
                                                                  lambda_(std::move(lambda)) {
    for (const auto &field : fields) {
      fields_.emplace_back(Field(branch, field));
    }
  };

  friend bool operator==(const AnalysisTree::Variable &that, const AnalysisTree::Variable &other);
  friend bool operator>(const Variable &that, const Variable &other);
  friend bool operator<(const Variable &that, const Variable &other);

  void Init(const Configuration &conf) {
    for (auto &field : fields_) {
      field.Init(conf);
    }
    is_init_ = true;
  }

  const std::string &GetName() const { return name_; }

  const std::string &GetBranchName(int id = 0) const { return fields_.at(id).GetBranchName(); }
  short GetBranchId(int id = 0) const { return fields_.at(id).GetBranchId(); }
  DetType GetBranchType(int id = 0) const { return fields_.at(id).GetBranchType(); }

  const std::string &GetFieldName(int id = 0) const { return fields_.at(id).GetName(); }
  short GetFieldId(int id = 0) const { return fields_.at(id).GetFieldId(); }
  Types GetFieldType(int id = 0) const { return fields_.at(id).GetFieldType(); }

  short GetId() const { return id_; }
  short GetSize() const { return size_; }
  void SetSize(size_t size) { size_ = size; }

  size_t GetNumberOfFields() const { return fields_.size(); }

  void SetId(short id) { id_ = id; }
  void SetName(std::string name) { name_ = std::move(name); }

  double GetValue(std::vector<double> &vars) const {
    return lambda_(vars);
  }

  size_t GetNumberOfBranches() const { return branch_names_.size(); }
  std::vector<std::string> GetBranches() const { return branch_names_; }

  template<class T>
  double GetValue(const T &object) const {
    assert(branch_names_.size() == 1);
    std::vector<double> vars{};
    vars.reserve(fields_.size());
    for (const auto &field : fields_) {
      vars.emplace_back(field.GetValue(object));
    }
    return lambda_(vars);
  }

  template<class A, class B>
  double GetValue(const A &a, const B &b) {
    assert(branch_names_.size() == 2);
    std::vector<double> vars{};
    vars.reserve(fields_.size());
    for (const auto &field : fields_) {
      if (field.GetBranchId() == a.GetId())
        vars.emplace_back(field.GetValue(a));
      else if (field.GetBranchId() == b.GetId())
        vars.emplace_back(field.GetValue(b));
      else {
        throw std::runtime_error("Variable::Fill - Cannot fill value from branch " + field.GetBranchName());
      }
    }
    return lambda_(vars);
  }

  void Print() const;

 private:
  std::string name_{""};
  std::vector<Field> fields_{};
  std::vector<std::string> branch_names_{};
  std::function<double(std::vector<double> &)> lambda_{[](std::vector<double> &var) { return var.at(0); }};//!

  short size_{1};
  short id_{-999};

  bool is_init_{false};
};

}// namespace AnalysisTree

#endif
