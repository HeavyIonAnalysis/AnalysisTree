#ifndef ANALYSISTREE_INFRA_VARIANTMAGIC_HPP_
#define ANALYSISTREE_INFRA_VARIANTMAGIC_HPP_

// I'm really sorry about this, blame Oleg
// To be removed after c++17 will be available everywhere

#include "Cuts.hpp"
#include "Utils.hpp"
#include "Variable.hpp"

namespace AnalysisTree::Version1 {

struct get_value : public Utils::Visitor<double> {
  get_value(Variable var, int i_channel) : var_(std::move(var)), i_channel_(i_channel) {}
  template<class Det>
  double apply(Det* d) const { return var_.GetValue(d->GetChannel(i_channel_)); }
  template<typename Entity>
  double operator()(Entity* d) const { return apply<Entity>(d); }
  Variable var_;
  int i_channel_;
};

struct apply_cut : public Utils::Visitor<bool> {
  apply_cut(int i_channel, const Cuts* cut) : i_channel_(i_channel), cut_(cut) {}
  template<class Entity>
  bool apply(Entity* d) const { return cut_->Apply(d->GetChannel(i_channel_)); }
  template<typename Entity>
  bool operator()(Entity* d) const { return apply<Entity>(d); }
  int i_channel_;
  const Cuts* cut_;
};

struct apply_cut_2_branches : public Utils::Visitor<bool> {
  apply_cut_2_branches(int ch1, int ch2, const Cuts* cut) : ch1_(ch1), ch2_(ch2), cut_(cut) {}
  template<class Det1, class Det2>
  bool apply(Det1* d1, Det2* d2) const { return cut_->Apply(d1->GetChannel(ch1_), d1->GetId(), d2->GetChannel(ch2_), d2->GetId()); }
  template<typename Entity1, typename Entity2>
  bool operator()(Entity1* d1, Entity2* d2) const { return apply<Entity1, Entity2>(d1, d2); }
  int ch1_;
  int ch2_;
  const Cuts* cut_;
};

struct fill_2_branches : public Utils::Visitor<double> {
  fill_2_branches(const Variable& var, int ch1, int ch2) : var_(var), ch1_(ch1), ch2_(ch2) {}
  template<class Det1, class Det2>
  double apply(Det1* d1, Det2* d2) const { return var_.GetValue(d1->GetChannel(ch1_), d1->GetId(), d2->GetChannel(ch2_), d2->GetId()); }
  template<typename Entity1, typename Entity2>
  double operator()(Entity1* d1, Entity2* d2) const { return apply<Entity1, Entity2>(d1, d2); }
  const Variable& var_;
  int ch1_;
  int ch2_;
};

struct get_n_channels_struct : public Utils::Visitor<size_t> {
  template<class Det>
  size_t get_n_channels(Det* d) const { return d->GetNumberOfChannels(); }
  template<typename Entity>
  size_t operator()(Entity* d) const { return get_n_channels<Entity>(d); }
};

struct get_id_struct : public Utils::Visitor<int> {
  template<typename Entity>
  int get_id(Entity* d) const { return d->GetId(); }
  template<typename Entity>
  int operator()(Entity* d) const { return get_id<Entity>(d); }
};

}// namespace AnalysisTree::Version1

#endif//ANALYSISTREE_INFRA_VARIANTMAGIC_HPP_
