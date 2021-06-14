/* Copyright (C) 2019-2021 GSI, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Eugeny Kashirin, Ilya Selyuzhenkov */
#include "ValueHolder.hpp"
#include "Branch.hpp"

#include <Hit.hpp>
#include <Module.hpp>
#include <Particle.hpp>
#include <Track.hpp>
#include <cassert>

using namespace AnalysisTree;

namespace Impl {

template<typename Entity>
inline Entity* DataT(void* data_ptr) { return reinterpret_cast<Entity*>(data_ptr); }
template<typename Entity>
inline const Entity* DataT(const void* data_ptr) { return reinterpret_cast<const Entity*>(data_ptr); }

template<typename Functor, typename DataPtr>
auto ApplyToEntity(AnalysisTree::DetType det_type, DataPtr ptr, Functor&& functor) {
  return std::visit(functor, ptr);
//  using AnalysisTree::DetType;
//  if (DetType::kParticle == det_type) {
//    return functor(DataT<AnalysisTree::Particle>(ptr));
//  } else if (DetType::kTrack == det_type) {
//    return functor(DataT<AnalysisTree::Track>(ptr));
//  } else if (DetType::kModule == det_type) {
//    return functor(DataT<AnalysisTree::Module>(ptr));
//  } else if (DetType::kHit == det_type) {
//    return functor(DataT<AnalysisTree::Hit>(ptr));
//  } else if (DetType::kEventHeader == det_type) {
//    return functor(DataT<AnalysisTree::EventHeader>(ptr));
//  }
//  /* unreachable */
//  __builtin_unreachable();
//  assert(false);
}

template<typename Entity, typename ValueType>
ValueType ReadValue(const Field& v, const Entity* e) {
  using AnalysisTree::Types;

  if (v.GetFieldType() == Types::kFloat) {
    return (ValueType) e->template GetField<float>(v.GetFieldId());
  } else if (v.GetFieldType() == Types::kInteger) {
    return (ValueType) e->template GetField<int>(v.GetFieldId());
  } else if (v.GetFieldType() == Types::kBool) {
    return (ValueType) e->template GetField<bool>(v.GetFieldId());
  } else if (v.GetFieldType() == Types::kNumberOfTypes) {
    /* Types::kNumberOfTypes */
    assert(false);
  }
  /* unreachable */
  __builtin_unreachable();
  assert(false);
}

template<typename Entity, typename ValueType>
inline void SetValue(const Field& v, Entity* e, ValueType new_value) {
  using AnalysisTree::Types;

  if (v.GetFieldType() == Types::kFloat) {
    e->SetField(Floating_t (new_value), v.GetFieldId());
    return;
  } else if (v.GetFieldType() == Types::kInteger) {
    e->SetField(Integer_t (new_value), v.GetFieldId());
    return;
  } else if (v.GetFieldType() == Types::kBool) {
    e->SetField(bool(new_value), v.GetFieldId());
    return;
  }
  /* unreachable */
  __builtin_unreachable();
  assert(false);
}

}// namespace Impl
float ValueHolder::GetVal() const {
  return Impl::ApplyToEntity(v.GetParentBranch()->GetBranchType(),
                             data_ptr, [this](auto entity_ptr) {
                               using Entity = std::remove_const_t<std::remove_pointer_t<decltype(entity_ptr)>>;
                               return Impl::ReadValue<Entity, float>(this->v, entity_ptr);
                             });
}

int ValueHolder::GetInt() const {
  return Impl::ApplyToEntity(v.GetParentBranch()->GetBranchType(),
                             data_ptr, [this](auto entity_ptr) {
                               using Entity = std::remove_const_t<std::remove_pointer_t<decltype(entity_ptr)>>;
                               return Impl::ReadValue<Entity, int>(this->v, entity_ptr);
                             });
}

bool ValueHolder::GetBool() const {
  return Impl::ApplyToEntity(v.GetParentBranch()->GetBranchType(),
                             data_ptr, [this](auto entity_ptr) {
                               using Entity = std::remove_const_t<std::remove_pointer_t<decltype(entity_ptr)>>;
                               return Impl::ReadValue<Entity, bool>(this->v, entity_ptr);
                             });
}
ValueHolder::operator float() const {
  return GetVal();
}
void ValueHolder::SetVal(float val) const {
  v.GetParentBranch()->CheckMutable(true);
  Impl::ApplyToEntity(v.GetParentBranch()->GetBranchType(), data_ptr, [this, val](auto entity_ptr) {
    Impl::SetValue(v, entity_ptr, val);
  });
}
void ValueHolder::SetVal(int val) const {
  v.GetParentBranch()->CheckMutable(true);
  Impl::ApplyToEntity(v.GetParentBranch()->GetBranchType(), data_ptr, [this, val](auto entity_ptr) {
    Impl::SetValue(v, entity_ptr, val);
  });
}
void ValueHolder::SetVal(bool val) const {
  v.GetParentBranch()->CheckMutable(true);
  Impl::ApplyToEntity(v.GetParentBranch()->GetBranchType(), data_ptr, [this, val](auto entity_ptr) {
    Impl::SetValue(v, entity_ptr, val);
  });
}

ValueHolder& ValueHolder::operator=(const ValueHolder& other) {
  if (this == &other) {
    return *this;
  }

  v.GetParentBranch()->CheckMutable(true);

  using AnalysisTree::Types;
  if (other.v.GetFieldType() == Types::kFloat) {
    this->SetVal(other.GetVal());
  } else if (other.v.GetFieldType() == Types::kInteger) {
    this->SetVal(other.GetInt());
  } else if (other.v.GetFieldType() == Types::kBool) {
    this->SetVal(other.GetBool());
  } else {
    /* unreachable */
    assert(false);
  }

  return *this;
}
