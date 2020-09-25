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

class IBranchView;
typedef std::shared_ptr<IBranchView> IBranchViewPtr;

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
   * @brief
   * @param entry
   */
  virtual void GetEntry(Long64_t entry) const = 0;

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
   * @brief Returns array of values vs channel if
   * --     dca_x
   * 0      0.
   * 1      1.
   * ...
   * @param field_name
   * @return
   */
  IBranchViewPtr Select(const std::string& field_name) const;;
  IBranchViewPtr operator[](const std::string& field_name) const { return Select(field_name); }

  /**
   * @brief Merges two similar views
   *        N (v1) == N (v2)
   * @param other
   * @return
   */
  IBranchViewPtr PlainMerge(const IBranchView& other) const {
    throw std::runtime_error("Not implemented");
  }

  virtual IBranchViewPtr Clone() const = 0;
};

namespace Details {

class SelectFieldsAction {

 public:
  SelectFieldsAction(std::vector<std::string> fields) : selected_fields_(std::move(fields)) {}

  std::vector<std::string> GetFieldsAfterAction(const IBranchViewPtr& view) const {
    std::vector<std::string> result;
    auto view_fields = view->GetFields();
    std::set_intersection(view_fields.begin(), view_fields.end(),
                          selected_fields_.begin(), selected_fields_.end(),
                          std::back_inserter(result));
    return result;
  }

 private:
  std::vector<std::string> selected_fields_;
};

}// namespace Details

template<typename Action>
class BranchViewActionResult : public IBranchView {
 public:
  BranchViewActionResult(Action action, IBranchViewPtr action_arg) : action_(action), action_arg_(std::move(action_arg)) {}

  std::vector<std::string> GetFields() const override {
    return action_.GetFieldsAfterAction(action_arg_);
  }

  IBranchViewPtr Clone() const override {
    auto newBranchView = std::make_shared<BranchViewActionResult>(action_, action_arg_);
    return newBranchView;
  }

  void GetEntry(Long64_t i_entry) const override {
    action_arg_->GetEntry(i_entry);
  }

  /* could be lazy, could be not, const is not needed */
  size_t GetNumberOfChannels() const override {
    return 0;
  }

 private:
  Action action_;
  IBranchViewPtr action_arg_;
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

  explicit AnalysisTreeBranch(BranchConfig Config) : config_(std::move(Config)) {}

  explicit AnalysisTreeBranch(BranchConfig Config, TTree* tree) : config_(std::move(Config)) {
    tree_ = tree;
    InitTree(tree);
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
