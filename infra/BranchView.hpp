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

class IAction {

 public:
  virtual ~IAction() = default;
  virtual IBranchViewPtr ApplyAction(const IBranchViewPtr& tgt) = 0;
};

class IFieldRef {

 public:
  virtual ~IFieldRef() = default;
  virtual double GetValue(size_t i_channel) const = 0;
};

typedef std::shared_ptr<IFieldRef> IFieldPtr;

namespace BranchViewAction {

template<typename Lambda>
IAction* NewDefineAction(const std::string& field_name, const std::vector<std::string>& lambda_args, Lambda&& lambda);

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

  virtual IFieldPtr GetFieldPtr(std::string field_name) const = 0;

  /**
   * @brief returns matrix of evaluation results
   * @return
   */
  virtual ResultsMCols<double> GetDataMatrix() {
    ResultsMCols<double> result;
    for (auto& column_name : GetFields()) {
      auto emplace_result = result.emplace(column_name, ResultsColumn<double>(GetNumberOfChannels()));
      ResultsColumn<double>& column_vector = emplace_result.first->second;
      IFieldPtr field_ptr = GetFieldPtr(column_name);
      for (size_t i_channel = 0; i_channel < GetNumberOfChannels(); ++i_channel) {
        column_vector[i_channel] = field_ptr->GetValue(i_channel);
      }
    }
    return result;
  }

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
  IBranchViewPtr Select(const std::string& field_name) const;
  ;
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
  IBranchViewPtr Define(std::string new_field_name, std::vector<std::string>&& arg_names, Lambda&& function) const {
    auto function_wrap = std::function(function);
    auto action_ptr = BranchViewAction::NewDefineAction(new_field_name, arg_names, std::move(function_wrap));
    return Apply(action_ptr);
  }

  template<typename Action>
  IBranchViewPtr Apply(Action&& action) const {
    return action.ApplyAction(Clone());
  }

