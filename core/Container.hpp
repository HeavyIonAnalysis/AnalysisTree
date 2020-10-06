#ifndef ANALYSISTREE_BASECONTAINER_H
#define ANALYSISTREE_BASECONTAINER_H

#include "BranchConfig.hpp"
#include "IndexedObject.hpp"

namespace AnalysisTree {


/*! \brief A class to store any number of integers, floats and bools
 *
 *  Consists of IndexedObject and separate std::vector<T>, for T={float, int, bool}.
 */

class Container : public IndexedObject {
 public:
  Container() = default;

  explicit Container(Integer_t id) : IndexedObject(id) {}
  Container(const Container& container) = default;
  Container(Container&& container) = default;
  Container& operator=(Container&&) = default;
  Container& operator=(const Container& part) = default;
  ~Container() override {
      std::cout << "~Container()" << std::endl;
  };

  template<class T>
  std::vector<T>& Vector();

  template<class T>
  const std::vector<T>& GetVector() const;

  template<typename T>
  void SetField(T value, Integer_t field_id){
    Vector<T>().at(field_id) = value;
  }

  template<typename T>
  [[nodiscard]] T GetField(Integer_t field_id) const {
    return GetVector<T>().at(field_id);
  }

  template<typename T>
  [[nodiscard]] size_t GetSize() const {
    return GetVector<T>().size();
  }

  void Init(const BranchConfig& branch);

 protected:

  std::vector<float> floats_{};
  std::vector<int> ints_{};
  std::vector<bool> bools_{};

  ClassDefOverride(AnalysisTree::Container, 2);

};

}// namespace AnalysisTree
#endif//ANALYSISTREE_BASECONTAINER_H
