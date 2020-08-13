#include "Particle.hpp"

namespace AnalysisTree {

void Particle::SetPid(PdgCode_t pid) {
  pid_ = pid;
  if (mass_ == -1000.f)
    mass_ = GetMassByPdgId(pid);
}
}// namespace AnalysisTree