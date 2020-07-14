#include "Field.hpp"
#include "Configuration.hpp"

namespace AnalysisTree {

void Field::Print() const {
  std::cout << "Field::Print():" << std::endl;
  std::cout << "  branch: " << branch_ << std::endl;
  std::cout << "  field: " << field_ << std::endl;

  if (is_init_) {
    std::cout << "  branch_id: " << branch_id_ << std::endl;
    std::cout << "  branch_type: " << (short) branch_type_ << std::endl;
    std::cout << "  field_id: " << field_id_ << std::endl;
    std::cout << "  type: " << (short) field_type_ << std::endl;
  }
}

bool operator==(const AnalysisTree::Field &that, const AnalysisTree::Field &other) {
  if (&that == &other) {
    return true;
  }
  return that.branch_ == other.branch_ && that.field_ == other.field_ && that.branch_id_ == other.branch_id_ && that.field_id_ == other.field_id_ && that.branch_type_ == other.branch_type_;
}

bool operator>(const AnalysisTree::Field &that, const AnalysisTree::Field &other) {
  return that.branch_ + that.field_ > other.branch_ + other.field_;
}

bool operator<(const AnalysisTree::Field &that, const AnalysisTree::Field &other) {
  return that.branch_ + that.field_ < other.branch_ + other.field_;
}

void Field::Init(const Configuration& conf) {
  const auto &branch_conf = conf.GetBranchConfig(branch_);
  branch_id_ = branch_conf.GetId();
  branch_type_ = branch_conf.GetType();
  field_id_ = branch_conf.GetFieldId(field_);
  field_type_ = branch_conf.GetFieldType(field_);
  if (field_id_ == UndefValueInt) {
    std::cout << "WARNING!! Field::Init - " << field_ << " is not found in branch " << branch_ << std::endl;
  }
  is_init_ = true;
}

}// namespace AnalysisTree
