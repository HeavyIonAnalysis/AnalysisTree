/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_INFRA_CHAINDRAWHELPER_HPP_
#define ANALYSISTREE_INFRA_CHAINDRAWHELPER_HPP_

#include "Configuration.hpp"

#include <string>
#include <vector>

namespace AnalysisTree {

class ChainDrawHelper {

 public:
  explicit ChainDrawHelper(Configuration* configuration) : configuration_(configuration) {}
  void DrawTransform(std::string& expr);

 protected:
  Configuration* configuration_{nullptr};

  static std::vector<std::pair<std::string, int>> FindAndRemoveFields(std::string& expr);
  void DrawFieldTransform(std::string& expr);
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_CHAINDRAWHELPER_HPP_
