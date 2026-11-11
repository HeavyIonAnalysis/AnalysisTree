/* Copyright (C) 2019-2026 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov, Frederic Linz */
#include "Matching.hpp"

#include <algorithm>
#include <stdexcept>

namespace AnalysisTree {

void Matching::AddMatch(Integer_t id1, Integer_t id2) {
  ConvertLegacy();
  if (id1 < 0 || id2 < 0) {
    throw std::invalid_argument("Matching::AddMatch() - ids must be non-negative");
  }
  if (offsets_.empty()) {
    offsets_.push_back(0);
  }
  const size_t n_offsets = static_cast<size_t>(id1) + 2;
  if (offsets_.size() < n_offsets) {
    const Integer_t last = offsets_.back();
    offsets_.resize(n_offsets, last);
  }
  ids2_.insert(ids2_.begin() + offsets_[id1 + 1], id2);
  for (size_t i = id1 + 1; i < offsets_.size(); ++i) {
    ++offsets_[i];
  }
  inverse_valid_ = false;
}

MatchRange Matching::GetMatchesDirect(Integer_t id1) const {
  ConvertLegacy();
  if (id1 < 0 || static_cast<size_t>(id1) + 1 >= offsets_.size()) {
    return {};
  }
  return {ids2_.data() + offsets_[id1], ids2_.data() + offsets_[id1 + 1]};
}

MatchRange Matching::GetMatchesInverted(Integer_t id2) const {
  ConvertLegacy();
  if (!inverse_valid_) {
    BuildInverse();
  }
  if (id2 < 0 || static_cast<size_t>(id2) + 1 >= inv_offsets_.size()) {
    return {};
  }
  return {inv_ids1_.data() + inv_offsets_[id2], inv_ids1_.data() + inv_offsets_[id2 + 1]};
}

Integer_t Matching::GetMatchDirect(Integer_t id) const {
  const auto matches = GetMatchesDirect(id);
  return matches.empty() ? UndefValueInt : matches[0];
}

Integer_t Matching::GetMatchInverted(Integer_t id) const {
  const auto matches = GetMatchesInverted(id);
  return matches.empty() ? UndefValueInt : matches[0];
}

size_t Matching::GetNumberOfIds(bool is_inverted) const {
  ConvertLegacy();
  if (is_inverted && !inverse_valid_) {
    BuildInverse();
  }
  const auto& offsets = is_inverted ? inv_offsets_ : offsets_;
  return offsets.empty() ? 0 : offsets.size() - 1;
}

void Matching::Clear() {
  offsets_.clear();
  ids2_.clear();
  match_.clear();
  match_inverted_.clear();
  inverse_valid_ = false;
}

void Matching::BuildInverse() const {
  inv_offsets_.clear();
  inv_ids1_.clear();
  if (!ids2_.empty()) {
    // counting sort by id2; iterating over id1 in increasing order keeps each section ordered by id1
    const auto max_id2 = *std::max_element(ids2_.begin(), ids2_.end());
    inv_offsets_.assign(max_id2 + 2, 0);
    for (auto id2 : ids2_) {
      ++inv_offsets_[id2 + 1];
    }
    for (size_t i = 1; i < inv_offsets_.size(); ++i) {
      inv_offsets_[i] += inv_offsets_[i - 1];
    }
    inv_ids1_.resize(ids2_.size());
    auto next = inv_offsets_;
    for (size_t id1 = 0; id1 + 1 < offsets_.size(); ++id1) {
      for (auto i = offsets_[id1]; i < offsets_[id1 + 1]; ++i) {
        inv_ids1_[next[ids2_[i]]++] = static_cast<Integer_t>(id1);
      }
    }
  }
  inverse_valid_ = true;
}

void Matching::ConvertLegacy() const {
  if (match_.empty() && match_inverted_.empty()) {
    return;
  }
  auto* self = const_cast<Matching*>(this);// conversion does not change the logical content
  const auto match = std::move(self->match_);
  const auto match_inverted = std::move(self->match_inverted_);
  self->match_.clear();
  self->match_inverted_.clear();
  self->offsets_.clear();
  self->ids2_.clear();
  // direct pairs first, so GetMatch() returns the same partner as before
  for (const auto& m : match) {
    if (m.first >= 0 && m.second >= 0) {
      self->AddMatch(m.first, m.second);
    }
  }
  // pairs lost in the direct map because of duplicate keys (1:n) survived in the inverted one
  for (const auto& m : match_inverted) {
    const auto direct = match.find(m.second);
    const bool is_known = direct != match.end() && direct->second == m.first;
    if (!is_known && m.first >= 0 && m.second >= 0) {
      self->AddMatch(m.second, m.first);
    }
  }
}

}// namespace AnalysisTree
