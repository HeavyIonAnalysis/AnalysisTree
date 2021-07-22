/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "ChainDrawHelper.hpp"
#include <TString.h>

namespace AnalysisTree{

void ChainDrawHelper::DrawFieldTransform(std::string& expr) {
  auto dot = expr.find('.');
  auto branch = expr.substr(0, dot);
  auto field = expr.substr(dot + 1);

  const auto& br = configuration_->GetBranchConfig(branch);
  auto type = br.GetFieldType(field);
  auto id = br.GetFieldId(field);
  std::string type_str{};

  switch (type) {
    case (Types::kFloat): {
      type_str = "float";
      break;
    }
    case (Types::kInteger): {
      type_str = "int";
      break;
    }
    case (Types::kBool): {
      type_str = "bool";
      break;
    }
    default: {
      throw std::runtime_error("Field type is not defined");
    }
  }

  expr = Form("%s.channels_.GetField<%s>(%i)", branch.c_str(), type_str.c_str(), id);
}

std::vector<std::pair<std::string, int>> ChainDrawHelper::FindAndRemoveFields(std::string& expr) {

  std::vector<std::pair<std::string, int>> fields{};

  int pos = 0;
  while (expr.find('.', pos) != size_t(-1)) {//TODO fix this
    auto dot = expr.find('.', pos);
    if ((!isdigit(expr[dot - 1]) || !isdigit(expr[dot + 1])) && expr[dot + 1] != ' ') {// is not a number like 1.5 or 1.

      auto begin = dot;
      auto is_allowed_char = [](char c) { return isalpha(c) || isdigit(c) || c == '_'; };
      while (begin > 0 && is_allowed_char(expr[begin - 1])) {
        begin--;
      }
      auto end = dot;
      while (end < expr.size() - 1 && is_allowed_char(expr[end + 1])) {
        end++;
      }
      auto field = expr.substr(begin, end - begin + 1);
      fields.emplace_back(std::make_pair(field, begin));
      expr.erase(begin, end - begin + 1);
      pos = begin;
    } else {
      pos = dot + 1;
    }
  }
  return fields;
}

void ChainDrawHelper::DrawTransform(std::string& expr) {
  auto fields = FindAndRemoveFields(expr);
  int sum{0};
  for (auto& field : fields) {
    DrawFieldTransform(field.first);
    expr.insert(field.second + sum, field.first);
    sum += field.first.size();
  }
}

}