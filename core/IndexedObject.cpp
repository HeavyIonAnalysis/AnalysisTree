//
// Created by eugene on 10/3/18.
//
#include "IndexedObject.hpp"



namespace AnalysisTree{

void IndexAccessor::InitIndexedObject(IndexedObject *indexedObject, Integer_t newID) {
  indexedObject->id_ = newID;
}

}
