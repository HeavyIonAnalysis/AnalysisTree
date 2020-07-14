#include "SimpleCut.hpp"

#include <iostream>

ClassImp(AnalysisTree::SimpleCut)

void AnalysisTree::SimpleCut::Print() const {
  std::cout << title_ << std::endl;
}
