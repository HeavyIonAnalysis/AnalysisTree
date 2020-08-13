#ifndef ANALYSISTREE_SRC_VARIABLE_H_
#define ANALYSISTREE_SRC_VARIABLE_H_

#include <cassert>
#include <functional>
#include <set>
#include <string>
#include <utility>

#include "Field.hpp"

namespace AnalysisTree {

class Variable {
 public:
  Variable() = default;
  Variable(const Variable&) = default;
  Variable(Variable&&) = default;
  Variable& operator=(Variable&&) = default;
  Variable& operator=(const Variable&) = default;
  ~Variable() = default;

  Variable(std::string name) : name_(std::move(name)) {
    fields_.emplace_back(Field(name_));
  };

  explicit Variable(const Field& field, short size = 1) : name_(field.GetBranchName() + "_" + field.GetName()),
                                                          fields_({field}),
                                                          branch_names_({field.GetBranchName()}),
                                                          size_(size){};

  Variable(const std::string& branch, const std::string& field, short size = 1) : Variable({branch, field}, size) {}

  /**
* Generic constructor for complicated variables
* @param fields vector of Fields
* @param lambda function to calculate variable
*/
  Variable(std::string name, std::vector<Field> fields, std::function<double(std::vector<double>&)> lambda);

  friend bool operator==(const Variable& that, const Variable& other);
  friend bool operator>(const Variable& that, const Variable& other);
  friend bool operator<(const Variable& that, const Variable& other);

  void Init(const Configuration& conf);

  [[nodiscard]] const std::string& GetName() const { return name_; }
  [[nodiscard]] const std::vector<Field>& GetFields() const { return fields_; }
  [[nodiscard]] size_t GetNumberOfBranches() const { return branch_names_.size(); }
  [[nodiscard]] std::set<std::string> GetBranches() const { return branch_names_; }
  [[nodiscard]] short GetSize() const { return size_; }
  [[nodiscard]] short GetId() const { return id_; }

  [[nodiscard]] std::string GetBranchName() const { return *branch_names_.begin(); }

  template<class T>
  double GetValue(const T& object) const;

  template<class A, class B>
  double GetValue(const A& a, int a_id, const B& b, int b_id) const;

  void SetName(std::string name) { name_ = std::move(name); }
  void SetSize(short size) { size_ = size; }
  void SetId(short id) { id_ = id; }

  void Print() const;

 private:
  std::string name_;
  std::vector<Field> fields_{};
  std::set<std::string> branch_names_{};
  std::set<short> branch_ids_{};

  std::function<double(std::vector<double>&)> lambda_{[](std::vector<double>& var) { return var.at(0); }};//!

  short size_{1};
  short id_{-999};

  bool is_init_{false};
};

template<class T>
double Variable::GetValue(const T& object) const {
  assert(branch_ids_.size() == 1);
  std::vector<double> vars{};
  vars.reserve(fields_.size());
  for (const auto& field : fields_) {
    vars.emplace_back(field.GetValue(object));
  }
  return lambda_(vars);
}

template<class A, class B>
double Variable::GetValue(const A& a, int a_id, const B& b, int b_id) const {
  //    assert(branch_ids_.size() == 2);
  std::vector<double> vars{};
  vars.reserve(fields_.size());
  for (const auto& field : fields_) {
    if (field.GetBranchId() == a_id)
      vars.emplace_back(field.GetValue(a));
    else if (field.GetBranchId() == b_id)
      vars.emplace_back(field.GetValue(b));
    else {
      throw std::runtime_error("Variable::Fill - Cannot fill value from branch " + field.GetBranchName());
    }
  }
  return lambda_(vars);
}

}// namespace AnalysisTree

#endif
