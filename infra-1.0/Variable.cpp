#include "Variable.hpp"

#include <algorithm>

#include "Configuration.hpp"

namespace AnalysisTree::Version1 {

Variable::Variable(std::string name, std::vector<Field> fields, std::function<double(std::vector<double>&)> lambda) : name_(std::move(name)),
                                                                                                                      fields_(std::move(fields)),
                                                                                                                      lambda_(std::move(lambda)) {
  for (const auto& f : fields_) {
    branch_names_.insert(f.GetBranchName());
  }
}

void Variable::Init(const Configuration& conf) {
  for (auto& field : fields_) {
    field.Init(conf);
  }
  for (const auto& branch : branch_names_) {
    branch_ids_.insert(conf.GetBranchConfig(branch).GetId());
  }
  is_init_ = true;
}

void Variable::Print() const {
  std::cout << "Variable::Print():" << std::endl;
  std::cout << "  name: " << name_ << std::endl;
  for (const auto& field : fields_) {
    field.Print();
  }
  std::cout << "  id: " << id_ << std::endl;
  std::cout << "  size: " << size_ << std::endl;
}

bool operator==(const Variable& that, const Variable& other) {
  if (&that == &other) {
    return true;
  }
  return that.name_ == other.name_ && that.fields_ == other.fields_;
}

bool operator>(const Variable& that, const Variable& other) {
  //  std::string field_that, field_other;
  //  for (const auto &field : that.fields_) {
  //    field_that += field.GetName();
  //  }
  //  for (const auto &field : other.fields_) {
  //    field_other += field.GetName();
  //  }
  return that.name_ > other.name_;
}

bool operator<(const Variable& that, const Variable& other) {
  //  std::string field_that, field_other;
  //  for (const auto &field : that.fields_) {
  //    field_that += field.GetName();
  //  }
  //  for (const auto &field : other.fields_) {
  //    field_other += field.GetName();
  //  }
  return that.name_ > other.name_;
}

Variable Variable::FromString(const std::string& full_name) {
  const std::regex name_regex("^(\\w+)[\\.\\/](\\w+)$");
  std::smatch match;
  auto search_ok = std::regex_search(full_name, match, name_regex);
  if (search_ok) {
    std::string branch_name{match.str(1)};
    std::string field_name{match.str(2)};
    return Variable(branch_name, field_name);
  }
  throw std::runtime_error("Field name must be in the format <branch>.<name>");
}

}// namespace AnalysisTree::Version1
