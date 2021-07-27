/* Copyright (C) 2019-2021 GSI, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Eugeny Kashirin, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_BRANCHCHANNEL_HPP_
#define ANALYSISTREE_INFRA_BRANCHCHANNEL_HPP_

#include <cassert>
#include <iostream>

#include "Utils.hpp"

namespace AnalysisTree {

class Branch;
class Field;

class BranchChannel {
 public:
  BranchChannel() = default;
  BranchChannel(const ChannelPointer& data_ptr, const Branch* branch= nullptr, size_t i_channel=0)
  : data_ptr_(data_ptr),
    branch_(branch),
    i_channel_(i_channel) {}

/* Getting value */
  [[nodiscard]] double Value(const Field& v) const;
  [[nodiscard]] double operator[](const Field& v) const;
  [[nodiscard]] inline std::size_t GetChannelNumber() const { return i_channel_; }

  void SetValue(const Field& v, double value);

  template<typename T>
  T* Data() { return ANALYSISTREE_UTILS_GET<T*>(data_ptr_); }
  template<typename T>
  const T* Data() const { return ANALYSISTREE_UTILS_GET<T*>(data_ptr_); }

  /**
   * @brief Copy contents of other branch channel
   * @param other
   * @return
   */
  void CopyContents(const BranchChannel& other);

  void Print(std::ostream& os = std::cout) const;

 private:
  friend Branch;

  BranchChannel(const Branch* branch, std::size_t i_channel);
  void UpdatePointer();
  void UpdateChannel(std::size_t new_channel);

  ChannelPointer data_ptr_;
  const Branch* branch_{nullptr};
  std::size_t i_channel_{0};
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_BRANCHCHANNEL_HPP_
