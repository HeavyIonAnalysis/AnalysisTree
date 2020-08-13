#ifndef ANALYSISTREE_BASECONTAINER_H
#define ANALYSISTREE_BASECONTAINER_H

#include "BranchConfig.hpp"
#include "IndexedObject.hpp"

namespace AnalysisTree {

template<typename T>
class Vector {
 public:
  Vector() = default;
  Vector(const Vector&) = default;
  Vector(Vector&&) = default;
  Vector& operator=(Vector&&) = default;
  Vector& operator=(const Vector&) = default;
  virtual ~Vector() = default;

  friend bool operator==(const Vector& that, const Vector& other) {
    if (&that == &other) {
      return true;
    }
    return that.field_ == other.field_;
  }

  virtual void SetField(T value, Integer_t iField) { field_.at(iField) = value; }//NOTE
  [[nodiscard]] virtual T GetField(Integer_t iField) const { return field_.at(iField); }

  void Resize(int size) {
    field_.resize(size);
  }

 protected:
  std::vector<T> field_{};
};

/**
 * A class to store integers, floats and bools
 */
class Container : public IndexedObject, public Vector<int>, public Vector<float>, public Vector<bool> {
 public:
  Container() = default;

  explicit Container(Integer_t id) : IndexedObject(id) {}
  Container(const Container& container) = default;
  Container(Container&& container) = default;
  Container& operator=(Container&&) = default;
  Container& operator=(const Container& part) = default;

  template<typename T>
  void SetField(T value, Integer_t iField) { Vector<T>::SetField(value, iField); }
  template<typename T>
  [[nodiscard]] T GetField(Integer_t iField) const { return Vector<T>::GetField(iField); }
  template<typename T>
  [[nodiscard]] size_t GetSize() const { return Vector<T>::field_.size(); }

  void Init(const BranchConfig& branch);
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_BASECONTAINER_H
