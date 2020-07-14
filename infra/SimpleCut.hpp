#ifndef ANALYSISTREE_SIMPLECUT_H
#define ANALYSISTREE_SIMPLECUT_H

#include <functional>
#include <string>
#include <vector>

#include "Constants.hpp"
#include "Variable.hpp"

namespace AnalysisTree {

class SimpleCut {

 public:
  SimpleCut() = default;
  SimpleCut(const SimpleCut &cut) = default;
  SimpleCut(SimpleCut &&cut) = default;
  SimpleCut &operator=(SimpleCut &&) = default;
  SimpleCut &operator=(const SimpleCut &cut) = default;
  virtual ~SimpleCut() = default;

  /**
  * Constructor for range cut: min <= field <= max
  * @param field name of the field
  * @param min minimal accepted value
  * @param max maximal accepted value
  */
  SimpleCut(const Variable &var, float min, float max, std::string title = "") : title_(std::move(title)) {
    vars_.emplace_back(var);
    lambda_ = [max, min](std::vector<double> &vars) { return vars[0] <= max && vars[0] >= min; };
  }
  /**
  * Constructor for integers and bool fields for cut: field == value
  * @param field name of the field
  * @param value only objects with field == value will be accepted
  */
  SimpleCut(const Variable &var, int value, std::string title = "") : title_(std::move(title)) {
    vars_.emplace_back(var);
    lambda_ = [value](std::vector<double> &vars) { return vars[0] <= value + SmallNumber && vars[0] >= value - SmallNumber; };
  }
  /**
  * Constructor for generic cut: bool f(var1, var2, ..., varn)
  * @param vars vector of Variables needed for a cut
  * @param lambda function of fields, returns bool
  */
  SimpleCut(std::vector<Variable> vars, std::function<bool(std::vector<double> &)> lambda, std::string title = "") : title_(std::move(title)),
                                                                                                                     vars_(std::move(vars)),
                                                                                                                     lambda_(std::move(lambda)) {
  }

  template<typename T>
  bool Apply(const T &object) const {
    std::vector<double> variables;
    variables.reserve(vars_.size());
    for (const auto &var : vars_) {
      variables.emplace_back(var.GetValue(object));
    }
    return lambda_(variables);
  }

  void Print() const;

  std::vector<Variable> &Variables() { return vars_; }
  const std::vector<Variable> &GetVariables() const { return vars_; }

 protected:
  std::string title_{""};
  std::vector<Variable> vars_{};
  std::function<bool(std::vector<double> &)> lambda_;///< function used to evaluate the cut.

  ClassDef(AnalysisTree::SimpleCut, 1)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_SIMPLECUT_H