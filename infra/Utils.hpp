//
// Created by eugene on 17/11/2020.
//

#ifndef ANALYSISTREE_INFRA_UTILS_HPP
#define ANALYSISTREE_INFRA_UTILS_HPP

#ifndef __has_include
#   define __has_include(MACRO) 0
#endif

#if __has_include(<variant>)
#   include <variant>
#   if defined(__cpp_lib_variant)
#     define ANALYSISTREE_STD_VARIANT 1
#     define ANALYSISTREE_UTILS_VARIANT std::variant
#     define ANALYSISTREE_UTILS_VISIT std::visit
#     define ANALYSISTREE_UTILS_GET std::get
#   endif
#endif

#if __has_include(<boost/variant.hpp>)
#  define ANALYSISTREE_BOOST_VARIANT 1
#endif

#if !defined(ANALYSISTREE_STD_VARIANT) && defined(ANALYSISTREE_BOOST_VARIANT)
#   include <boost/variant.hpp>
#   include <boost/variant/static_visitor.hpp>
#   define ANALYSISTREE_BOOST_VARIANT 1
#   define ANALYSISTREE_UTILS_VARIANT boost::variant
#   define ANALYSISTREE_UTILS_VISIT boost::apply_visitor
#   define ANALYSISTREE_UTILS_GET boost::get
#endif

namespace AnalysisTree {

namespace Utils {

template<typename RetType>
struct Visitor
#if !defined(ANALYSISTREE_STD_VARIANT)
    : public boost::static_visitor<RetType>
#endif// USEBOOST
{
 public:
#if !defined(ANALYSISTREE_BOOST_VARIANT)
  typedef RetType result_type;
#endif
  virtual ~Visitor() {}
};


}// namespace Utils

}// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_UTILS_HPP
