#include "Field.hpp"

namespace AnalysisTree{

void Field::Print() const {
  std::cout << "Field::Print():" << std::endl;
  std::cout << "  branch: " << branch_ << std::endl;
  std::cout << "  field: " << field_ << std::endl;

  if(is_init_) {
    std::cout << "  branch_id: " << branch_id_ << std::endl;
    std::cout << "  branch_type: " << (short)branch_type_ << std::endl;
    std::cout << "  field_id: " << field_id_ << std::endl;
    std::cout << "  type: " << (short)field_type_ << std::endl;
  }
}

bool operator==(const AnalysisTree::Field& that, const AnalysisTree::Field& other) {
  if (&that == &other) {
    return true;
  }
  return
      that.branch_ == other.branch_ &&
      that.field_ == other.field_ &&
      that.branch_id_ == other.branch_id_ &&
      that.field_id_ == other.field_id_ &&
      that.branch_type_ == other.branch_type_ ;
}

bool operator>(const AnalysisTree::Field& that, const AnalysisTree::Field& other) {
  return that.branch_ + that.field_ > other.branch_ + other.field_;
}

bool operator<(const AnalysisTree::Field& that, const AnalysisTree::Field& other) {
  return that.branch_ + that.field_ < other.branch_ + other.field_;
}

}
