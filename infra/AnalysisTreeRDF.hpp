//
// Created by eugene on 17/10/2021.
//

#pragma once

#include <ROOT/RDataSource.hxx>
#include <TFile.h>
#include <TTree.h>
#include <map>
#include <utility>
#include <memory>
#include <cassert>

#include "BranchReader.hpp"
#include "Configuration.hpp"
#include "EventHeader.hpp"
#include "Detector.hpp"

namespace AnalysisTree {

namespace Impl {

template <typename T>
std::vector<std::string> getTransitiveFields() { return {}; }


template <>
std::vector<std::string> getTransitiveFields<::AnalysisTree::Track>() {
  return {"eta", "phi", "pT", "p"};
}





template<typename T>
struct ColumnReader {
  ColumnReader(BranchConfig& config, std::string_view field_name) {
    field_id = config.GetFieldId(std::string(field_name));
    field_type = config.GetFieldType(std::string(field_name));
    if (field_type == Types::kBool) {
      cached = new bool;
    }
  }
  ~ColumnReader() {
    if (cached) {
      delete cached;
    }
  }

  void update(T* ptr) {
    if (field_type == Types::kBool) {
      *((bool*)cached) = ptr->template GetField<bool>(field_id);
      data = cached;
    } else {
      data = Data(ptr, field_id, field_type);
    }
    assert(data);
  }

  void* dataPtr() {
    return std::addressof(data);
  }

  Integer_t field_id{};
  Types field_type{};

  void* cached{nullptr};
  void* data{nullptr};
};

class AnalysisTreeRDFBase {
 protected:
 public:
  AnalysisTreeRDFBase(std::string Filename, std::string TreeName, std::string BranchName) : filename_(std::move(Filename)),
                                                                                            tree_name_(std::move(TreeName)),
                                                                                            branch_name_(std::move(BranchName)) {
    column_names_ = GetColumnNamesImpl();
  }

 protected:
  bool HasColumnImpl(std::string_view View) const {
    return std::find(begin(column_names_), end(column_names_), View) != end(column_names_);
  }
  std::vector<std::string> GetColumnNamesImpl() const {
    TFile f(filename_.c_str(), "read");
    auto configuration = f.template Get<Configuration>("Configuration");
    auto branch_config = configuration->GetBranchConfig(branch_name_);

    std::vector<std::string> result;
    for (const auto& map_entry : branch_config.template GetMap<int>()) {
      result.push_back(map_entry.first);
    }
    for (const auto& map_entry : branch_config.template GetMap<float>()) {
      result.push_back(map_entry.first);
    }
    for (const auto& map_entry : branch_config.template GetMap<bool>()) {
      result.push_back(map_entry.first);
    }
    return result;
  }

  std::string GetTypeNameImpl(std::string_view View) const {
    TFile f(filename_.c_str(), "read");
    auto configuration = f.template Get<Configuration>("Configuration");
    auto branch_config = configuration->GetBranchConfig(branch_name_);
    auto column_type = branch_config.GetFieldType(std::string(View));
    switch (column_type) {
      case Types::kBool:
        return "bool";
      case Types::kInteger:
        return "int";
      case Types::kFloat:
        return "float";
      default:;
    }
    return {};
  }

  std::string filename_;
  std::string tree_name_;
  std::string branch_name_;

