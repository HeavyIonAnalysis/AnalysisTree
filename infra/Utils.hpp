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
#   endif
#endif

#if !defined(ANALYSISTREE_STD_VARIANT) && defined(ANALYSISTREE_BOOST_FOUND)
#   include <boost/variant.hpp>
#   include <boost/variant/static_visitor.hpp>
#   define ANALYSISTREE_BOOST_VARIANT 1
#   define ANALYSISTREE_UTILS_VARIANT boost::variant
#   define ANALYSISTREE_UTILS_VISIT boost::apply_visitor
#endif

#ifndef __has_cpp_attribute // if we don't have __has_attribute, ignore it
#   define __has_cpp_attribute(x) 0
#endif

#if __has_cpp_attribute(deprecated)
#   define ANALYSISTREE_ATTR_DEPRECATED(MESSAGE) [[deprecated]]
#else
#   define ANALYSISTREE_ATTR_DEPRECATED(MESSAGE)  // attribute "deprecated" not available
#endif

#if __has_cpp_attribute(nodiscard)
#   define  ANALYSISTREE_ATTR_NODISCARD [[nodiscard]]
#else
#   define  ANALYSISTREE_ATTR_NODISCARD
#endif


namespace AnalysisTree {

namespace Utils {

template<typename RetType>
struct Visitor
#ifdef ANALYSISTREE_BOOST_VARIANT
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
