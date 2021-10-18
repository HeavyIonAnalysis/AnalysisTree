#ifndef ANALYSISTREE_BASEEVENTHEADER_H
#define ANALYSISTREE_BASEEVENTHEADER_H

#include "Container.hpp"
#include <array>
#include <cassert>
#include <stdexcept>

class TVector3;

namespace AnalysisTree {

class EventHeader;

namespace Impl {
void* Data(EventHeader*, Integer_t, Types field_type);
}

/**
 * A class for an event header
 */
class EventHeader : public Container {
 public:
  EventHeader() = default;
  explicit EventHeader(size_t id) : Container(id){};
  EventHeader(size_t id, const BranchConfig& branch) noexcept : Container(id, branch) {}

  EventHeader(const EventHeader& eh) = default;
  EventHeader(EventHeader&& eh) = default;
  EventHeader& operator=(EventHeader&& eh) = default;
  EventHeader& operator=(const EventHeader& eh) = default;

  /**
   * @return 3D-position of the vertex
   */
  ANALYSISTREE_ATTR_NODISCARD TVector3 GetVertexPosition3() const;

  void SetVertexPosition3(const TVector3& pos);

  template<typename T>
  T GetField(Integer_t iField) const {
    if (iField >= 0)
      return Container::GetField<T>(iField);
    else {
      switch (iField) {
        case EventHeaderFields::kVertexX: return GetVertexX();
        case EventHeaderFields::kVertexY: return GetVertexY();
        case EventHeaderFields::kVertexZ: return GetVertexZ();
        default: throw std::out_of_range("EventHeader::GetField - Index " + std::to_string(iField) + " is not found");
      }
    }
  }

  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetVertexX() const { return vtx_pos_[Exyz::kX]; }
  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetVertexY() const { return vtx_pos_[Exyz::kY]; }
  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetVertexZ() const { return vtx_pos_[Exyz::kZ]; }

  static constexpr size_t GetNumberOfChannels() { return 1; }// needed in order to have EventHeader similar to Detector

  ANALYSISTREE_ATTR_NODISCARD const EventHeader& GetChannel(size_t i) const;// needed in order to have EventHeader similar to Detector

 protected:
  std::array<Floating_t, 3> vtx_pos_{{UndefValueFloat, UndefValueFloat, UndefValueFloat}};

  friend void* Impl::Data(EventHeader*, Integer_t, Types field_type);

  ClassDefOverride(EventHeader, 2)
};

namespace Impl {

inline void* Data(EventHeader* header_ptr, Integer_t i_field, Types field_type) {
  switch (i_field) {
    case EventHeaderFields::kVertexX: return &(header_ptr->vtx_pos_[Exyz::kX]);
    case EventHeaderFields::kVertexY: return  &(header_ptr->vtx_pos_[Exyz::kY]);
    case EventHeaderFields::kVertexZ: return &(header_ptr->vtx_pos_[Exyz::kZ]);
    default: return ::AnalysisTree::Impl::Data(dynamic_cast<Container*>(header_ptr), i_field, field_type);
  }
  return nullptr;
}

}// namespace Impl

}// namespace AnalysisTree
#endif//ANALYSISTREE_BASEEVENTHEADER_H
