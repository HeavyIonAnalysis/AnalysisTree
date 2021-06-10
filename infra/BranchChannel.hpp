//
// Created by eugene on 13/03/2021.
//

#ifndef ATTASKSKELETON_ATI2_BRANCHCHANNEL_HPP_
#define ATTASKSKELETON_ATI2_BRANCHCHANNEL_HPP_

#include "ATI2_fwd.hpp"
#include "ValueHolder.hpp"

#include <iostream>
#include <cassert>


namespace ATI2 {

class BranchChannel {
 public:
  /* Getting value */
  inline ValueHolder Value(const Field &v) const {
    assert(v.GetParentBranch() == branch);
    assert(v.IsInitialized());
    return ValueHolder(v, data_ptr);
  }
  inline ValueHolder operator[](const Field &v) const { return Value(v); };
  inline std::size_t GetNChannel() const { return i_channel; }

  /* usage of this functions is highly discouraged */
  void *Data() { return data_ptr; }
  const void *Data() const { return data_ptr; }
  template<typename T>
  T *DataT() { return reinterpret_cast<T *>(data_ptr); }
  template<typename T>
  const T *DataT() const { return reinterpret_cast<T *>(data_ptr); }

  /**
   * @brief Copy contents of other branch channel
   * @param other
   * @return
   */
  void CopyContents(const BranchChannel &other);
  void CopyContents(Branch &other);

  void Print(std::ostream &os = std::cout) const;

 private:
  friend Branch;
  friend BranchChannelsIter;

  BranchChannel(Branch *branch, std::size_t i_channel);
  void UpdatePointer();
  void UpdateChannel(std::size_t new_channel);

  void *data_ptr{nullptr};
  Branch *branch;
  std::size_t i_channel;
};


}

#endif //ATTASKSKELETON_ATI2_BRANCHCHANNEL_HPP_
