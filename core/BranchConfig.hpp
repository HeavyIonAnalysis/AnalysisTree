
#ifndef ANALYSISTREE_BRANCHCONFIG_H
#define ANALYSISTREE_BRANCHCONFIG_H

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <TObject.h>

#include "Constants.hpp"

namespace AnalysisTree {

struct ConfigElement {
  ConfigElement() = default;
  explicit ConfigElement(ShortInt_t id, std::string title="") : id_(id), title_(std::move(title)) {}
  ShortInt_t id_{0};
  std::string title_;
};

template<typename T>
class VectorConfig {
 public:
  typedef std::map<std::string, ConfigElement> MapType;

  VectorConfig() = default;
  VectorConfig(const VectorConfig&) = default;
  VectorConfig(VectorConfig&&) noexcept(std::is_nothrow_move_constructible<MapType>::value) = default;
  VectorConfig& operator=(VectorConfig&&) noexcept(std::is_nothrow_move_assignable<MapType>::value) = default;
  VectorConfig& operator=(const VectorConfig&) = default;
  virtual ~VectorConfig() = default;

  virtual void AddField(const std::string& name, const std::string& title) {
    map_.insert(std::pair<std::string, ConfigElement>(name, ConfigElement(size_++, title)));
  }
  void AddField(const std::string& name, ShortInt_t id, const std::string& title = "") {
    map_.insert(std::pair<std::string, ConfigElement>(name, ConfigElement(id, title)));
  }

  virtual void AddFields(const std::vector<std::string>& names) {
    for (const auto& name : names) {
      map_.insert(std::pair<std::string, ConfigElement>(name, ConfigElement(size_++)));
    }
  }

  ANALYSISTREE_ATTR_NODISCARD ShortInt_t GetId(const std::string& sField) const {
    auto search = map_.find(sField);
    if (search != map_.end()) {
      return search->second.id_;
    } else {
      return UndefValueShort;
    }
  }

  ANALYSISTREE_ATTR_NODISCARD virtual const MapType& GetMap() const { return map_; }
  ANALYSISTREE_ATTR_NODISCARD virtual ShortInt_t GetSize() const { return size_; }

  virtual void Print() const;

 protected:
  MapType map_{};
  ShortInt_t size_{0};
  ClassDef(VectorConfig, 1)

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

  void Print() const override;

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

  ClassDefOverride(BranchConfig, 2)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_BRANCHCONFIG_H
