/* Copyright (C) 2019-2021 GSI, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Eugeny Kashirin, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_BRANCH_HPP_
#define ANALYSISTREE_INFRA_BRANCH_HPP_

#include <BranchConfig.hpp>
#include <Configuration.hpp>
#include <Detector.hpp>
#include <EventHeader.hpp>

#include <cassert>
#include <map>
#include <vector>

#include "BranchChannel.hpp"

class TTree;

namespace AnalysisTree {

class Branch {
 public:

  using BranchPointer = ANALYSISTREE_UTILS_VARIANT<TrackDetector*, Particles*, ModuleDetector*, HitDetector*, EventHeader*>;

  /* c-tors */
  explicit Branch(AnalysisTree::BranchConfig config) : config_(std::move(config)) {
    InitDataPtr();
    UpdateConfigHash();
  }

  template<class T>
  Branch(AnalysisTree::BranchConfig config, T* data) : config_(std::move(config)), data_(data) {
    UpdateConfigHash();
  }

  ~Branch();

  struct FieldsMapping {
    std::vector<std::pair<Field /* src */, Field /* dst */>> field_pairs;
  };

  /* Accessors to branch' main parameters, used very often */
  inline auto GetBranchName() const { return config_.GetName(); }
  inline auto GetBranchType() const { return config_.GetType(); }
  inline const AnalysisTree::BranchConfig& GetConfig() const { return config_; }

  void InitDataPtr();
  void ConnectOutputTree(TTree* tree);

  BranchChannel NewChannel();
  void ClearChannels();
  Field NewVariable(const std::string& field_name, AnalysisTree::Types type);
  void CloneVariables(const AnalysisTree::BranchConfig& other);
  void CopyContents(Branch* br);

  //  /* iterating */
  size_t size() const;
  BranchChannel operator[](size_t i_channel) {
    return BranchChannel(this, i_channel);
  }

  size_t GetId() const {
    return std::visit([](auto&& b) { return b->GetId(); }, data_);
  }

  [[nodiscard]] BranchPointer GetData() const { return data_; }

 private:
  AnalysisTree::BranchConfig config_;
  BranchPointer data_;/// owns object
  bool is_mutable_{false};
  bool is_frozen_{false};
  std::size_t config_hash_{0};

 public:
  AnalysisTree::Configuration* parent_config{nullptr};
  bool is_connected_to_input{false};
  bool is_connected_to_output{false};

  std::map<const Branch* /* other branch */, FieldsMapping> copy_fields_mapping;

  /* Modification */
  void Freeze(bool freeze = true) { is_frozen_ = freeze; };
  void SetMutable(bool is_mutable_ = true) { Branch::is_mutable_ = is_mutable_; }
  /* Checks are used very often */
  inline void CheckFrozen(bool expected = true) const {
    if (is_frozen_ != expected)
      throw std::runtime_error("Branch is frozen");
  }
  inline void CheckMutable(bool expected = true) const {
    if (is_mutable_ != expected)
      throw std::runtime_error("Branch is not mutable");
  }

  Field GetFieldVar(const std::string& field_name);
  /**
   * @brief Gets variables according to variable names specified in the arguments.
   * Returns tuple of variables which is suitable for unpacking with std::tie()
   * @tparam Args
   * @param field_name variable names convertible to std::string
   * @return tuple of variables
   */
  template<typename... Args>
  auto GetVars(Args... field_name) {
    return GetVarsImpl(std::array<std::string, sizeof...(Args)>{{std::string(field_name)...}},
                       std::make_index_sequence<sizeof...(Args)>());
  }
  /**
   * @brief Initializes ATI2::Field objects
   * @param vars - vector of pairs with name and reference to the ATI2::Field object
   */
  void UseFields(std::vector<std::pair<std::string, std::reference_wrapper<Field>>>&& vars, bool ignore_missing = false);
  bool HasField(const std::string& field_name) const;
  std::vector<std::string> GetFieldNames() const;

//  /* Getting value */
//  inline ValueHolder Value(const Field& v) const {
//    assert(v.IsInitialized());
//    assert(v.GetParentBranch() == this);
//    if (config.GetType() == AnalysisTree::DetType::kEventHeader) {
//      return ValueHolder(v, data);
//    }
//    throw std::runtime_error("Not implemented for iterable branch");
//  }
//  inline ValueHolder operator[](const Field& v) const { return Value(v); };



  /**
   * @brief Copies contents from other branch 'as-is'. Faster than CopyContents() since it creates no mapping
   * @param other
   */
  void CopyContentsRaw(Branch* other);

  void CreateMapping(Branch* other);

  void UpdateConfigHash();

  template<typename EntityPtr>
  constexpr static const bool is_event_header_v =
      std::is_same_v<AnalysisTree::EventHeader, std::remove_const_t<std::remove_pointer_t<EntityPtr>>>;

  template<typename Functor>
  auto ApplyT(Functor&& f) {
    return std::visit(f, data_);
  }

  template<typename Functor>
  auto ApplyT(Functor&& f) const {
    return std::visit(f, data_);
  }

  AnalysisTree::ShortInt_t Hash() const {
    const auto hasher = std::hash<std::string>();
    return AnalysisTree::ShortInt_t(hasher(config_.GetName()));
  }

 private:
  template<size_t... Idx>
  auto GetVarsImpl(std::array<std::string, sizeof...(Idx)>&& field_names, std::index_sequence<Idx...>) {
    return std::make_tuple(GetFieldVar(field_names[Idx])...);
  }
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_BRANCH_HPP_
