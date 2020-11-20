#include "Container.hpp"

#include "BranchConfig.hpp"

namespace AnalysisTree {

template <> std::vector<int>& Container::Vector<int>() { return ints_;}
template <> std::vector<float>& Container::Vector<float>() { return floats_;}
template <> std::vector<bool>& Container::Vector<bool>() { return bools_;}

template <> const std::vector<int>& Container::GetVector<int>() const { return ints_;}
template <> const std::vector<float>& Container::GetVector<float>() const { return floats_;}
template <> const std::vector<bool>& Container::GetVector<bool>() const { return bools_;}

void Container::Init(const AnalysisTree::BranchConfig& branch) {
   Vector<float>().resize(branch.GetSize<float>());
   Vector<int>().resize(branch.GetSize<int>());
   Vector<bool>().resize(branch.GetSize<bool>());
}



}// namespace AnalysisTree