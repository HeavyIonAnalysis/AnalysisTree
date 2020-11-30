
#ifndef ANALYSISTREE_BRANCHCONFIG_H
#define ANALYSISTREE_BRANCHCONFIG_H

#include <map>
#include <string>
#include <vector>

#include <TObject.h>

#include "Constants.hpp"

namespace AnalysisTree {

template<typename T>
class VectorConfig {
 public:
  typedef std::pair<ShortInt_t, std::string> MapElement;
  typedef std::map<std::string, MapElement> MapType;

  VectorConfig() = default;
  VectorConfig(const VectorConfig&) = default;
  VectorConfig(VectorConfig&&) noexcept(std::is_nothrow_move_constructible<MapType>::value) = default;
  VectorConfig& operator=(VectorConfig&&) noexcept(std::is_nothrow_move_assignable<MapType>::value) = default;
  VectorConfig& operator=(const VectorConfig&) = default;
  virtual ~VectorConfig() = default;

  virtual void AddField(const std::string& name, const std::string& title) {
    map_.insert(std::make_pair(name, std::make_pair(size_++, title)));
  }
  void AddField(const std::string& name, ShortInt_t id, const std::string& title = "") {
    map_.insert(std::make_pair(name, std::make_pair(id, title)));
  }

  virtual void AddFields(const std::vector<std::string>& names) {
    for (const auto& name : names) {
      map_.insert(std::make_pair(name, std::make_pair(size_++, "")));
    }
  }

  ANALYSISTREE_ATTR_NODISCARD ShortInt_t GetId(const std::string& sField) const {
    auto search = map_.find(sField);
    if (search != map_.end()) {
      return search->second.first;
    } else {
      return UndefValueShort;
    }
  }

  ANALYSISTREE_ATTR_NODISCARD virtual const MapType& GetMap() const { return map_; }
  ANALYSISTREE_ATTR_NODISCARD virtual ShortInt_t GetSize() const { return size_; }

 protected:
  MapType map_{};
  ShortInt_t size_{0};
};

class BranchConfig : public VectorConfig<int>, public VectorConfig<float>, public VectorConfig<bool> {

 public:
  BranchConfig() = default;
  BranchConfig(const BranchConfig&) = default;
  BranchConfig(BranchConfig&&) = default;
  BranchConfig& operator=(BranchConfig&&) = default;
  BranchConfig& operator=(const BranchConfig&) = default;
  ~BranchConfig() override = default;

  explicit BranchConfig(std::string name, DetType type);

  void Print() const;

  ANALYSISTREE_ATTR_NODISCARD Types GetFieldType(const std::string& sField) const;
  ANALYSISTREE_ATTR_NODISCARD ShortInt_t GetFieldId(const std::string& sField) const;

  // Setters
  void SetId(ShortInt_t id) { id_ = id; }
  template<typename T>
  void AddField(const std::string& name, const std::string& title = "") {
    VectorConfig<T>::AddField(name, title);
  }
  template<typename T>
  void AddFields(const std::vector<std::string>& names) { VectorConfig<T>::AddFields(names); }

  // Getters
  template<typename T>
  ANALYSISTREE_ATTR_NODISCARD const VectorConfig<int>::MapType& GetMap() const { return VectorConfig<T>::GetMap(); }
  template<typename T>
  ANALYSISTREE_ATTR_NODISCARD ShortInt_t GetSize() const { return VectorConfig<T>::GetSize(); }

  ANALYSISTREE_ATTR_NODISCARD std::string GetName() const { return name_; }
  ANALYSISTREE_ATTR_NODISCARD ShortInt_t GetId() const { return id_; }
  ANALYSISTREE_ATTR_NODISCARD DetType GetType() const { return type_; }

 protected:
  std::string name_;
  ShortInt_t id_{UndefValueShort};
  DetType type_{DetType(UndefValueShort)};

  ClassDef(AnalysisTree::BranchConfig, 2)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_BRANCHCONFIG_H
