/* Copyright (C) 2019-2021 GSI, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Eugeny Kashirin, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_BRANCHCHANNEL_HPP_
#define ANALYSISTREE_INFRA_BRANCHCHANNEL_HPP_

#include "Field.hpp"
//#include "ValueHolder.hpp"

#include <Track.hpp>
#include <Particle.hpp>
#include <Module.hpp>
#include <Hit.hpp>
#include <EventHeader.hpp>

#include <cassert>
#include <iostream>

namespace AnalysisTree {

class Branch;
class BranchChannelsIter;

class BranchChannel {
 public:
  using ChannelPointer = ANALYSISTREE_UTILS_VARIANT<Track*, Particle*, Module*, Hit*, EventHeader*>;

/* Getting value */
  [[nodiscard]] double Value(const Field& v) const;
  double operator[](const Field& v) const;;

  /* Getting value */
//  inline ValueHolder Value(const Field& v) const {
//    assert(v.GetParentBranch() == branch);
//    assert(v.IsInitialized());
//    return ValueHolder(v, data_ptr);
//  }
//  inline ValueHolder operator[](const Field& v) const { return Value(v); };
  inline std::size_t GetNChannel() const { return i_channel; }

  /* usage of this functions is highly discouraged */
  ChannelPointer Data() { return data_ptr; }
  const ChannelPointer Data() const { return data_ptr; }
  template<typename T>
  T* DataT() { return std::get<T*>(data_ptr); }
  template<typename T>
  const T* DataT() const { return std::get<T*>(data_ptr); }

  /**
   * @brief Copy contents of other branch channel
   * @param other
   * @return
   */
  void CopyContents(const BranchChannel& other);
  void CopyContents(Branch& other);

  void Print(std::ostream& os = std::cout) const;

 private:
  friend Branch;

  BranchChannel(Branch* branch, std::size_t i_channel);
  void UpdatePointer();
  void UpdateChannel(std::size_t new_channel);

  ChannelPointer data_ptr;
  Branch* branch;
  std::size_t i_channel;
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_BRANCHCHANNEL_HPP_
