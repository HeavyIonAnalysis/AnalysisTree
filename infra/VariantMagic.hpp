#ifndef ANALYSISTREE_INFRA_VARIANTMAGIC_HPP_
#define ANALYSISTREE_INFRA_VARIANTMAGIC_HPP_

// I'm really sorry about this, blame Oleg
// To be removed after c++17 will be available everywhere

#ifdef USEBOOST
#include <TTree.h>
#include <utility>

#include "Cuts.hpp"
#include "Detector.hpp"
#include "EventHeader.hpp"
#include "boost/variant.hpp"

#pragma message("Compiling with Boost")
#define get_func boost::get
using namespace AnalysisTree;


struct set_branch_address_struct : public boost::static_visitor<int> {
  set_branch_address_struct(TTree* tree, std::string name) : tree_(tree), name_(std::move(name)) {}
  template<class Det>
  int set_branch_address(Det* d) const { return tree_->SetBranchAddress(name_.c_str(), &d); }
  int operator()(TrackDetector* d) const { return set_branch_address<TrackDetector>(d); }
  int operator()(HitDetector* d) const { return set_branch_address<HitDetector>(d); }
  int operator()(ModuleDetector* d) const { return set_branch_address<ModuleDetector>(d); }
  int operator()(Particles* d) const { return set_branch_address<Particles>(d); }
  int operator()(EventHeader* d) const { return set_branch_address<EventHeader>(d); }
  TTree* tree_{nullptr};
  std::string name_;
};

struct get_value : public boost::static_visitor<double> {
  get_value(Variable var, int i_channel) : var_(std::move(var)), i_channel_(i_channel) {}
  template<class Det>
  double apply(Det* d) const { return var_.GetValue(d->GetChannel(i_channel_)); }
  double operator()(TrackDetector* d) const { return apply<TrackDetector>(d); }
  double operator()(HitDetector* d) const { return apply<HitDetector>(d); }
  double operator()(ModuleDetector* d) const { return apply<ModuleDetector>(d); }
  double operator()(Particles* d) const { return apply<Particles>(d); }
  double operator()(EventHeader* d) const { return apply<EventHeader>(d); }
  Variable var_;
  int i_channel_;
};

struct apply_cut : public boost::static_visitor<bool> {
  apply_cut(int i_channel, const Cuts* cut) : i_channel_(i_channel), cut_(cut) {}
  template<class Det>
  bool apply(Det* d) const { return cut_->Apply(d->GetChannel(i_channel_)); }
  bool operator()(TrackDetector* d) const { return apply<TrackDetector>(d); }
  bool operator()(HitDetector* d) const { return apply<HitDetector>(d); }
  bool operator()(ModuleDetector* d) const { return apply<ModuleDetector>(d); }
  bool operator()(Particles* d) const { return apply<Particles>(d); }
  bool operator()(EventHeader* d) const { return apply<EventHeader>(d); }
  int i_channel_;
  const Cuts* cut_;
};

