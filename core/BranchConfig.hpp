
#ifndef ANALYSISTREE_BRANCHCONFIG_H
#define ANALYSISTREE_BRANCHCONFIG_H

#include <utility>
#include <vector>
#include <map>
#include <string>
#include <iostream>

#include <TObject.h>

#include "Constants.hpp"

namespace AnalysisTree {

template<typename T>
class VectorConfig {
 public:
  VectorConfig() = default;
  VectorConfig(const VectorConfig&) = default;
  VectorConfig(VectorConfig&&) = default;
  VectorConfig& operator=(VectorConfig&&) = default;
  VectorConfig& operator= (const VectorConfig&) = default;
  virtual ~VectorConfig() = default;

  virtual size_t GetSize() const { return map_.size(); }
  virtual void AddField(const std::string& name) { map_.insert (std::pair<std::string, ShortInt_t>(name, size_++) ); }
  void AddField(const std::string& name, ShortInt_t id) { map_.insert ( std::pair<std::string,ShortInt_t>(name, id) ); }

  virtual void AddFields(const std::vector<std::string>& names) {
    for (const auto& name : names){
      map_.insert ( std::pair<std::string, ShortInt_t>(name, size_++) );
    }
  }

  ShortInt_t GetId(const std::string& sField) const {
    auto search = map_.find(sField);
    if (search != map_.end()){
      return search->second;
    } else {
//         std::cout << "BranchConfig::GetFloatingId - Not found\n";
      return UndefValueShort;
    }
  }

  virtual const std::map <std::string, ShortInt_t>& GetMap() const { return map_; }

 protected:
  std::map <std::string, ShortInt_t> map_{};
  ShortInt_t size_{0};
};

class BranchConfig : public VectorConfig<int>, public VectorConfig<float>, public VectorConfig<bool> {

 public:
  
  BranchConfig() = default;
  BranchConfig(const BranchConfig &) = default;
  BranchConfig(BranchConfig &&) = default;
  BranchConfig& operator=(BranchConfig&&) = default;
  BranchConfig& operator= (const BranchConfig &) = default;
  ~BranchConfig() override = default;

  explicit BranchConfig(std::string name, DetType type) : name_(std::move(name)), type_(type) {

    if (type_ == DetType::kTrack)
    {
      VectorConfig<float>::AddField("px",  TrackFields::kPx);
      VectorConfig<float>::AddField("py",  TrackFields::kPy);
      VectorConfig<float>::AddField("pz",  TrackFields::kPz);
      VectorConfig<float>::AddField("pT",  TrackFields::kPt);
      VectorConfig<float>::AddField("phi",  TrackFields::kPhi);
      VectorConfig<float>::AddField("eta",  TrackFields::kEta);
      VectorConfig<float>::AddField("p",  TrackFields::kP);
    }
    if (type_ == DetType::kParticle)
    {
      VectorConfig<float>::AddField("px",  ParticleFields::kPx);
      VectorConfig<float>::AddField("py",  ParticleFields::kPy);
      VectorConfig<float>::AddField("pz",  ParticleFields::kPz);
      VectorConfig<float>::AddField("pT",  ParticleFields::kPt);
      VectorConfig<float>::AddField("phi",  ParticleFields::kPhi);
      VectorConfig<float>::AddField("eta",  ParticleFields::kEta);
      VectorConfig<float>::AddField("mass",  ParticleFields::kMass);
      VectorConfig<float>::AddField("p",  ParticleFields::kP);
      VectorConfig<float>::AddField("rapidity",  ParticleFields::kRapidity);
      VectorConfig<int>::AddField("pid",  ParticleFields::kPid);
    }
    if (type_ == DetType::kHit)
    {
      VectorConfig<float>::AddField("x",  HitFields::kX);
      VectorConfig<float>::AddField("y",  HitFields::kY);
      VectorConfig<float>::AddField("z",  HitFields::kZ);
      VectorConfig<float>::AddField("phi",  HitFields::kPhi);
      VectorConfig<float>::AddField("signal",  HitFields::kSignal);
    }
    if (type_ == DetType::kModule)
    {
      VectorConfig<int>::AddField("id",  ModuleFields::kId);
      VectorConfig<float>::AddField("signal",  ModuleFields::kSignal);
    }
    if (type_ == DetType::kEventHeader)
    {
      VectorConfig<float>::AddField("vtx_x",  EventHeaderFields::kVertexX);
      VectorConfig<float>::AddField("vtx_y",  EventHeaderFields::kVertexY);
      VectorConfig<float>::AddField("vtx_z",  EventHeaderFields::kVertexZ);
    }
  };

  DetType GetType() const { return type_; }

  template<typename T>
  void AddField(const std::string& name) { VectorConfig<T>::AddField(name); }
  template<typename T>
  void AddFields(const std::vector<std::string>& names) { VectorConfig<T>::AddFields(names); }

  template<typename T>
  const std::map <std::string, ShortInt_t>& GetMap() const { return VectorConfig<T>::GetMap(); }

  Types GetFieldType(const std::string& sField) const {
    ShortInt_t id = VectorConfig<int>::GetId(sField);
    if (id != UndefValueShort) return Types::kInteger;

    id = VectorConfig<float>::GetId(sField);
    if (id != UndefValueShort) return Types::kFloat;

    id = VectorConfig<bool>::GetId(sField);
    if (id != UndefValueShort) return Types::kBool;

    return (Types)UndefValueShort;
  }

  template<typename T>
  ShortInt_t GetSize() const { return VectorConfig<T>::GetSize(); }

  ShortInt_t GetFieldId(const std::string& sField) const {
    ShortInt_t id = VectorConfig<int>::GetId(sField);
    if (id != UndefValueShort) return id;

    id = VectorConfig<float>::GetId(sField);
    if (id != UndefValueShort) return id;

    id = VectorConfig<bool>::GetId(sField);
    if (id != UndefValueShort) return id;

    return UndefValueShort;
  }
  
  void SetId(ShortInt_t id) { id_ = id; }
  ShortInt_t GetId() const { return id_; }

  std::string GetName() const { return name_; }
  
  void Print() const 
  {
    std::cout << "Branch " << name_ << " (id=" << id_ << ") consists of:" << std::endl;

    std::cout << "  floating fields:" << std::endl;
    for (const auto& entry : VectorConfig<float>::GetMap())
      std::cout << "   " << entry.first << " (id=" << entry.second << ")" << std::endl;

    std::cout << "  integer fields:" << std::endl;
    for (const auto& entry : VectorConfig<int>::GetMap())
      std::cout << "   " << entry.first << " (id=" << entry.second << ")" << std::endl;

    std::cout << "  boolean fields:" << std::endl;
    for (const auto& entry : VectorConfig<bool>::GetMap())
      std::cout << "   " << entry.first << " (id=" << entry.second << ")" << std::endl;
  }
  
 protected:

  std::string name_{""};
  ShortInt_t id_{UndefValueShort};
  DetType type_{DetType(UndefValueShort)};
    
  ClassDef(AnalysisTree::BranchConfig, 1)

  
};


} // AnalysisTree
#endif //ANALYSISTREE_BRANCHCONFIG_H
