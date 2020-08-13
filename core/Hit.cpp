#include "Hit.hpp"

namespace AnalysisTree {

void Hit::Print() const {
  std::cout << "  x = " << x_ << "  y = " << y_ << "  z = " << z_ << "  phi = " << GetPhi() << "  signal = " << signal_ << std::endl;
}

bool operator==(const Hit& that, const Hit& other) {
  if (&that == &other) {
    return true;
  }

  if ((Container&) that != (Container&) other) {
    return false;
  }

  return that.x_ == other.x_ && that.y_ == other.y_ && that.z_ == other.z_ && that.signal_ == other.signal_;
}

Floating_t Hit::GetPhi() const { return atan2(y_, x_); }

}// namespace AnalysisTree