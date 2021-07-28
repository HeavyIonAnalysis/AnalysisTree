/* Copyright (C) 2019-2021 GSI, Universität Tübingen, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Eugeny Kashirin, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_VARIANTMAGIC_HPP_
#define ANALYSISTREE_INFRA_VARIANTMAGIC_HPP_

// I'm really sorry about this, blame Oleg
// To be removed after c++17 will be available everywhere

#include "TTree.h"

#include "Cuts.hpp"
#include "Utils.hpp"
#include "Variable.hpp"

namespace AnalysisTree {

//struct get_value : public Utils::Visitor<double> {
//  get_value(Variable var, int i_channel_) : var_(std::move(var)), i_channel_(i_channel_) {}
//  template<class Det>
//  double apply(Det* d) const { return var_.GetValue(d->GetChannel(i_channel_)); }
//  template<typename Entity>
//  double operator()(Entity* d) const { return apply<Entity>(d); }
//  Variable var_;
//  int i_channel_;
//};

struct get_id_struct : public Utils::Visitor<size_t> {
  template<typename Entity>
  size_t get_id(Entity* d) const { return d->GetId(); }
  template<typename Entity>
  size_t operator()(Entity* d) const { return get_id<Entity>(d); }
};

struct get_channel_struct : public Utils::Visitor<ChannelPointer> {
  explicit get_channel_struct(size_t i) : i_channel_(i) {}
  template<typename Entity>
  ChannelPointer get_channel(Entity* d) const { return ChannelPointer( &(d->Channel(i_channel_)) ); }
  template<typename Entity>
  ChannelPointer operator()(Entity* d) const { return get_channel<Entity>(d); }
  size_t i_channel_;
};

struct clear_channels_struct : public Utils::Visitor<void> {
  template<typename Entity>
  void clear_channels(Entity* d) const { d->ClearChannels(); }
  template<typename Entity>
  void operator()(Entity* d) const { clear_channels<Entity>(d); }
};

struct new_channel_struct : public Utils::Visitor<void> {
  explicit new_channel_struct(BranchConfig* config) : config_(config) {}
  template<typename Entity>
  void new_channel(Entity* d) const {
    auto channel = d->AddChannel();
    channel->Init(*config_);
  }
  template<typename Entity>
  void operator()(Entity* d) const { new_channel<Entity>(d); }
  BranchConfig* config_;
};

template<typename T>
struct get_field_struct : public Utils::Visitor<double> {
  explicit get_field_struct(int id) : id_(id) {}
  template<typename Entity>
  double get_field(Entity* d) const { return d->template GetField<T>(id_); }
  template<typename Entity>
  double operator()(Entity* d) const { return get_field<Entity>(d); }
  int id_{-999};
};

template<typename T>
struct set_field_struct : public Utils::Visitor<void> {
  set_field_struct(double value, int id) : value_(value), id_(id) {}
  template<typename Entity>
  void set_field(Entity* d) const { d->template SetField<T>(value_, id_); }
  template<typename Entity>
  void   operator()(Entity* d) const { set_field<Entity>(d); }
  double value_{0.};
  int id_{-999};
};

//struct apply_cut : public Utils::Visitor<bool> {
//  apply_cut(int i_channel_, const Cuts* cut) : i_channel_(i_channel_), cut_(cut) {}
//  template<class Entity>
//  bool apply(Entity* d) const { return cut_->Apply(d->GetChannel(i_channel_)); }
//  template<typename Entity>
//  bool operator()(Entity* d) const { return apply<Entity>(d); }
//  int i_channel_;
//  const Cuts* cut_;
//};

//struct apply_cut_2_branches : public Utils::Visitor<bool> {
//  apply_cut_2_branches(int ch1, int ch2, const Cuts* cut) : ch1_(ch1), ch2_(ch2), cut_(cut) {}
//  template<class Det1, class Det2>
//  bool apply(Det1* d1, Det2* d2) const { return cut_->Apply(d1->GetChannel(ch1_), d1->GetId(), d2->GetChannel(ch2_), d2->GetId()); }
//  template<typename Entity1, typename Entity2>
//  bool operator()(Entity1* d1, Entity2* d2) const { return apply<Entity1, Entity2>(d1, d2); }
//  int ch1_;
//  int ch2_;
//  const Cuts* cut_;
//};

//struct fill_2_branches : public Utils::Visitor<double> {
//  fill_2_branches(const Variable& var, int ch1, int ch2) : var_(var), ch1_(ch1), ch2_(ch2) {}
//  template<class Det1, class Det2>
//  double apply(Det1* d1, Det2* d2) const { return var_.GetValue(d1->GetChannel(ch1_), d1->GetId(), d2->GetChannel(ch2_), d2->GetId()); }
//  template<typename Entity1, typename Entity2>
//  double operator()(Entity1* d1, Entity2* d2) const { return apply<Entity1, Entity2>(d1, d2); }
//  const Variable& var_;
//  int ch1_;
//  int ch2_;
//};

struct get_n_channels_struct : public Utils::Visitor<size_t> {
  template<class Det>
  size_t get_n_channels(Det* d) const { return d->GetNumberOfChannels(); }
  template<typename Entity>
  size_t operator()(Entity* d) const { return get_n_channels<Entity>(d); }
};

struct set_branch_address_struct : public Utils::Visitor<int> {
  set_branch_address_struct(TTree* tree, std::string name) : tree_(tree), name_(std::move(name)) {}
  template<class Det>
  int set_branch_address(Det*& d) const { return tree_->SetBranchAddress(name_.c_str(), &d); }
  template<typename Entity>
  int operator()(Entity*& d) const { return set_branch_address<Entity>(d); }
  TTree* tree_{nullptr};
  std::string name_;
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_VARIANTMAGIC_HPP_
