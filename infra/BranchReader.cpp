#include "BranchReader.hpp"

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

void BranchReader::ClearAndReserveOutput(int n_channels) {
  values_.resize(vars_.size());
  for (auto &v : values_) {
    v.clear();
    v.reserve(n_channels);
  }
}

void BranchReader::FillValues() {
  const auto n_channels = GetNumberOfChannels();
  ClearAndReserveOutput(n_channels);
  for (size_t i_channel = 0; i_channel < n_channels; ++i_channel) {
    if (!ApplyCut(i_channel)) continue;
    int i_var{0};

    for (const auto &var : vars_) {
      if (var.GetNumberOfBranches() == 1) {
        double value = std::visit([var, i_channel](auto &&arg) { return var.GetValue(arg->GetChannel(i_channel)); }, data_);
        values_.at(i_var).emplace_back(value);
      }
      else{
        throw std::runtime_error("Trying to fill variable with more than 1 branch");
      }
      i_var++;
    }//variables
  }  //channels
}

}// namespace AnalysisTree