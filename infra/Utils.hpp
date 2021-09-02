/* Copyright (C) 2019-2021 GSI, Universität Tübingen, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Eugeny Kashirin, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_UTILS_HPP
#define ANALYSISTREE_INFRA_UTILS_HPP

#ifndef __has_include
#define __has_include(MACRO) 0
#endif

#if __has_include(<variant>) && __cplusplus > 201402L
#include <variant>
#define ANALYSISTREE_STD_VARIANT 1
#define ANALYSISTREE_UTILS_VARIANT std::variant
#define ANALYSISTREE_UTILS_VISIT std::visit
#define ANALYSISTREE_UTILS_GET std::get
#elif __has_include(<boost/variant.hpp>)
#include <boost/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#define ANALYSISTREE_BOOST_VARIANT 1
#define ANALYSISTREE_UTILS_VARIANT boost::variant
#define ANALYSISTREE_UTILS_VISIT boost::apply_visitor
#define ANALYSISTREE_UTILS_GET boost::get
#endif

#include "Detector.hpp"

namespace AnalysisTree {

class Track;
class Particle;
class Module;
class Hit;
class EventHeader;

using BranchPointer = ANALYSISTREE_UTILS_VARIANT<HitDetector*, ModuleDetector*, TrackDetector*, EventHeader*, Particles*>;
using ChannelPointer = ANALYSISTREE_UTILS_VARIANT<Hit*, Module*, Track*, EventHeader*, Particle*>;

namespace Utils {

template<typename RetType>
struct Visitor
#if defined(ANALYSISTREE_BOOST_VARIANT)
    : public boost::static_visitor<RetType>
#endif// USEBOOST
{
 public:
#if defined(ANALYSISTREE_STD_VARIANT)
  typedef RetType result_type;
#endif
  virtual ~Visitor() = default;
};

}// namespace Utils

}// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_UTILS_HPP
