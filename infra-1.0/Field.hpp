
#ifndef ANALYSISTREE_INFRA_FIELD_H_
#define ANALYSISTREE_INFRA_FIELD_H_

#include <string>
#include <stdexcept>

#include "Constants.hpp"
#include "Utils.hpp"

namespace AnalysisTree{
class Configuration;
}

namespace AnalysisTree::Version1 {
/**
 * @brief Field is a _pointer_ in a branch/field structure.
 */
class Field {
 public:
  Field() = default;
  Field(const Field&) = default;
  Field(Field&&) = default;
  Field& operator=(Field&&) = default;
  Field& operator=(const Field&) = default;
  ~Field() = default;

  Field(std::string name) : field_(std::move(name)){};

  Field(std::string branch, std::string field) : branch_(std::move(branch)),
                                                 field_(std::move(field)){};

  friend bool operator==(const Field& that, const Field& other);
  friend bool operator>(const Field& that, const Field& other);
  friend bool operator<(const Field& that, const Field& other);

  /**
   * @brief Initializes branchId and fieldId
   * @param conf Configuration
   */
  void Init(const AnalysisTree::Configuration& conf);

  ANALYSISTREE_ATTR_NODISCARD const std::string& GetName() const { return field_; }
  ANALYSISTREE_ATTR_NODISCARD const std::string& GetBranchName() const { return branch_; }

  ANALYSISTREE_ATTR_NODISCARD short GetBranchId() const { return branch_id_; }
  ANALYSISTREE_ATTR_NODISCARD short GetFieldId() const { return field_id_; }

  ANALYSISTREE_ATTR_NODISCARD DetType GetBranchType() const { return branch_type_; }
  ANALYSISTREE_ATTR_NODISCARD Types GetFieldType() const { return field_type_; }

  /**
   * @brief Gets numerical value from data-object associated with TTree
   * @tparam T type of data-object (EventHeader, Track, etc)
   * @param object
   * @return value of the field
   */
  template<class T>
  double GetValue(const T& object) const {
    if (!is_init_) {
      throw std::runtime_error("Field::Fill - Field " + field_ + " is not initialized");
    }
    if (field_type_ == Types::kFloat)
      return object.template GetField<float>(field_id_);
    else if (field_type_ == Types::kInteger)
      return object.template GetField<int>(field_id_);
    else if (field_type_ == Types::kBool)
      return object.template GetField<bool>(field_id_);
    return UndefValueFloat;
  }

  void Print() const;

 private:
  std::string branch_;
  std::string field_;

  short field_id_{};
  Types field_type_{};

  short branch_id_{UndefValueShort};
  DetType branch_type_{DetType(UndefValueShort)};

  bool is_init_{false};
};

}// namespace AnalysisTree::Version1

#endif//ANALYSISTREE_INFRA_FIELD_H_
