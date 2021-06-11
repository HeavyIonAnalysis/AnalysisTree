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

struct BranchChannelsIter {
  BranchChannelsIter(Branch* branch, size_t i_channel) : branch(branch), i_channel(i_channel) {
    current_channel.reset(new BranchChannel(branch, i_channel));
  }

  bool operator==(const BranchChannelsIter& rhs) const {
    return i_channel == rhs.i_channel && branch == rhs.branch;
  }
  bool operator!=(const BranchChannelsIter& rhs) const {
    return !(rhs == *this);
  }
  BranchChannel operator*() const {
    return current_channel.operator*();
  }
  BranchChannel& operator*() {
    return current_channel.operator*();
  }
  BranchChannelsIter& operator++();

  std::unique_ptr<BranchChannel> current_channel;
  Branch* branch;
  size_t i_channel;
};

struct Branch {
  struct BranchChannelsLoop {
    explicit BranchChannelsLoop(Branch* branch) : branch(branch) {}
    Branch* branch{nullptr};

    inline BranchChannelsIter begin() const { return branch->ChannelsBegin(); };
    inline BranchChannelsIter end() const { return branch->ChannelsEnd(); };
  };

  struct FieldsMapping {
    std::vector<std::pair<Field /* src */, Field /* dst */>> field_pairs;
  };

  ~Branch();

 private:
  AnalysisTree::BranchConfig config;
  void* data{nullptr};/// owns object
  bool is_mutable{false};
  bool is_frozen{false};
  std::size_t config_hash{0};

 public:
  AnalysisTree::Configuration* parent_config{nullptr};
  bool is_connected_to_input{false};
  bool is_connected_to_output{false};

  std::map<const Branch* /* other branch */, FieldsMapping> copy_fields_mapping;

  /* c-tors */
  explicit Branch(AnalysisTree::BranchConfig config) : config(std::move(config)) {
    InitDataPtr();
    UpdateConfigHash();
  }
  Branch(AnalysisTree::BranchConfig config, void* data) : config(std::move(config)), data(data) {
    UpdateConfigHash();
  }

  /* Accessors to branch' main parameters, used very often */
  inline auto GetBranchName() const { return config.GetName(); }
  inline auto GetBranchType() const { return config.GetType(); }
  inline const AnalysisTree::BranchConfig& GetConfig() const { return config; }

  void InitDataPtr();
  void ConnectOutputTree(TTree* tree);

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

  /* Getting value */
  inline ValueHolder Value(const Field& v) const {
    assert(v.IsInitialized());
    assert(v.GetParentBranch() == this);
    if (config.GetType() == AnalysisTree::DetType::kEventHeader) {
      return ValueHolder(v, data);
    }
    throw std::runtime_error("Not implemented for iterable branch");
  }

  inline ValueHolder operator[](const Field& v) const { return Value(v); };

  /* iterating */
  size_t size() const;
  BranchChannel operator[](size_t i_channel);
  inline BranchChannelsLoop Loop() { return BranchChannelsLoop(this); };
  inline BranchChannelsIter ChannelsBegin() { return BranchChannelsIter(this, 0); };
  inline BranchChannelsIter ChannelsEnd() { return BranchChannelsIter(this, size()); };

  /* Modification */
  void Freeze(bool freeze = true) { is_frozen = freeze; };
  void SetMutable(bool is_mutable = true) { Branch::is_mutable = is_mutable; }
  /* Checks are used very often */
  inline void CheckFrozen(bool expected = true) const {
    if (is_frozen != expected)
      throw std::runtime_error("Branch is frozen");
  }
  inline void CheckMutable(bool expected = true) const {
    if (is_mutable != expected)
      throw std::runtime_error("Branch is not mutable");
  }
  BranchChannel NewChannel();
  void ClearChannels();
  Field NewVariable(const std::string& field_name, AnalysisTree::Types type);
  void CloneVariables(const AnalysisTree::BranchConfig& other);
  void CopyContents(Branch* br);

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
    using AnalysisTree::DetType;

    if (config.GetType() == DetType::kParticle) {
      return f((AnalysisTree::Particles*) data);
    } else if (config.GetType() == DetType::kTrack) {
      return f((AnalysisTree::TrackDetector*) data);
    } else if (config.GetType() == DetType::kModule) {
      return f((AnalysisTree::ModuleDetector*) data);
    } else if (config.GetType() == DetType::kHit) {
      return f((AnalysisTree::HitDetector*) data);
    } else if (config.GetType() == DetType::kEventHeader) {
      return f((AnalysisTree::EventHeader*) data);
    }
    /* unreachable */
    __builtin_unreachable();
    assert(false);
  }

  template<typename Functor>
  auto ApplyT(Functor&& f) const {
    using AnalysisTree::DetType;

    if (config.GetType() == DetType::kParticle) {
      return f((const AnalysisTree::Particles*) data);
    } else if (config.GetType() == DetType::kTrack) {
      return f((const AnalysisTree::TrackDetector*) data);
    } else if (config.GetType() == DetType::kModule) {
      return f((const AnalysisTree::ModuleDetector*) data);
    } else if (config.GetType() == DetType::kHit) {
      return f((const AnalysisTree::HitDetector*) data);
    } else if (config.GetType() == DetType::kEventHeader) {
      return f((const AnalysisTree::EventHeader*) data);
    }
    /* unreachable */
    __builtin_unreachable();
    assert(false);
  }

  AnalysisTree::ShortInt_t Hash() const {
    const auto hasher = std::hash<std::string>();
    return AnalysisTree::ShortInt_t(hasher(config.GetName()));
  }

 private:
  template<size_t... Idx>
  auto GetVarsImpl(std::array<std::string, sizeof...(Idx)>&& field_names, std::index_sequence<Idx...>) {
    return std::make_tuple(GetFieldVar(field_names[Idx])...);
  }
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_BRANCH_HPP_
