#ifndef ANALYSISTREE_INDEXEDOBJECT_H
#define ANALYSISTREE_INDEXEDOBJECT_H

#include "Constants.hpp"
#include <TObject.h>

namespace AnalysisTree {

class IndexedObject;

/**
 * An interface class for the object having an access
 * to the private field ID of IndexedObject
 */
class IndexAccessor {

 public:
  static void InitIndexedObject(IndexedObject* indexedObject, Integer_t newID);
};

/**
 * This is the base class for all indexed objects
 * like tracks, channels, detectors etc
 * By the contract field id_ is uniq and an actual position inside
 * vector object (e.g. TClonesArray)
 */
class IndexedObject {
 public:
  IndexedObject() = default;//, id_(-1)  {}

  /**
   * Object ID is defined at the object creation and mustn't change
   * anymore in the lifecycle
   * @param id_
   */
  explicit IndexedObject(Integer_t id) : id_(id) {}

  /**
   * Copy ctor for IndexedObject
   * TODO consider if we do really need this ctor since as soon as it was copied it is not uniq
   * @param indexedObject
   */
  IndexedObject(const IndexedObject& indexedObject) = default;
  IndexedObject(IndexedObject&& indexedObject) = default;
  IndexedObject& operator=(IndexedObject&&) = default;
  IndexedObject& operator=(const IndexedObject& indexedObject) = default;
  virtual ~IndexedObject() = default;

  ANALYSISTREE_ATTR_NODISCARD inline Integer_t GetId() const {
    return id_;
  }

  friend bool operator==(const IndexedObject& that, const IndexedObject& other) {
    if (&that == &other) {
      return true;
    }

    return that.id_ == other.id_;
  }

  friend bool operator!=(const IndexedObject& that, const IndexedObject& other) {
    return !(that == other);
  }

 private:
  friend class IndexAccessor;

  Integer_t id_{-1};

  ClassDef(IndexedObject, 1)
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_INDEXEDOBJECT_H
