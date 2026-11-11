/* Copyright (C) 2019-2026 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov, Frederic Linz */
#ifndef ANALYSISTREE_MATCHING_H
#define ANALYSISTREE_MATCHING_H

#include "Constants.hpp"
#include <map>
#include <vector>

namespace AnalysisTree {

/**
 * Non-owning view of the partner ids of one object.
 * Valid until the Matching is modified or the next entry is read.
 */
class MatchRange {
 public:
  MatchRange() = default;
  MatchRange(const Integer_t* begin, const Integer_t* end) : begin_(begin), end_(end) {}

  ANALYSISTREE_ATTR_NODISCARD const Integer_t* begin() const { return begin_; }
  ANALYSISTREE_ATTR_NODISCARD const Integer_t* end() const { return end_; }
  ANALYSISTREE_ATTR_NODISCARD size_t size() const { return end_ - begin_; }
  ANALYSISTREE_ATTR_NODISCARD bool empty() const { return begin_ == end_; }
  ANALYSISTREE_ATTR_NODISCARD Integer_t operator[](size_t i) const { return begin_[i]; }

 private:
  const Integer_t* begin_{nullptr};//!
  const Integer_t* end_{nullptr};  //!
};

/**
 * A class for a matching information between channels of two branches (id1 of branch1, id2 of branch2).
 *
 * All pairs are stored in CSR layout: the partners of id1 are ids2_[offsets_[id1], offsets_[id1 + 1]),
 * in the order they were added. By convention the best partner is added first, GetMatch() returns it.
 * The inverse direction (id2 -> id1) is not persisted, it is built on first use and ordered by id1.
 * Ids are channel ids, i.e. indices in the Detector, and must be non-negative.
 */
class Matching {

 public:
  Matching() = default;
  Matching(size_t id1, size_t id2) : branch1_id_(id1), branch2_id_(id2){};

  virtual ~Matching() = default;

  /**
   * Adds a pair. Adding in increasing order of id1 is O(1), otherwise the pair is inserted in place.
   */
  void AddMatch(Integer_t id1, Integer_t id2);

  /**
   * All partners of an object; empty if there is none
   */
  ANALYSISTREE_ATTR_NODISCARD MatchRange GetMatchesDirect(Integer_t id1) const;
  ANALYSISTREE_ATTR_NODISCARD MatchRange GetMatchesInverted(Integer_t id2) const;
  ANALYSISTREE_ATTR_NODISCARD MatchRange GetMatches(Integer_t id, bool is_inverted) const {
    return is_inverted ? GetMatchesInverted(id) : GetMatchesDirect(id);
  }

  /**
   * First partner of an object (inverted: smallest id1); UndefValueInt if there is none
   */
  ANALYSISTREE_ATTR_NODISCARD Integer_t GetMatchDirect(Integer_t id) const;
  ANALYSISTREE_ATTR_NODISCARD Integer_t GetMatchInverted(Integer_t id) const;
  ANALYSISTREE_ATTR_NODISCARD Integer_t GetMatch(Integer_t id, bool is_inverted = false) const {
    return is_inverted ? GetMatchInverted(id) : GetMatchDirect(id);
  }

  ANALYSISTREE_ATTR_NODISCARD size_t GetNumberOfPairs() const { return ids2_.size(); }

  /**
   * Upper bound of the ids on the origin side: all ids with partners are smaller
   */
  ANALYSISTREE_ATTR_NODISCARD size_t GetNumberOfIds(bool is_inverted = false) const;

  void Clear();

  ANALYSISTREE_ATTR_NODISCARD size_t GetBranch1Id() const { return branch1_id_; }
  ANALYSISTREE_ATTR_NODISCARD size_t GetBranch2Id() const { return branch2_id_; }

  /**
   * Converts the two maps of Matching version <= 2 into the CSR layout.
   * Pairs of both maps are kept; the pair of the direct map comes first for each id1.
   * Public only to be callable from the I/O read rule in AnalysisTreeCoreLinkDef.h.
   */
  static void FillFromLegacyMaps(const std::map<Integer_t, Integer_t>& match,
                                 const std::map<Integer_t, Integer_t>& match_inverted,
                                 std::vector<Integer_t>& offsets, std::vector<Integer_t>& ids2);

 protected:
  void BuildInverse() const;

  size_t branch1_id_{0};
  size_t branch2_id_{0};

  std::vector<Integer_t> offsets_{};///< partners of id1 are ids2_[offsets_[id1], offsets_[id1 + 1])
  std::vector<Integer_t> ids2_{};   ///< partner ids grouped by id1

  mutable std::vector<Integer_t> inv_offsets_{};//! inverse CSR, built on first use
  mutable std::vector<Integer_t> inv_ids1_{};   //!
  mutable bool inverse_valid_{false};           //! reset after each read by an I/O read rule

  ClassDef(Matching, 3)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_MATCHING_H
