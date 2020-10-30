
#include "BranchReader.hpp"
#include "Cuts.hpp"

namespace AnalysisTree {

BranchReader::BranchReader(std::string name, BranchPointer data, DetType type, Cuts* cuts) :
  name_(std::move(name)),
  data_(data),
  cuts_(cuts),
  type_(type)
{
#ifdef USEBOOST
  id_ = boost::apply_visitor(get_id_struct(), data_);
#else
  id_ = std::visit([](auto&& arg) { return arg->GetId(); }, data_);
#endif
}

size_t BranchReader::GetNumberOfChannels() const {
#ifdef USEBOOST
  return boost::apply_visitor(get_n_channels_struct(), data_);
#else
  size_t n = std::visit([](auto&& arg) { return arg->GetNumberOfChannels(); }, data_);
  return n;
#endif
}

bool BranchReader::ApplyCut(int i_channel) const {
  if (!cuts_) return true;
#ifdef USEBOOST
  return boost::apply_visitor(apply_cut(i_channel, cuts_), data_);
#else
  return std::visit([i_channel, this](auto&& arg) { return cuts_->Apply(arg->GetChannel(i_channel)); }, data_);
#endif
}

double BranchReader::GetValue(const Variable& var, int i_channel) const {
#ifdef USEBOOST
  return boost::apply_visitor(get_value(var, i_channel), data_);
#else
  return std::visit([&var, i_channel](auto&& arg) { return var.GetValue(arg->GetChannel(i_channel)); }, data_);
#endif
}

}// namespace AnalysisTree