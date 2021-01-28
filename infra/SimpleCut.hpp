#ifndef ANALYSISTREE_SIMPLECUT_H
#define ANALYSISTREE_SIMPLECUT_H

#include <algorithm>
#include <functional>
#include <set>
#include <string>
#include <vector>

#include "Constants.hpp"
#include "Utils.hpp"
#include "Variable.hpp"

namespace AnalysisTree {

/**
 * @brief SimpleCut keeps predicate (lambda function with vector of arguments) and list of Variables.
 */
class SimpleCut {

 public:
  SimpleCut() = default;
  SimpleCut(const SimpleCut& cut) = default;
  SimpleCut(SimpleCut&& cut) = default;
  SimpleCut& operator=(SimpleCut&&) = default;
  SimpleCut& operator=(const SimpleCut& cut) = default;
  virtual ~SimpleCut() = default;

  /**
  * Constructor for generic cut: bool f(var1, var2, ..., varn)
  * @param vars vector of variable NAMES needed for a cut
  * @param lambda function of fields, returns bool
  */
  SimpleCut(std::vector<std::string> vars, std::function<bool(std::vector<double>&)> lambda, std::string title = "") : title_(std::move(title)),
                                                                                                                       lambda_(std::move(lambda)) {
    std::transform(vars.begin(), vars.end(),
                   std::back_inserter(vars_),
                   [](const std::string& arg_name) { return Variable::FromString(arg_name); });

    FillBranchNames();
  }

  /**
   * @brief Evaluates cut
   * @tparam T type of data-object associated with TTree
   * @param object
   * @return result of cut
   */
  template<class T>
  bool Apply(const T& object) const {
    std::vector<double> variables;
    variables.reserve(vars_.size());
    for (const auto& var : vars_) {
      variables.emplace_back(var.GetValue(object));
    }
    return lambda_(variables);
  }

  template<class A, class B>
  bool Apply(const A& a, int a_id, const B& b, int b_id) const {
    std::vector<double> variables;
    variables.reserve(vars_.size());
    for (const auto& var : vars_) {
      variables.emplace_back(var.GetValue(a, a_id, b, b_id));
    }
    return lambda_(variables);
  }

  void Print() const;

  std::vector<Variable>& Variables() { return vars_; }
  ANALYSISTREE_ATTR_NODISCARD const std::vector<Variable>& GetVariables() const { return vars_; }
  ANALYSISTREE_ATTR_NODISCARD const std::set<std::string>& GetBranches() const { return branch_names_; }

  friend bool operator==(const SimpleCut& that, const SimpleCut& other);

  friend SimpleCut RangeCut(const std::string& variable_name, float lo, float hi, const std::string& title);
  friend SimpleCut EqualsCut(const std::string& variable_name, int value, const std::string& title);

 protected:
  /**
* Constructor for range cut: min <= field <= max
* @param field name of the field
* @param min minimal accepted value
* @param max maximal accepted value
*/
  //  ANALYSISTREE_ATTR_DEPRECATED("Use AnalysisTree::RangeCut instead")
  SimpleCut(const Variable& var, float min, float max, std::string title = "") : title_(std::move(title)) {
    vars_.emplace_back(var);
    lambda_ = [max, min](std::vector<double>& vars) { return vars[0] <= max && vars[0] >= min; };
    FillBranchNames();
  }
  /**
  * Constructor for integers and bool fields for cut: field == value
  * @param field name of the field
  * @param value only objects with field == value will be accepted
  */
  //  ANALYSISTREE_ATTR_DEPRECATED("Use AnalysisTree::EqualsCut")
  SimpleCut(const Variable& var, int value, std::string title = "") : title_(std::move(title)) {
    vars_.emplace_back(var);
    lambda_ = [value](std::vector<double>& vars) { return vars[0] <= value + SmallNumber && vars[0] >= value - SmallNumber; };
    FillBranchNames();
  }

  void FillBranchNames();

  std::string title_;
  std::vector<Variable> vars_{};
  std::set<std::string> branch_names_{};
  std::function<bool(std::vector<double>&)> lambda_;///< function used to evaluate the cut.

  ClassDef(SimpleCut, 1)
};

SimpleCut RangeCut(const std::string& variable_name, float lo, float hi, const std::string& title = "");
SimpleCut EqualsCut(const std::string& variable_name, int value, const std::string& title = "");

}// namespace AnalysisTree
#endif//ANALYSISTREE_SIMPLECUT_H