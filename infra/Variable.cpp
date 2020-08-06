#include "Variable.hpp"

#include <algorithm>

#include "Configuration.hpp"

namespace AnalysisTree {

Variable::Variable(std::string name, std::vector<Field> fields, std::function<double(std::vector<double>&)> lambda) : name_(std::move(name)),
                                                                                                                      fields_(std::move(fields)),
                                                                                                                      lambda_(std::move(lambda)) {
  for (const auto &f : fields_) {
    branch_names_.insert(f.GetBranchName());
  }
}

void Variable::Init(const Configuration& conf) {
  for (auto &field : fields_) {
    field.Init(conf);
  }
  for(const auto& branch : branch_names_){
    branch_ids_.insert(conf.GetBranchConfig(branch).GetId());
  }
  is_init_ = true;
}

void Variable::Print() const {
  std::cout << "Variable::Print():" << std::endl;
  std::cout << "  name: " << name_ << std::endl;
  for (const auto &field : fields_) {
    field.Print();
  }
  std::cout << "  id: " << id_ << std::endl;
  std::cout << "  size: " << size_ << std::endl;
}

bool operator==(const AnalysisTree::Variable &that, const AnalysisTree::Variable &other) {
  if (&that == &other) {
    return true;
  }
  return that.name_ == other.name_ && that.fields_ == other.fields_;
}

bool operator>(const AnalysisTree::Variable &that, const AnalysisTree::Variable &other) {
//  std::string field_that, field_other;
//  for (const auto &field : that.fields_) {
//    field_that += field.GetName();
//  }
//  for (const auto &field : other.fields_) {
//    field_other += field.GetName();
//  }
  return that.name_ > other.name_;
}

bool operator<(const AnalysisTree::Variable &that, const AnalysisTree::Variable &other) {
//  std::string field_that, field_other;
//  for (const auto &field : that.fields_) {
//    field_that += field.GetName();
//  }
//  for (const auto &field : other.fields_) {
//    field_other += field.GetName();
//  }
  return that.name_ > other.name_;
}



}// namespace AnalysisTree
