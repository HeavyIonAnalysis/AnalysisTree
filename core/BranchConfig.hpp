
#ifndef ANALYSISTREE_BRANCHCONFIG_H
#define ANALYSISTREE_BRANCHCONFIG_H

#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <TObject.h>

#include "Constants.hpp"

namespace AnalysisTree {

struct ConfigElement {
  ConfigElement() = default;
  virtual ~ConfigElement() = default;

  explicit ConfigElement(ShortInt_t id, std::string title) : id_(id), title_(std::move(title)) {}
  ShortInt_t id_{0};
  std::string title_;

 protected:
  ClassDef(ConfigElement, 1)
};

template<typename T>
class VectorConfig {
 public:
  typedef std::map<std::string, ConfigElement> MapType;

  VectorConfig() = default;
  VectorConfig(const VectorConfig&) = default;
  explicit VectorConfig(const MapType& map) : map_(map), size_(map.size()) {}
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

  virtual void AddFields(const std::vector<std::string>& names, const std::string& title) {
    for (const auto& name : names) {
      map_.insert(std::pair<std::string, ConfigElement>(name, ConfigElement(size_++, title)));
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

  virtual void Print() const {
    if (map_.empty()) return;

    auto print_row = [](const std::vector<std::pair<std::string, int>>& elements) {
      for (const auto& el : elements) {
        std::cout << std::left << std::setw(el.second) << std::setfill(' ') << el.first;
      }
      std::cout << std::endl;
    };

    print_row({{"Id", 10}, {"Name", 20}, {"Info", 50}});
    for (const auto& entry : map_) {
      print_row({{std::to_string(entry.second.id_), 10}, {entry.first, 20}, {entry.second.title_, 50}});
    }
  }

 protected:
  MapType map_{};
  ShortInt_t size_{0};
  ClassDef(VectorConfig, 2)
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

  BranchConfig(std::string name, DetType type, const VectorConfig<int>::MapType& ints, const VectorConfig<float>::MapType& floats, const VectorConfig<bool>::MapType& bools) :
    VectorConfig<int>::VectorConfig<int>(ints),
    VectorConfig<float>::VectorConfig<float>(floats),
    VectorConfig<bool>::VectorConfig<bool>(bools),
    name_(std::move(name)),
    type_(type)
  {}

  void Print() const override;

  ANALYSISTREE_ATTR_NODISCARD Types GetFieldType(const std::string& sField) const;
  ANALYSISTREE_ATTR_NODISCARD ShortInt_t GetFieldId(const std::string& sField) const;

  // Setters
  void SetId(size_t id) { id_ = id; }
  template<typename T>
  void AddField(const std::string& name, const std::string& title = "") {
    VectorConfig<T>::AddField(name, title);
  }
  template<typename T>
  void AddFields(const std::vector<std::string>& names, const std::string& title = "") {
    VectorConfig<T>::AddFields(names, title);
  }

  // Getters
  template<typename T>
  ANALYSISTREE_ATTR_NODISCARD const VectorConfig<int>::MapType& GetMap() const { return VectorConfig<T>::GetMap(); }
  template<typename T>
  ANALYSISTREE_ATTR_NODISCARD ShortInt_t GetSize() const { return VectorConfig<T>::GetSize(); }

  ANALYSISTREE_ATTR_NODISCARD std::string GetName() const { return name_; }
  ANALYSISTREE_ATTR_NODISCARD size_t GetId() const { return id_; }
  ANALYSISTREE_ATTR_NODISCARD DetType GetType() const { return type_; }
  ANALYSISTREE_ATTR_NODISCARD BranchConfig Clone(const std::string& name, DetType type) const {
    return BranchConfig(name, type, VectorConfig<int>::map_, VectorConfig<float>::map_, VectorConfig<bool>::map_);
  }

 protected:
  std::string name_;
  size_t id_{0};
  DetType type_{DetType(UndefValueShort)};

  ClassDefOverride(BranchConfig, 3);
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_BRANCHCONFIG_H
