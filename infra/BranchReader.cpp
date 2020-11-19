
#include "BranchReader.hpp"
#include "Cuts.hpp"

namespace AnalysisTree {

BranchReader::BranchReader(std::string name, void* data, DetType type, Cuts* cuts) : name_(std::move(name)), cuts_(cuts), type_(type) {
  switch (type) {
    case DetType::kTrack: {
      data_ = (TrackDetector*) data;
      break;
    }
    case DetType::kParticle: {
      data_ = (Particles*) data;
      break;
    }
    case DetType::kHit: {
      data_ = (HitDetector*) data;
      break;
    }
    case DetType::kModule: {
      data_ = (ModuleDetector*) data;
      break;
    }
    case DetType::kEventHeader: {
      data_ = (EventHeader*) data;
      break;
    }
    default: {
      throw std::runtime_error("BranchReader::BranchReader - Wrong branch type");
    }
  }
  id_ = ANALYSISTREE_UTILS_VISIT(get_id_struct(), data_);

}

size_t BranchReader::GetNumberOfChannels() {
  return ANALYSISTREE_UTILS_VISIT(get_n_channels_struct(), data_);
}

bool BranchReader::ApplyCut(int i_channel) {
  if (!cuts_) return true;
  return ANALYSISTREE_UTILS_VISIT(apply_cut(i_channel, cuts_), data_);
}

double BranchReader::GetValue(const Variable& var, int i_channel) {
  return ANALYSISTREE_UTILS_VISIT(get_value(var, i_channel), data_);
}

}// namespace AnalysisTree