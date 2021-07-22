/* Copyright (C) 2019-2021 GSI, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Eugeny Kashirin, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_BRANCHCHANNEL_HPP_
#define ANALYSISTREE_INFRA_BRANCHCHANNEL_HPP_

#include "Field.hpp"

#include <Track.hpp>
#include <Particle.hpp>
#include <Module.hpp>
#include <Hit.hpp>
#include <EventHeader.hpp>

#include <cassert>
#include <iostream>

namespace AnalysisTree {

class Branch;

class BranchChannel {
 public:

/* Getting value */
  [[nodiscard]] double Value(const Field& v) const;
  [[nodiscard]] double operator[](const Field& v) const;
  [[nodiscard]] inline std::size_t GetChannelNumber() const { return i_channel; }

  void SetValue(const Field& v, double value);

  template<typename T>
  T* Data() { return ANALYSISTREE_UTILS_GET<T*>(data_ptr); }
  template<typename T>
  const T* Data() const { return ANALYSISTREE_UTILS_GET<T*>(data_ptr); }

  /**
   * @brief Copy contents of other branch channel
   * @param other
   * @return
   */
  void CopyContents(const BranchChannel& other);
//  void CopyContents(Branch& other);

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
