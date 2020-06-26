#ifndef ANALYSISTREE_MATCHING_H
#define ANALYSISTREE_MATCHING_H

#include <map>
#include <iostream>

#include "Constants.hpp"

namespace AnalysisTree{

/**
 * A class for a matching information
 */

class Matching {
public:
  Matching() = default;
  Matching(ShortInt_t id1, ShortInt_t id2) : branch1_id_(id1), branch2_id_(id2) {};
  virtual ~Matching() = default;
  
  void AddMatch(ShortInt_t id1, ShortInt_t id2)
  {
    match_.insert( std::pair<ShortInt_t, ShortInt_t>(id1, id2) ); 
    match_inverted_.insert( std::pair<ShortInt_t, ShortInt_t>(id2, id1) ); 
  }
    
  ShortInt_t GetMatchDirect(ShortInt_t id) const {
    auto search = match_.find(id);
    if (search != match_.end()){
      return search->second;
    } else {
//        std::cout << "Matching::GetMatchDirect - Not found\n";
        return UndefValueShort;
    }
  }

  ShortInt_t GetMatchInverted(ShortInt_t id) const {
    auto search = match_inverted_.find(id);
    if (search != match_inverted_.end()){
      return search->second;
    } else {
//        std::cout << "Matching::GetMatchInverted - Not found\n";
        return UndefValueShort;
    }
  }

  ShortInt_t GetMatch(ShortInt_t id, bool is_inverted=false) const {
    return is_inverted ? GetMatchInverted(id) : GetMatchDirect(id);
  }

  void Clear(){
    match_.clear();
    match_inverted_.clear();
  }

protected:
  
  ShortInt_t branch1_id_{UndefValueShort};
  ShortInt_t branch2_id_{UndefValueShort};

  std::map<ShortInt_t, ShortInt_t> match_{};
  std::map<ShortInt_t, ShortInt_t> match_inverted_{}; //TODO is there a better way? Boost.Bimap?
  
  ClassDef(AnalysisTree::Matching, 1)
};


}
#endif //ANALYSISTREE_MATCHING_H
