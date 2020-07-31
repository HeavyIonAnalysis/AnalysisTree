
#include "BranchReader.hpp"
#include "Cuts.hpp"

namespace AnalysisTree {

BranchReader::BranchReader(std::string name, void *data, DetType type, Cuts *cuts) : name_(std::move(name)), cuts_(cuts), type_(type) {
  switch (type) {
    case DetType::kTrack: {
      data_ = (TrackDetector *) data;
      break;
    }
    case DetType::kParticle: {
      data_ = (Particles *) data;
      break;
    }
    case DetType::kHit: {
      data_ = (HitDetector *) data;
      break;
    }
    case DetType::kModule: {
      data_ = (ModuleDetector *) data;
      break;
    }
    case DetType::kEventHeader: {
      data_ = (EventHeader *) data;
      break;
    }
    default: {
      throw std::runtime_error("BranchReader::BranchReader - Wrong branch type");
    }
  }
  id_ = std::visit([](auto &&arg) { return arg->GetId(); }, data_);
}

size_t BranchReader::GetNumberOfChannels() {
#ifdef USEBOOST
  return boost::apply_visitor(get_n_channels_struct(), data_);
#else
  return std::visit([](auto &&arg) { return arg->GetNumberOfChannels(); }, data_);
#endif
}

bool BranchReader::ApplyCut(int i_channel) {
  if (i_channel < 0) return false;
  if (!cuts_) return true;
#ifdef USEBOOST
  return boost::apply_visitor(apply_cut(i_channel, cuts_), data_);
#else
  return std::visit([i_channel, this](auto &&arg) { return cuts_->Apply(arg->GetChannel(i_channel)); }, data_);
#endif
}

}// namespace AnalysisTree