struct apply_cut_2_branches : public boost::static_visitor<bool> {
  apply_cut_2_branches(int ch1, int ch2, const Cuts* cut) : ch1_(ch1), ch2_(ch2), cut_(cut) {}
  template<class Det1, class Det2>
  bool apply(Det1* d1, Det2* d2) const { return cut_->Apply(d1->GetChannel(ch1_), d1->GetId(), d2->GetChannel(ch2_), d2->GetId()); }
  bool operator()(TrackDetector* d1, TrackDetector* d2) const { return apply<TrackDetector, TrackDetector>(d1, d2); }
  bool operator()(TrackDetector* d1, Particles* d2) const { return apply<TrackDetector, Particles>(d1, d2); }
  bool operator()(TrackDetector* d1, EventHeader* d2) const { return apply<TrackDetector, EventHeader>(d1, d2); }
  bool operator()(TrackDetector* d1, HitDetector* d2) const { return apply<TrackDetector, HitDetector>(d1, d2); }
  bool operator()(TrackDetector* d1, ModuleDetector* d2) const { return apply<TrackDetector, ModuleDetector>(d1, d2); }
  bool operator()(Particles* d1, TrackDetector* d2) const { return apply<Particles, TrackDetector>(d1, d2); }
  bool operator()(Particles* d1, Particles* d2) const { return apply<Particles, Particles>(d1, d2); }
  bool operator()(Particles* d1, EventHeader* d2) const { return apply<Particles, EventHeader>(d1, d2); }
  bool operator()(Particles* d1, HitDetector* d2) const { return apply<Particles, HitDetector>(d1, d2); }
  bool operator()(Particles* d1, ModuleDetector* d2) const { return apply<Particles, ModuleDetector>(d1, d2); }
  bool operator()(EventHeader* d1, TrackDetector* d2) const { return apply<EventHeader, TrackDetector>(d1, d2); }
  bool operator()(EventHeader* d1, Particles* d2) const { return apply<EventHeader, Particles>(d1, d2); }
  bool operator()(EventHeader* d1, EventHeader* d2) const { return apply<EventHeader, EventHeader>(d1, d2); }
  bool operator()(EventHeader* d1, HitDetector* d2) const { return apply<EventHeader, HitDetector>(d1, d2); }
  bool operator()(EventHeader* d1, ModuleDetector* d2) const { return apply<EventHeader, ModuleDetector>(d1, d2); }
  bool operator()(HitDetector* d1, TrackDetector* d2) const { return apply<HitDetector, TrackDetector>(d1, d2); }
  bool operator()(HitDetector* d1, Particles* d2) const { return apply<HitDetector, Particles>(d1, d2); }
  bool operator()(HitDetector* d1, EventHeader* d2) const { return apply<HitDetector, EventHeader>(d1, d2); }
  bool operator()(HitDetector* d1, HitDetector* d2) const { return apply<HitDetector, HitDetector>(d1, d2); }
  bool operator()(HitDetector* d1, ModuleDetector* d2) const { return apply<HitDetector, ModuleDetector>(d1, d2); }
  bool operator()(ModuleDetector* d1, TrackDetector* d2) const { return apply<ModuleDetector, TrackDetector>(d1, d2); }
  bool operator()(ModuleDetector* d1, Particles* d2) const { return apply<ModuleDetector, Particles>(d1, d2); }
  bool operator()(ModuleDetector* d1, EventHeader* d2) const { return apply<ModuleDetector, EventHeader>(d1, d2); }
  bool operator()(ModuleDetector* d1, HitDetector* d2) const { return apply<ModuleDetector, HitDetector>(d1, d2); }
  bool operator()(ModuleDetector* d1, ModuleDetector* d2) const { return apply<ModuleDetector, ModuleDetector>(d1, d2); }
  int ch1_;
  int ch2_;
  const Cuts* cut_;
};

