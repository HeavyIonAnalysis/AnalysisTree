#include "Variable.hpp"

namespace AnalysisTree {

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
  return that.name_ == other.name_ && that.fields_ == other.fields_ && that.size_ == other.size_;
}

bool operator>(const AnalysisTree::Variable &that, const AnalysisTree::Variable &other) {
  std::string field_that, field_other;
  for (const auto &field : that.fields_) {
    field_that += field.GetName();
  }
  for (const auto &field : other.fields_) {
    field_other += field.GetName();
  }
  return field_that + that.name_ > field_other + other.name_;
}

bool operator<(const AnalysisTree::Variable &that, const AnalysisTree::Variable &other) {
  std::string field_that, field_other;
  for (const auto &field : that.fields_) {
    field_that += field.GetName();
  }
  for (const auto &field : other.fields_) {
    field_other += field.GetName();
  }
  return field_that + that.name_ < field_other + other.name_;
}

}// namespace AnalysisTree
