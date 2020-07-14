#include "Container.hpp"

#include "BranchConfig.hpp"

namespace AnalysisTree{

void Container::Init(const AnalysisTree::BranchConfig& branch) {
  Vector<float>::Resize(branch.GetSize<float>());
  Vector<int>::Resize(branch.GetSize<int>());
  Vector<bool>::Resize(branch.GetSize<bool>());
}

}