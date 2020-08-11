
#ifndef ANALYSISTREE_INFRA_FIELD_H_
#define ANALYSISTREE_INFRA_FIELD_H_

#include <string>

#include "Constants.hpp"

namespace AnalysisTree {

class Configuration;

class Field {
 public:
  Field() = default;
  Field(const Field &) = default;
  Field(Field &&) = default;
  Field &operator=(Field &&) = default;
  Field &operator=(const Field &) = default;
  ~Field() = default;

  Field(std::string name) : field_(std::move(name)){};

  Field(std::string branch, std::string field) : branch_(std::move(branch)),
                                                 field_(std::move(field)){};

  friend bool operator==(const Field &that, const Field &other);
  friend bool operator>(const Field &that, const Field &other);
  friend bool operator<(const Field &that, const Field &other);

  void Init(const Configuration &conf);

  [[nodiscard]] const std::string &GetName() const { return field_; }
  [[nodiscard]] const std::string &GetBranchName() const { return branch_; }

  [[nodiscard]] short GetBranchId() const { return branch_id_; }
  [[nodiscard]] short GetFieldId() const { return field_id_; }

  [[nodiscard]] DetType GetBranchType() const { return branch_type_; }
  [[nodiscard]] Types GetFieldType() const { return field_type_; }

  template<class T>
  double GetValue(const T &object) const {
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

}// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_FIELD_H_
