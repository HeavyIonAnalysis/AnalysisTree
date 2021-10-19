
#ifndef ANALYSISTREE_BRANCHCONFIG_H
#define ANALYSISTREE_BRANCHCONFIG_H

#include <map>
#include <string>
#include <vector>
#include <algorithm>

#include <TObject.h>

#include "Constants.hpp"

namespace AnalysisTree {

template<typename T>
class VectorConfig {
 public:
  typedef std::map<std::string,ShortInt_t> MapType;

  VectorConfig() = default;
  VectorConfig(const VectorConfig&) = default;
  VectorConfig(VectorConfig&&) noexcept(std::is_nothrow_move_constructible<MapType>::value) = default;
  VectorConfig& operator=(VectorConfig&&) noexcept(std::is_nothrow_move_assignable<MapType>::value) = default;
  VectorConfig& operator=(const VectorConfig&) = default;
  virtual ~VectorConfig() = default;

  virtual void AddField(const std::string& name) { map_.insert(std::make_pair(name, size_++)); }
  void AddField(const std::string& name, ShortInt_t id) { map_.insert(std::make_pair(name, id)); }

  virtual void AddFields(const std::vector<std::string>& names) {
    for (const auto& name : names) {
      map_.insert(std::pair<std::string, ShortInt_t>(name, size_++));
    }
  }

  [[nodiscard]] ShortInt_t GetId(const std::string& sField) const {
    auto search = map_.find(sField);
    if (search != map_.end()) {
      return search->second;
    } else {
      return UndefValueShort;
    }
  }

  [[nodiscard]] virtual const std::map<std::string, ShortInt_t>& GetMap() const { return map_; }
  [[nodiscard]] virtual ShortInt_t GetSize() const { return size_; }

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

  [[nodiscard]] Types GetFieldType(const std::string& sField) const;
  [[nodiscard]] ShortInt_t GetFieldId(const std::string& sField) const;

  // Setters
  void SetId(ShortInt_t id) { id_ = id; }
  template<typename T>
  void AddField(const std::string& name) { VectorConfig<T>::AddField(name); }
  template<typename T>
  void AddFields(const std::vector<std::string>& names) { VectorConfig<T>::AddFields(names); }

  // Getters
  template<typename T>
  [[nodiscard]] const std::map<std::string, ShortInt_t>& GetMap() const { return VectorConfig<T>::GetMap(); }
  template<typename T>
  [[nodiscard]] ShortInt_t GetSize() const { return VectorConfig<T>::GetSize(); }

  [[nodiscard]] std::string GetName() const { return name_; }
  [[nodiscard]] ShortInt_t GetId() const { return id_; }
  [[nodiscard]] DetType GetType() const { return type_; }
  template<typename T>
  [[nodiscard]] std::vector<std::string> GetFieldsNamesT() const {
    std::vector<std::string> result;
    std::transform(begin(GetMap<T>()), end(GetMap<T>()), back_inserter(result),
                   [] (const typename VectorConfig<T>::MapType::value_type &elem) { return elem.first; });
    return result;
  }


 protected:
  std::string name_;
  ShortInt_t id_{UndefValueShort};
  DetType type_{DetType(UndefValueShort)};

  ClassDef(AnalysisTree::BranchConfig, 1)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_BRANCHCONFIG_H