  std::vector<std::string> column_names_;
};

template<typename T>
class AnalysisTreeRDFImplT;

template<typename T>
class AnalysisTreeRDFImplT : public AnalysisTreeRDFBase,
                             public ROOT::RDF::RDataSource {

 public:
  AnalysisTreeRDFImplT(std::string Filename,
                       std::string TreeName,
                       std::string BranchName) : AnalysisTreeRDFBase(Filename, TreeName, BranchName) {}

  void SetNSlots(unsigned int nSlots) override {
    slots_.reserve(nSlots);
    for (unsigned int i_slot = 0; i_slot < nSlots; ++i_slot) {
      slots_.push_back(makeSlot());
    }
  }
  const std::vector<std::string>& GetColumnNames() const override {
    return column_names_;
  }
  bool HasColumn(std::string_view View) const override {
    return HasColumnImpl(View);
  }
  std::string GetTypeName(std::string_view View) const override {
    return GetTypeNameImpl(View);
  }
  std::vector<std::pair<ULong64_t, ULong64_t>> GetEntryRanges() override {
    auto last_entry = slots_.back()->ientry;
    auto total_entries = slots_.back()->tree->GetEntries();
    if (last_entry == total_entries - 1) {
      return {};
    }
    std::vector<std::pair<ULong64_t, ULong64_t>> result(slots_.size());
    result[0].first = last_entry;
    result[0].second = total_entries;
    return result;
  }
  bool SetEntry(unsigned int i_slot, ULong64_t entry) override {
    slots_[i_slot]->tree->GetEntry(entry);
    slots_[i_slot]->ientry = entry;
    for (auto& reader : slots_[i_slot]->column_readers) {
      reader.second->update(slots_[i_slot]->entry_ptr);
    }
    return true;
  }

 protected:
  Record_t GetColumnReadersImpl(std::string_view name, const std::type_info& Info) override {
    /* type-erased vector of pointers to pointers to column values - one per slot  */
    Record_t result;
    result.reserve(slots_.size());
    for (auto& slot : slots_) {
      auto new_column_reader = std::make_shared<ColumnReader<T>>(slot->branch_config, name);
      auto emplace_result = slot->column_readers.emplace(name, new_column_reader);
      result.template emplace_back(emplace_result.first->second->dataPtr());
    }
    return result;
  }

 private:
  struct Slot {
    BranchConfig branch_config;
    std::unique_ptr<TFile> file{nullptr};
    TTree* tree{nullptr};
    T* entry_ptr{nullptr};
    std::map<std::string, std::shared_ptr<ColumnReader<T>>> column_readers;
    ULong64_t ientry{0ul};
  };

  std::shared_ptr<Slot> makeSlot() const {
    auto new_slot = std::make_shared<Slot>();
    new_slot->file.reset(TFile::Open(filename_.c_str(), "READ"));
    new_slot->tree = new_slot->file->template Get<TTree>(tree_name_.c_str());
    new_slot->tree->template SetBranchAddress(branch_name_.c_str(), std::addressof(new_slot->entry_ptr));

    auto configuration = new_slot->file->template Get<Configuration>("Configuration");
    new_slot->branch_config = configuration->GetBranchConfig(branch_name_);
    return new_slot;
  }

  std::vector<std::shared_ptr<Slot>> slots_;
};

template<typename T>
class AnalysisTreeRDFImplT<AnalysisTree::Detector<T>> :
    public AnalysisTreeRDFBase,
    public ROOT::RDF::RDataSource {
 public:

  typedef Detector<T> detector_type;
  typedef T channel_type;

  struct Slot {
    BranchConfig config;
    std::unique_ptr<TFile> file;
    TTree *tree{nullptr};
    detector_type *detector_ptr{nullptr};
    std::map<std::string, std::shared_ptr<ColumnReader<channel_type>>> column_readers;

    bool is_initialized{false};
    ULong64_t event_no{0ul};
    ULong64_t entry_offset{0ul};
    size_t n_channels{0ul};
  };
  typedef std::shared_ptr<Slot> SlotPtr;


  AnalysisTreeRDFImplT(const std::string& Filename, const std::string& TreeName, const std::string& BranchName) : AnalysisTreeRDFBase(Filename, TreeName, BranchName) {
    for (auto &transitive_field: getTransitiveFields<channel_type>()) {
      auto field_pos = std::find(begin(column_names_), end(column_names_), transitive_field);
      if (field_pos != end(column_names_)) {
        column_names_.erase(field_pos);
      }
    }
  }

  void SetNSlots(unsigned int nSlots) override {
    for (int i = 0; i < nSlots; ++i) {
      slots_.emplace_back(makeSlot());
    }
  }
  const std::vector<std::string>& GetColumnNames() const override {
    return column_names_;
  }
  bool HasColumn(std::string_view View) const override {
    return HasColumnImpl(View);
  }
  std::string GetTypeName(std::string_view View) const override {
    return GetTypeNameImpl(View);
  }
  std::vector<std::pair<ULong64_t, ULong64_t>> GetEntryRanges() override {
    std::vector<std::pair<ULong64_t, ULong64_t>> result;

    auto event_no = slots_.back()->is_initialized? slots_.back()->event_no + 1 : 0ul;
    auto entry_offset = slots_.back()->is_initialized?
                                                      slots_.back()->entry_offset +
                                                      slots_.back()->n_channels: 0ul;
    if (event_no >= slots_.front()->tree->GetEntries())
      return {};

    for (SlotPtr &slot_ptr: slots_) {
      Slot &slot = *slot_ptr;
      assert(event_no < slot.tree->GetEntries());
      slot.tree->GetEntry(event_no);

      slot.event_no = event_no;
      slot.n_channels = slot.detector_ptr->GetNumberOfChannels();
      slot.entry_offset = entry_offset;
      slot.is_initialized = true;

      result.emplace_back(entry_offset, slot.entry_offset + slot.n_channels - 1);

      event_no+=1;
      entry_offset += slot.n_channels;
    }
    return result;
  }
  bool SetEntry(unsigned int islot, ULong64_t entry) override {
    Slot& slot = *(slots_[islot]);
    auto i_channel = entry - slot.entry_offset;
    if (i_channel < slot.n_channels) {
      for (auto &reader : slot.column_readers) {
        reader.second->update(std::addressof(slot.detector_ptr->Channel(i_channel)));
      }
      return true;
    }
    return false;
  }

 protected:
  Record_t GetColumnReadersImpl(std::string_view name, const std::type_info& Info) override {
    Record_t result;
    result.reserve(slots_.size());
    for (auto& slot_ptr : slots_) {
      Slot& slot = *slot_ptr;
      auto new_column_reader = std::make_shared<ColumnReader<channel_type>>(slot.config, name);
      auto emplace_result = slot.column_readers.emplace(name, new_column_reader);
      result.template emplace_back(emplace_result.first->second->dataPtr());
    }
    return result;
  }



  std::shared_ptr<Slot> makeSlot() {
    auto slot = std::make_shared<Slot>();
    slot->file.reset(TFile::Open(filename_.c_str(), "read"));
    slot->tree = slot->file->template Get<TTree>(tree_name_.c_str());
    slot->tree->SetBranchAddress(branch_name_.c_str(), std::addressof(slot->detector_ptr));

    auto configuration = slot->file->template Get<Configuration>("Configuration");
    slot->config = configuration->GetBranchConfig(branch_name_);
    return slot;
  }

  std::vector<std::shared_ptr<Slot>> slots_;
};

}// namespace Impl

}// namespace AnalysisTree
