#ifndef ANALYSISTREE_MATCHING_H
#define ANALYSISTREE_MATCHING_H

#include "Constants.hpp"
#include <map>

namespace AnalysisTree {

/**
 * A class for a matching information
 */

class Matching {
 public:
  Matching() = default;
  Matching(ShortInt_t id1, ShortInt_t id2) : branch1_id_(id1), branch2_id_(id2){};
  virtual ~Matching() = default;

  void AddMatch(Integer_t id1, Integer_t id2) {
    match_.insert(std::make_pair(id1, id2));
    match_inverted_.insert(std::make_pair(id2, id1));
  }

  [[nodiscard]] Integer_t GetMatchDirect(Integer_t id) const;
  [[nodiscard]] Integer_t GetMatchInverted(Integer_t id) const;
  [[nodiscard]] Integer_t GetMatch(Integer_t id, bool is_inverted = false) const {
    return is_inverted ? GetMatchInverted(id) : GetMatchDirect(id);
  }

  [[nodiscard]] const std::map<Integer_t, Integer_t>& GetMatches(bool is_inv = false) const {
    return is_inv ? match_inverted_ : match_;
  }

  void Clear() {
    match_.clear();
    match_inverted_.clear();
  }

  [[nodiscard]] ShortInt_t GetBranch1Id() const { return branch1_id_; }
  [[nodiscard]] ShortInt_t GetBranch2Id() const { return branch2_id_; }

 protected:
  ShortInt_t branch1_id_{UndefValueShort};
  ShortInt_t branch2_id_{UndefValueShort};

  std::map<Integer_t, Integer_t> match_{};
  std::map<Integer_t, Integer_t> match_inverted_{};//TODO is there a better way? Boost.Bimap?

  ClassDef(AnalysisTree::Matching, 1)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_MATCHING_H
