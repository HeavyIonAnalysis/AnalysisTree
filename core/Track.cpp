#include "Track.hpp"

namespace AnalysisTree {

void Track::Print() const {
  std::cout << " Px = " << px_ << "  Py = " << py_ << "  Pz = " << pz_
            << "  phi = " << GetPhi() << "  pT = " << GetPt() << "  eta = " << GetEta() << std::endl;
}

bool operator==(const Track& that, const Track& other) {
  if (&that == &other) {
    return true;
  }

  if ((Container&) that != (Container&) other) {
    return false;
  }

  return that.px_ == other.px_ && that.py_ == other.py_ && that.pz_ == other.pz_;
}

}// namespace AnalysisTree