/* Copyright (C) 2019-2021 GSI, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Eugeny Kashirin, Ilya Selyuzhenkov */
#include "IndexedObject.hpp"

namespace AnalysisTree {

void IndexAccessor::InitIndexedObject(IndexedObject* indexedObject, size_t newID) {
  indexedObject->id_ = newID;
}

}// namespace AnalysisTree
