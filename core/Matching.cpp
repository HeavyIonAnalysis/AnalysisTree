#include "Matching.hpp"

namespace AnalysisTree{

ShortInt_t AnalysisTree::Matching::GetMatchInverted(AnalysisTree::ShortInt_t id) const {
  auto search = match_inverted_.find(id);
  if (search != match_inverted_.end()) {
    return search->second;
  } else {
    //        std::cout << "Matching::GetMatchInverted - Not found\n";
    return UndefValueShort;
  }
}
ShortInt_t Matching::GetMatchDirect(ShortInt_t id) const {
  auto search = match_.find(id);
  if (search != match_.end()) {
    return search->second;
  } else {
    //        std::cout << "Matching::GetMatchDirect - Not found\n";
    return UndefValueShort;
  }
}

}