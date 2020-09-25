//
// Created by eugene on 24/09/2020.
//

#ifndef ANALYSISTREE_INFRA_BRANCHVIEW_HPP
#define ANALYSISTREE_INFRA_BRANCHVIEW_HPP

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <Rtypes.h>
#include <TTree.h>

#include <AnalysisTree/BranchConfig.hpp>
#include <AnalysisTree/Detector.hpp>

namespace AnalysisTree {

/* fwd declatarions */
class IBranchView;
typedef std::shared_ptr<IBranchView> IBranchViewPtr;

namespace BranchViewAction {

template <typename Lambda>
class BranchViewDefineAction;

}


template<typename T>
using ResultsColumn = std::vector<T>;

template<typename T>
using ResultsMCols = std::map<std::string, ResultsColumn<T>>;

/**
 * @brief Interface class representing two-dimensional object
 * with index [channel_id, field_name + (type)]
 *
 * e.g:
 *
 * --       dca_x       dca_y
 * 0        0.          0.
 * 1        1.          1.
 * ...
 */
class IBranchView {
 public:
  /**
   * @brief Returns name of the view
   * @return
   */
  std::string GetViewName() const;
  /**
   * @brief Returns list of fields for the view
   * @return
   */
  virtual std::vector<std::string> GetFields() const = 0;
  /**
   * @brief Returns number of channels for the view
   * @return
   */
  virtual size_t GetNumberOfChannels() const = 0;

  /**
   * @brief returns matrix of evaluation results
   * @return
   */
  virtual ResultsMCols<double> GetDataMatrix() = 0;

  /**
   * @brief
   * @param entry
   */
  virtual void GetEntry(Long64_t entry) const = 0;

  virtual IBranchViewPtr Clone() const = 0;

  /**
   * @brief Select fields in a given channel range [channel_lo, channel_hi)
   * @param channel_lo
   * @param channel_hi
   * @return
   */
  IBranchViewPtr RangeChannels(size_t channel_lo, size_t channel_hi) const {
    throw std::runtime_error("Not implemented");
  };
  /**
   * @brief Returns fields of a given channel
   * --     dca_x       dca_y
   * 0      0.          0.
   * @param channel_id
   * @return
   */
  IBranchViewPtr SliceFields(size_t channel_id) const { return RangeChannels(channel_id, channel_id + 1); };

  IBranchViewPtr operator[](size_t channel_id) const { return SliceFields(channel_id); }



  /**
   * @brief Requests enlisted field_names
   * @param field_names
   * @return
   */
  IBranchViewPtr Select(const std::vector<std::string>& field_names) const;
  /**
   * @brief Selects single field
   * @param field_name
   * @return
   */
  IBranchViewPtr Select(const std::string& field_name) const;;
  IBranchViewPtr operator[](const std::string& field_name) const { return Select(field_name); }

  /**
   * @brief Merges two compatible views
   *        N (v1) == N (v2)
   * @param other
   * @return
   */
  IBranchViewPtr PlainMerge(const IBranchView& other) const {
    throw std::runtime_error("Not implemented");
  }

  template<typename Lambda>
  IBranchViewPtr Define(std::string new_field_name, std::vector<std::string> && arg_names, Lambda && function) const {
    BranchViewAction::BranchViewDefineAction<Lambda> action(new_field_name, arg_names, std::forward<Lambda>(function));
    return Apply(action);
  }

  template<typename Action>
  IBranchViewPtr Apply(Action && action) const {
    return action.ApplyAction(Clone());
  }
};

namespace Details {



template<typename T>
struct EntityTraits {
  static constexpr bool is_channel_or_track = false;
  typedef T ChannelType;

  inline static size_t GetNChannels(const T& /* entity */) {
    return 1ul;
  }

  inline static T& GetChannel(T& entity, size_t i_channel) {
    if (i_channel == 0) {
      return entity;
    }
    throw std::out_of_range("Non-channel entity cannot have > 1 channels");
  }
};

template<typename T>
struct EntityTraits<Detector<T>> {
  static constexpr bool is_channel_or_track = true;
  typedef T ChannelType;

  inline static T& GetChannel(Detector<T>& det, size_t i_channel) {
    return det.GetChannel(i_channel);
  }

  inline static size_t GetNChannels(const Detector<T> detector) {
    return detector.GetNumberOfChannels();
  }
};

}// namespace Details

template<typename Entity>
class AnalysisTreeBranch : public IBranchView {

 public:
  typedef typename Details::EntityTraits<Entity>::ChannelType ChannelType;

  IBranchViewPtr Clone() const override {
    auto newAnalysisTreeBranch = std::make_shared<AnalysisTreeBranch>(this->config_);
    newAnalysisTreeBranch->tree_ = tree_;
    newAnalysisTreeBranch->data = data;
    return newAnalysisTreeBranch;
  }

  size_t GetNumberOfChannels() const override {
    return Details::EntityTraits<Entity>::GetNChannels(*data);
  }

  std::vector<std::string> GetFields() const override {
    std::vector<std::string> result;
    GetFieldsImpl<int>(result);
    GetFieldsImpl<float>(result);
    GetFieldsImpl<bool>(result);
    return result;
  }

  void GetEntry(Long64_t entry) const override {
    tree_->GetEntry(entry);
  }

  explicit AnalysisTreeBranch(BranchConfig config) : config_(std::move(config)) {
  }

  explicit AnalysisTreeBranch(BranchConfig Config, TTree* tree) : config_(std::move(Config)) {
    tree_ = tree;
    InitTree(tree);
  }

  ResultsMCols<double> GetDataMatrix() override {
    ResultsMCols<double> result;
    for (auto &column_name : GetFields()) {
      auto emplace_result = result.emplace(column_name, ResultsColumn<double>(GetNumberOfChannels()));
      ResultsColumn<double>& column_vector = emplace_result.first->second;

      auto column_field_id = config_.GetFieldId(column_name);
      auto column_field_type = config_.GetFieldType(column_name);

      for (size_t channel_id = 0; channel_id < GetNumberOfChannels(); ++channel_id) {
        ChannelType &channel = Details::EntityTraits<Entity>::GetChannel(*data, channel_id);
        if (column_field_type == Types::kInteger) {
          column_vector[channel_id] = channel.template GetField<int>(column_field_id);
        } else if (column_field_type == Types::kFloat) {
          column_vector[channel_id] = channel.template GetField<float>(column_field_id);
        } else if (column_field_type == Types::kBool) {
          column_vector[channel_id] = channel.template GetField<bool>(column_field_id);
        }
      }
    }
    return result;
  }

 private:
  void InitTree(TTree *tree) {
    tree->SetBranchAddress(config_.GetName().c_str(), &data);
  }

  template<typename T>
  void GetFieldsImpl(std::vector<std::string>& fields) const {
    auto field_map = config_.GetMap<T>();
    std::transform(field_map.begin(), field_map.end(), std::back_inserter(fields), [](auto& elem) {
      return elem.first;
    });
  }


  BranchConfig config_;
  TTree* tree_{nullptr};
  Entity* data{new Entity};
};

namespace Details {

}

}// namespace AnalysisTree
#endif//ANALYSISTREE_INFRA_BRANCHVIEW_HPP
