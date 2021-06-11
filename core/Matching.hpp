/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_MATCHING_H
#define ANALYSISTREE_MATCHING_H

#include "Constants.hpp"
#include <map>
#include <utility>

namespace AnalysisTree {

/**
 * A class for a matching information
 */

class Matching {

  typedef std::map<Integer_t, Integer_t> MapType;

 public:
  Matching() = default;
  Matching(size_t id1, size_t id2) : branch1_id_(id1), branch2_id_(id2){};
  Matching(size_t id1, size_t id2, MapType match, MapType match_inverted) : branch1_id_(id1),
                                                                            branch2_id_(id2),
                                                                            match_(std::move(match)),
                                                                            match_inverted_(std::move(match_inverted)){};

  virtual ~Matching() = default;

  void AddMatch(Integer_t id1, Integer_t id2) {
    match_.insert(std::make_pair(id1, id2));
    match_inverted_.insert(std::make_pair(id2, id1));
  }

  ANALYSISTREE_ATTR_NODISCARD Integer_t GetMatchDirect(Integer_t id) const;
  ANALYSISTREE_ATTR_NODISCARD Integer_t GetMatchInverted(Integer_t id) const;
  ANALYSISTREE_ATTR_NODISCARD Integer_t GetMatch(Integer_t id, bool is_inverted = false) const {
    return is_inverted ? GetMatchInverted(id) : GetMatchDirect(id);
  }

  ANALYSISTREE_ATTR_NODISCARD const MapType& GetMatches(bool is_inv = false) const {
    return is_inv ? match_inverted_ : match_;
  }

  void Clear() {
    match_.clear();
    match_inverted_.clear();
  }

  ANALYSISTREE_ATTR_NODISCARD size_t GetBranch1Id() const { return branch1_id_; }
  ANALYSISTREE_ATTR_NODISCARD size_t GetBranch2Id() const { return branch2_id_; }

  void SetMatches(MapType match, MapType match_inv) {
    match_ = std::move(match);
    match_inverted_ = std::move(match_inv);
  }

 protected:
  size_t branch1_id_{0};
  size_t branch2_id_{0};

  MapType match_{};
  MapType match_inverted_{};//TODO is there a better way? Boost.Bimap?

  ClassDef(Matching, 2)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_MATCHING_H