struct fill_2_branches : public boost::static_visitor<double> {
  fill_2_branches(const Variable& var, int ch1, int ch2) : var_(var), ch1_(ch1), ch2_(ch2) {}
  template<class Det1, class Det2>
  double apply(Det1* d1, Det2* d2) const { return var_.GetValue(d1->GetChannel(ch1_), d1->GetId(), d2->GetChannel(ch2_), d2->GetId()); }
  double operator()(TrackDetector* d1, TrackDetector* d2) const { return apply<TrackDetector, TrackDetector>(d1, d2); }
  double operator()(TrackDetector* d1, Particles* d2) const { return apply<TrackDetector, Particles>(d1, d2); }
  double operator()(TrackDetector* d1, EventHeader* d2) const { return apply<TrackDetector, EventHeader>(d1, d2); }
  double operator()(TrackDetector* d1, HitDetector* d2) const { return apply<TrackDetector, HitDetector>(d1, d2); }
  double operator()(TrackDetector* d1, ModuleDetector* d2) const { return apply<TrackDetector, ModuleDetector>(d1, d2); }
  double operator()(Particles* d1, TrackDetector* d2) const { return apply<Particles, TrackDetector>(d1, d2); }
  double operator()(Particles* d1, Particles* d2) const { return apply<Particles, Particles>(d1, d2); }
  double operator()(Particles* d1, EventHeader* d2) const { return apply<Particles, EventHeader>(d1, d2); }
  double operator()(Particles* d1, HitDetector* d2) const { return apply<Particles, HitDetector>(d1, d2); }
  double operator()(Particles* d1, ModuleDetector* d2) const { return apply<Particles, ModuleDetector>(d1, d2); }
  double operator()(EventHeader* d1, TrackDetector* d2) const { return apply<EventHeader, TrackDetector>(d1, d2); }
  double operator()(EventHeader* d1, Particles* d2) const { return apply<EventHeader, Particles>(d1, d2); }
  double operator()(EventHeader* d1, EventHeader* d2) const { return apply<EventHeader, EventHeader>(d1, d2); }
  double operator()(EventHeader* d1, HitDetector* d2) const { return apply<EventHeader, HitDetector>(d1, d2); }
  double operator()(EventHeader* d1, ModuleDetector* d2) const { return apply<EventHeader, ModuleDetector>(d1, d2); }
  double operator()(HitDetector* d1, TrackDetector* d2) const { return apply<HitDetector, TrackDetector>(d1, d2); }
  double operator()(HitDetector* d1, Particles* d2) const { return apply<HitDetector, Particles>(d1, d2); }
  double operator()(HitDetector* d1, EventHeader* d2) const { return apply<HitDetector, EventHeader>(d1, d2); }
  double operator()(HitDetector* d1, HitDetector* d2) const { return apply<HitDetector, HitDetector>(d1, d2); }
  double operator()(HitDetector* d1, ModuleDetector* d2) const { return apply<HitDetector, ModuleDetector>(d1, d2); }
  double operator()(ModuleDetector* d1, TrackDetector* d2) const { return apply<ModuleDetector, TrackDetector>(d1, d2); }
  double operator()(ModuleDetector* d1, Particles* d2) const { return apply<ModuleDetector, Particles>(d1, d2); }
  double operator()(ModuleDetector* d1, EventHeader* d2) const { return apply<ModuleDetector, EventHeader>(d1, d2); }
  double operator()(ModuleDetector* d1, HitDetector* d2) const { return apply<ModuleDetector, HitDetector>(d1, d2); }
  double operator()(ModuleDetector* d1, ModuleDetector* d2) const { return apply<ModuleDetector, ModuleDetector>(d1, d2); }
  const Variable& var_;
  int ch1_;
  int ch2_;
};

struct get_n_channels_struct : public boost::static_visitor<size_t> {
  template<class Det>
  size_t get_n_channels(Det* d) const { return d->GetNumberOfChannels(); }
  size_t operator()(TrackDetector* d) const { return get_n_channels<TrackDetector>(d); }
  size_t operator()(HitDetector* d) const { return get_n_channels<HitDetector>(d); }
  size_t operator()(ModuleDetector* d) const { return get_n_channels<ModuleDetector>(d); }
  size_t operator()(Particles* d) const { return get_n_channels<Particles>(d); }
  size_t operator()(EventHeader* d) const { return get_n_channels<EventHeader>(d); }
};

struct get_id_struct : public boost::static_visitor<int> {
  template<class Det>
  int get_id(Det* d) const { return d->GetId(); }
  int operator()(TrackDetector* d) const { return get_id<TrackDetector>(d); }
  int operator()(HitDetector* d) const { return get_id<HitDetector>(d); }
  int operator()(ModuleDetector* d) const { return get_id<ModuleDetector>(d); }
  int operator()(Particles* d) const { return get_id<Particles>(d); }
  int operator()(EventHeader* d) const { return get_id<EventHeader>(d); }
};

#else
#include <variant>
#pragma message("Compiling with c++17")
#define get_func std::get
#endif

#endif//ANALYSISTREE_INFRA_VARIANTMAGIC_HPP_
