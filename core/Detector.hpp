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

  ~Detector() override {
    ClearChannels();
  }

  size_t GetNumberOfChannels() const {
    return channels_->size();
  }

  T *AddChannel() {
    channels_->push_back(T(channels_->size()));
    return &(channels_->back());
  }

  void ClearChannels() {
    if (channels_ != nullptr) channels_->clear();
  }

  T &GetChannel(size_t number)// needed in converter to modify tracks id. //TODO maybe rename?
  {
    if (number < GetNumberOfChannels()) {
      return channels_->at(number);
    } else {
      throw std::out_of_range("Detector::GetChannel - wrong channel number " + std::to_string(number) + " Number of channels in this detector is " + std::to_string(GetNumberOfChannels()));
    }
  }

  const T &GetChannel(size_t number) const {
    if (number < GetNumberOfChannels()) {
      return channels_->at(number);
    } else {
      throw std::out_of_range("Detector::GetChannel - wrong channel number " + std::to_string(number) + " Number of channels in this detector is " + std::to_string(GetNumberOfChannels()));
    }
  }

  friend bool operator==(const Detector &that, const Detector &other) {
    if (&that == &other) {
      return true;
    }

    if ((IndexedObject &) that != (IndexedObject &) other) {
      return false;
    }

    return std::equal(that.channels_->begin(), that.channels_->end(), other.channels_->begin());
  }

  const std::vector<T> *GetChannels() const { return channels_; }

  void Reserve(size_t n) { channels_->reserve(n); }
  void Resize(size_t n) { channels_->resize(n); }

  void Print() const {
    for (const auto &channel : *channels_) {
      channel.Print();
    }
  }

 protected:
  std::vector<T> *channels_{new std::vector<T>};
};

using TrackDetector = Detector<Track>;
using ModuleDetector = Detector<Module>;
using ModulePositions = Detector<ModulePosition>;
using HitDetector = Detector<Hit>;
using Particles = Detector<Particle>;

}// namespace AnalysisTree

#endif//ANALYSISTREE_GENERICCHANNELDETECTOR_H
