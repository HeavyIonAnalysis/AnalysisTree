/* Copyright (C) 2019-2021 GSI, Universität Tübingen, MEPhI
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Eugeny Kashirin, Ilya Selyuzhenkov */
#ifndef ANALYSISTREE_BASEEVENTHEADER_H
#define ANALYSISTREE_BASEEVENTHEADER_H

#include "Container.hpp"
#include <array>
#include <stdexcept>

class TVector3;

namespace AnalysisTree {

/**
 * A class for an event header
 */
class EventHeader : public Container {
 public:
  EventHeader() = default;
  explicit EventHeader(size_t id) : Container(id){};
  explicit EventHeader(const Container& cont) : Container(cont) {}
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
        case EventHeaderFields::kId: return GetId();
        default: throw std::out_of_range("EventHeader::GetField - Index " + std::to_string(iField) + " is not found");
      }
    }
  }

  template<typename T>
  void SetField(T value, Integer_t field_id) {
    using AnalysisTree::EventHeaderFields::EventHeaderFields;
    if (field_id >= 0) {
      Container::SetField(value, field_id);
    } else {
      switch (field_id) {
        case EventHeaderFields::kVertexX: vtx_pos_[Exyz::kX] = value; break;
        case EventHeaderFields::kVertexY: vtx_pos_[Exyz::kY] = value; break;
        case EventHeaderFields::kVertexZ: vtx_pos_[Exyz::kZ] = value; break;
        case EventHeaderFields::kId: break;
        default: throw std::runtime_error("Invalid field id");
      }
    }
  }

  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetVertexX() const { return vtx_pos_[Exyz::kX]; }
  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetVertexY() const { return vtx_pos_[Exyz::kY]; }
  ANALYSISTREE_ATTR_NODISCARD inline Floating_t GetVertexZ() const { return vtx_pos_[Exyz::kZ]; }

  static constexpr size_t GetNumberOfChannels() { return 1; }// needed in order to have EventHeader similar to Detector
  EventHeader* begin() { return this; }
  EventHeader* end() { return this; }
  const EventHeader* cbegin() const { return this; }
  const EventHeader* cend() const { return this; }

  ANALYSISTREE_ATTR_NODISCARD const EventHeader& GetChannel(size_t i) const;// needed in order to have EventHeader similar to Detector
  ANALYSISTREE_ATTR_NODISCARD EventHeader& Channel(size_t i);               // needed in order to have EventHeader similar to Detector
  static void ClearChannels() { throw std::runtime_error("Not available for EventHeader"); }
  static EventHeader* AddChannel() { throw std::runtime_error("Not available for EventHeader"); }

  void Print() const noexcept override;

 protected:
  std::array<Floating_t, 3> vtx_pos_{{UndefValueFloat, UndefValueFloat, UndefValueFloat}};

  ClassDefOverride(EventHeader, 2)
};

}// namespace AnalysisTree
#endif//ANALYSISTREE_BASEEVENTHEADER_H
