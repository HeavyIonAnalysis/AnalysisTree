/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */

#ifndef ANALYSISTREE_BRANCHCONFIG_H
#define ANALYSISTREE_BRANCHCONFIG_H

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <TObject.h>

#include "Constants.hpp"

namespace AnalysisTree {

/// Information to store about a data field in Configuration
struct ConfigElement {
  ConfigElement() = default;
  virtual ~ConfigElement() = default;

  explicit ConfigElement(ShortInt_t id, std::string title) : id_(id), title_(std::move(title)) {}
  ShortInt_t id_{0};
  std::string title_;

 protected:
  ClassDef(ConfigElement, 1)
};

typedef std::map<std::string, ConfigElement> MapType;

/// Template class to store configuration, e. g. name and description of the vector element
template<typename T>
class VectorConfig {
 public:
  VectorConfig() = default;
  VectorConfig(const VectorConfig&) = default;
  explicit VectorConfig(const MapType& map) : map_(map), size_(map.size()) {}
  VectorConfig(VectorConfig&&) noexcept(std::is_nothrow_move_constructible<MapType>::value) = default;
  VectorConfig& operator=(VectorConfig&&) noexcept(std::is_nothrow_move_assignable<MapType>::value) = default;
  VectorConfig& operator=(const VectorConfig&) = default;
  virtual ~VectorConfig() = default;

  virtual void AddField(const std::string& name, const std::string& title) {
    map_.insert(std::make_pair(name, ConfigElement(size_++, title)));
  }
  virtual void AddField(const std::string& name, ShortInt_t id, const std::string& title) {
    map_.insert(std::make_pair(name, ConfigElement(id, title)));
  }
  void AddField(const std::string& name, const ConfigElement& field) {
    map_.insert(std::make_pair(name, field));
  }
  virtual void AddFields(const std::vector<std::string>& names, const std::string& title) {
    for (const auto& name : names) {
      map_.insert(std::make_pair(name, ConfigElement(size_++, title)));
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

/// A class to store configuration of the Container.
/**
 * Maybe better design choise would be use composition over inheritance (?)
 */
class BranchConfig : public VectorConfig<int>, public VectorConfig<float>, public VectorConfig<bool> {

 public:
  BranchConfig() = default;
  BranchConfig(const BranchConfig&) = default;
  BranchConfig(BranchConfig&&) = default;
  BranchConfig& operator=(BranchConfig&&) = default;
  BranchConfig& operator=(const BranchConfig&) = default;
  ~BranchConfig() override = default;

  BranchConfig(std::string name, DetType type);

  void Print() const override;

  ANALYSISTREE_ATTR_NODISCARD Types GetFieldType(const std::string& sField) const;
  ANALYSISTREE_ATTR_NODISCARD ShortInt_t GetFieldId(const std::string& sField) const;

  // Setters
  template<typename T>
  void AddField(const std::string& name, const std::string& title = "") {
    VectorConfig<T>::AddField(name, title);
  }
  template<typename T>
  void AddFields(const std::vector<std::string>& names, const std::string& title = "") {
    VectorConfig<T>::AddFields(names, title);
  }
  template<typename T>
  void AddField(const std::string& name, ShortInt_t id, const std::string& title = "") {
    VectorConfig<T>::AddField(name, id, title);
  }

  // Getters
  template<typename T>
  ANALYSISTREE_ATTR_NODISCARD const MapType& GetMap() const { return VectorConfig<T>::GetMap(); }
  template<typename T>
  ANALYSISTREE_ATTR_NODISCARD ShortInt_t GetSize() const { return VectorConfig<T>::GetSize(); }

  ANALYSISTREE_ATTR_NODISCARD std::string GetName() const { return name_; }
  template<typename T>
  ANALYSISTREE_ATTR_NODISCARD std::vector<std::string> GetFieldsNamesT() const {
    std::vector<std::string> result;
    std::transform(begin(GetMap<T>()), end(GetMap<T>()), back_inserter(result),
                   [](const typename MapType::value_type& elem) { return elem.first; });
    return result;
  }
  ANALYSISTREE_ATTR_NODISCARD size_t GetId() const { return id_; }
  ANALYSISTREE_ATTR_NODISCARD DetType GetType() const { return type_; }

  /**
 * @brief Creates a copy with different name and/or detector type
 * @param name new name
 * @param type new type
 */
  ANALYSISTREE_ATTR_NODISCARD BranchConfig Clone(const std::string& name, DetType type) const;

  bool HasField(const std::string& field) const { return GetFieldId(field) != UndefValueShort; }

 protected:
  void GenerateId();

  std::string name_;
  size_t id_{0};
  DetType type_{DetType(UndefValueShort)};

  ClassDefOverride(BranchConfig, 3);
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_BRANCHCONFIG_H