  IBranchViewPtr Apply(IAction* action) const {
    auto apply_result = action->ApplyAction(Clone());
    delete action;
    return apply_result;
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

  class FieldRefImpl : public IFieldRef {
   public:
    FieldRefImpl(Entity* Data, ShortInt_t FieldId, Types FieldType) : data_(Data), field_id_(FieldId), field_type_(FieldType) {}
    double GetValue(size_t i_channel) const override {
      auto& channel = Details::EntityTraits<Entity>::GetChannel(*data_, i_channel);
      if (field_type_ == Types::kInteger) {
        return channel.template GetField<int>(field_id_);
      } else if (field_type_ == Types::kFloat) {
        return channel.template GetField<float>(field_id_);
      } else if (field_type_ == Types::kBool) {
        return channel.template GetField<bool>(field_id_);
      }
      assert(false);
    }

   private:
    Entity* data_{nullptr};
    ShortInt_t field_id_{};
    Types field_type_{Types::kNumberOfTypes};
  };

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

  IFieldPtr GetFieldPtr(std::string field_name) const override {
    auto column_field_id = config_.GetFieldId(field_name);
    auto column_field_type = config_.GetFieldType(field_name);
    return std::make_shared<FieldRefImpl>(data, column_field_id, column_field_type);
  }

 private:
  void InitTree(TTree* tree) {
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

namespace BranchViewAction {

namespace Details {
template<typename T>
struct Arity : Arity<decltype(&T::operator())> {};
template<typename R, typename... Args>
struct Arity<R (*)(Args...)> : std::integral_constant<unsigned, sizeof...(Args)> {};
template<typename R, typename C, typename... Args>
struct Arity<R (C::*)(Args...)> : std::integral_constant<unsigned, sizeof...(Args)> {};
template<typename R, typename C, typename... Args>
struct Arity<R (C::*)(Args...) const> : std::integral_constant<unsigned, sizeof...(Args)> {};
}// namespace Details

template<typename Function>
class BranchViewDefineAction : public IAction {

  class DefineActionResultImpl : public IBranchView {

    static constexpr size_t function_arity = Details::Arity<Function>::value;

    class FieldRefImpl : public IFieldRef {

     public:
      FieldRefImpl(Function&& lambda, std::vector<IFieldPtr> lambda_args) : lambda_(lambda), lambda_args_(std::move(lambda_args)) {}

      double GetValue(size_t i_channel) const final {
        return GetValueImpl(i_channel, std::make_index_sequence<function_arity>());
      }

     private:
      template<size_t ArgIndex>
      double GetArgValue(size_t i_channel) const {
        return lambda_args_[ArgIndex]->GetValue(i_channel);
      }

      template<size_t... ArgIndex>
      double
      GetValueImpl(size_t i_channel, std::index_sequence<ArgIndex...>) const {
        return lambda_(GetArgValue<ArgIndex>(i_channel)...);
      }

      Function lambda_;
      std::vector<IFieldPtr> lambda_args_;
    };

   public:
    DefineActionResultImpl(std::string defined_field_name,
                           std::vector<std::string> lambda_args,
                           Function lambda,
                           IBranchViewPtr origin) : defined_field_name_(std::move(defined_field_name)),
                                                    lambda_args_(std::move(lambda_args)),
                                                    lambda_(lambda),
                                                    origin_(std::move(origin)) {
      if (function_arity != lambda_args_.size()) {
        throw std::out_of_range("Function arity is now consistent with number of passed arguments");
      }
      std::vector<IFieldPtr> lambda_args_ptrs;
      lambda_args_ptrs.reserve(lambda_args_.size());
      for (auto& arg_field_name : lambda_args_) {
        lambda_args_ptrs.emplace_back(origin_->GetFieldPtr(arg_field_name));
      }
      defined_field_ptr_ = std::make_shared<FieldRefImpl>(std::forward<Function>(lambda_), lambda_args_ptrs);
    }

    std::vector<std::string> GetFields() const override {
      std::vector<std::string> fields({defined_field_name_});
      auto origin_fields = origin_->GetFields();
      std::copy(origin_fields.begin(), origin_fields.end(), std::back_inserter(fields));
      return fields;
    }
    size_t GetNumberOfChannels() const override {
      return origin_->GetNumberOfChannels();
    }
    void GetEntry(Long64_t entry) const override {
      origin_->GetEntry(entry);
    }
    IBranchViewPtr Clone() const override {
      return std::make_shared<DefineActionResultImpl>(defined_field_name_, lambda_args_, lambda_, origin_->Clone());
    }
    IFieldPtr GetFieldPtr(std::string field_name) const override {
      if (field_name == defined_field_name_) {
        return defined_field_ptr_;
      }
      return origin_->GetFieldPtr(field_name);
    }

    std::string defined_field_name_;
    std::vector<std::string> lambda_args_;
    Function lambda_;
    IBranchViewPtr origin_;
    IFieldPtr defined_field_ptr_;
  };

 public:
  BranchViewDefineAction(std::string field_name, std::vector<std::string> lambda_args, Function&& lambda) : defined_field_name_(std::move(field_name)), lambda_args_(std::move(lambda_args)), lambda_(lambda) {
  }
  IBranchViewPtr ApplyAction(const IBranchViewPtr& origin) override {
    /* check if all fields exist in the origin */
    auto origin_fields = origin->GetFields();
    std::vector<std::string> field_intersection;
    std::sort(origin_fields.begin(), origin_fields.end());
    std::sort(lambda_args_.begin(), lambda_args_.end());
    std::set_intersection(origin_fields.begin(), origin_fields.end(),
                          lambda_args_.begin(), lambda_args_.end(),
                          std::back_inserter(field_intersection));
    if (field_intersection.size() != lambda_args_.size()) {
      throw std::out_of_range("Requested field is missing in the input view");
    }
    /* check if defined field is absent in the input */
    if (std::find(origin_fields.begin(), origin_fields.end(), defined_field_name_) != origin_fields.end()) {
      throw std::runtime_error("New variable already exists in the input view");
    }

    auto result = std::make_shared<DefineActionResultImpl>(defined_field_name_, lambda_args_, lambda_, origin);
    return result;
  }

 private:
  std::string defined_field_name_;
  std::vector<std::string> lambda_args_;
  Function lambda_;
};

template<typename Lambda>
IAction* NewDefineAction(const std::string& field_name, const std::vector<std::string>& lambda_args, Lambda&& lambda) {
  return new BranchViewDefineAction(field_name, lambda_args, std::function(std::forward<Lambda>(lambda)));
}

template<typename Lambda>
IAction* NewDefineAction(const std::string& field_name, std::initializer_list<std::string> lambda_args, Lambda&& lambda) {
  std::vector<std::string> lambda_args_v(lambda_args.begin(), lambda_args.end());
  return NewDefineAction(field_name, lambda_args_v, lambda);
}

}// namespace BranchViewAction

}// namespace AnalysisTree
#endif//ANALYSISTREE_INFRA_BRANCHVIEW_HPP
