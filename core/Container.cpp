/* Copyright (C) 2019-2021 GSI, Universität Tübingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Ilya Selyuzhenkov */
#include "Container.hpp"

#include "BranchConfig.hpp"

namespace AnalysisTree {

template<>
std::vector<int>& Container::Vector<int>() { return ints_; }
template<>
std::vector<float>& Container::Vector<float>() { return floats_; }
template<>
std::vector<bool>& Container::Vector<bool>() { return bools_; }

template<>
const std::vector<int>& Container::GetVector<int>() const { return ints_; }
template<>
const std::vector<float>& Container::GetVector<float>() const { return floats_; }
template<>
const std::vector<bool>& Container::GetVector<bool>() const { return bools_; }

void Container::Init(const AnalysisTree::BranchConfig& branch) {
  floats_.resize(branch.GetSize<float>());
  ints_.resize(branch.GetSize<int>());
  bools_.resize(branch.GetSize<bool>());
}

void Container::Print() const noexcept{
  if(!ints_.empty()){
    std::cout << "Integer fields: ";
    for(auto i : ints_){
      std::cout << i << " ";
    }
    std::cout << std::endl;
  }
  if(!floats_.empty()){
    std::cout << "Floating fields: ";
    for(auto f : floats_){
      std::cout << f << " ";
    }
    std::cout << std::endl;
  }
  if(!bools_.empty()){
    std::cout << "Boolean fields: ";
    for(auto b : bools_){
      std::cout << b << " ";
    }
    std::cout << std::endl;
  }
}


}// namespace AnalysisTree