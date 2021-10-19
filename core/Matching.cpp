/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "Matching.hpp"

namespace AnalysisTree {

Integer_t AnalysisTree::Matching::GetMatchInverted(AnalysisTree::Integer_t id) const {
  auto search = match_inverted_.find(id);
  if (search != match_inverted_.end()) {
    return search->second;
  } else {
    return UndefValueInt;
  }
}
Integer_t Matching::GetMatchDirect(Integer_t id) const {
  auto search = match_.find(id);
  if (search != match_.end()) {
    return search->second;
  } else {
    return UndefValueInt;
  }
}

}// namespace AnalysisTree