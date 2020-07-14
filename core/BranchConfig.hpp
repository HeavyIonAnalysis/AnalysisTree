
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
  VectorConfig() = default;
  VectorConfig(const VectorConfig &) = default;
  VectorConfig(VectorConfig &&) = default;
  VectorConfig &operator=(VectorConfig &&) = default;
  VectorConfig &operator=(const VectorConfig &) = default;
  virtual ~VectorConfig() = default;

  virtual size_t GetSize() const { return map_.size(); }
  virtual void AddField(const std::string &name) { map_.insert(std::pair<std::string, ShortInt_t>(name, size_++)); }
  void AddField(const std::string &name, ShortInt_t id) { map_.insert(std::pair<std::string, ShortInt_t>(name, id)); }

  virtual void AddFields(const std::vector<std::string> &names) {
    for (const auto &name : names) {
      map_.insert(std::pair<std::string, ShortInt_t>(name, size_++));
    }
  }

  ShortInt_t GetId(const std::string &sField) const {
    auto search = map_.find(sField);
    if (search != map_.end()) {
      return search->second;
    } else {
      //         std::cout << "BranchConfig::GetFloatingId - Not found\n";
      return UndefValueShort;
    }
  }

  virtual const std::map<std::string, ShortInt_t> &GetMap() const { return map_; }

 protected:
  std::map<std::string, ShortInt_t> map_{};
  ShortInt_t size_{0};
};

class BranchConfig : public VectorConfig<int>, public VectorConfig<float>, public VectorConfig<bool> {

 public:
  BranchConfig() = default;
  BranchConfig(const BranchConfig &) = default;
  BranchConfig(BranchConfig &&) = default;
  BranchConfig &operator=(BranchConfig &&) = default;
  BranchConfig &operator=(const BranchConfig &) = default;
  ~BranchConfig() override = default;

  explicit BranchConfig(std::string name, DetType type);;

  DetType GetType() const { return type_; }

  template<typename T>
  void AddField(const std::string &name) { VectorConfig<T>::AddField(name); }
  template<typename T>
  void AddFields(const std::vector<std::string> &names) { VectorConfig<T>::AddFields(names); }

  template<typename T>
  const std::map<std::string, ShortInt_t> &GetMap() const { return VectorConfig<T>::GetMap(); }

  Types GetFieldType(const std::string &sField) const;

  template<typename T>
  ShortInt_t GetSize() const { return VectorConfig<T>::GetSize(); }

  ShortInt_t GetFieldId(const std::string &sField) const;

  void SetId(ShortInt_t id) { id_ = id; }
  ShortInt_t GetId() const { return id_; }
  std::string GetName() const { return name_; }

  void Print() const;

 protected:
  std::string name_{""};
  ShortInt_t id_{UndefValueShort};
  DetType type_{DetType(UndefValueShort)};

  ClassDef(AnalysisTree::BranchConfig, 1)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_BRANCHCONFIG_H
