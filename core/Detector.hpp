#ifndef ANALYSISTREE_GENERICCHANNELDETECTOR_H
#define ANALYSISTREE_GENERICCHANNELDETECTOR_H

#include "IndexedObject.hpp"

#include "Hit.hpp"
#include "Module.hpp"
#include "Particle.hpp"
#include "Track.hpp"

namespace AnalysisTree {
/**
 * A base class for any kind of channel detector.
 * As an IndexAccessor has an access to index variable if the Channel
 * @tparam T a type of channel
 */
template<class T>
class Detector : public IndexedObject, protected IndexAccessor {

 public:
  Detector() = default;
  explicit Detector(Integer_t id) : IndexedObject(id) {}
  Detector(const Detector& otherDetector) = default;
  Detector(Detector&& otherDetector) noexcept = default;
  Detector& operator=(Detector&&) noexcept = default;
  Detector& operator=(const Detector& part) = default;

  ~Detector() override = default;

  [[nodiscard]] size_t GetNumberOfChannels() const noexcept {
    return channels_.size();
  }

  T* AddChannel() {
    channels_.emplace_back(T(channels_.size()));
    return &(channels_.back());
  }

  void ClearChannels() {
      channels_.clear();
  }

  T& Channel(size_t number)// needed in converter to modify tracks id
  {
    if (number < GetNumberOfChannels()) {
      return channels_.at(number);
    } else {
      throw std::out_of_range("Detector::Channel - wrong channel number " + std::to_string(number) + " Number of channels in this detector is " + std::to_string(GetNumberOfChannels()));
    }
  }

  T GetChannel(size_t number) const {
    if (number < GetNumberOfChannels()) {
      return channels_.at(number);
    } else {
      throw std::out_of_range("Detector::GetChannel - wrong channel number " + std::to_string(number) + " Number of channels in this detector is " + std::to_string(GetNumberOfChannels()));
    }
  }

  friend bool operator==(const Detector& that, const Detector& other) {
    if (&that == &other) {
      return true;
    }

    if ((IndexedObject&) that != (IndexedObject&) other) {
      return false;
    }

    return std::equal(that.channels_.begin(), that.channels_.end(), other.channels_.begin());
  }

  [[deprecated("Please use range-based loops: for(const auto& channel : detector)")]]
  const std::vector<T>* GetChannels() const { return &channels_; }
  [[deprecated("Please use range-based loops: for(const auto& channel : detector)")]]
  std::vector<T>* Channels() { return &channels_; }

  void Reserve(size_t n) {
    channels_.reserve(n);
  }

  void Print() const {
    for (const auto& channel : channels_) {
      channel.Print();
    }
  }

  auto begin() -> typename std::vector<T>::iterator { return channels_.begin(); }
  auto end() -> typename std::vector<T>::iterator { return channels_.end(); }
  auto cbegin() const -> typename std::vector<T>::const_iterator  { return channels_.begin(); }
  auto cend() const -> typename std::vector<T>::const_iterator  { return channels_.end(); }
  auto begin() const -> typename std::vector<T>::const_iterator  { return channels_.begin(); }
  auto end() const -> typename std::vector<T>::const_iterator  { return channels_.end(); }

 protected:
  std::vector<T> channels_{};

 ClassDefOverride(Detector,2)

};

using TrackDetector = Detector<Track>;
using ModuleDetector = Detector<Module>;
using ModulePositions = Detector<ModulePosition>;
using HitDetector = Detector<Hit>;
using Particles = Detector<Particle>;
using GeneralDetector = Detector<Container>;

}// namespace AnalysisTree

#endif//ANALYSISTREE_GENERICCHANNELDETECTOR_H
