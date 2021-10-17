//
// Created by eugene on 17/10/2021.
//

#pragma once

#include <TTree.h>
#include <TFile.h>
#include <ROOT/RDataSource.hxx>
#include <utility>
#include <map>

#include <AnalysisTree/BranchReader.hpp>
#include <AnalysisTree/Configuration.hpp>
#include <AnalysisTree/EventHeader.hpp>
#include <EventHeader.hpp>

namespace AnalysisTree {

namespace Impl {

template<typename T>
struct ColumnReader;

template <>
struct ColumnReader<EventHeader> {
  std::map<std::string, void *> data_ptr_to_ptr;

  void *readData(const BranchConfig& config, const std::string &field_name, EventHeader *ptr) {
    if (data_ptr_to_ptr.find(field_name) != end(data_ptr_to_ptr)) {
      return &(data_ptr_to_ptr[field_name]);
    }
    auto field_id = config.GetFieldId(field_name);
    EventHeaderAccessor event_header_accessor(ptr);
    data_ptr_to_ptr.emplace(field_name, event_header_accessor.getFieldData(field_id));
    return &(data_ptr_to_ptr[field_name]);
  }
};

template<typename T>
class AnalysisTreeRDFImplT : public ROOT::RDF::RDataSource {

 public:
  AnalysisTreeRDFImplT(std::string  Filename, std::string  TreeName, std::string  BranchName) : filename_(std::move(Filename)), tree_name_(std::move(TreeName)), branch_name_(std::move(BranchName)) {
    column_names_ = getColumnNames();
  }

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
    return std::find(begin(column_names_), end(column_names_), View) != end(column_names_);
  }
  std::string GetTypeName(std::string_view View) const override {
    TFile f(filename_.c_str(), "read");
    auto configuration = f.template Get<Configuration>("Configuration");
    auto branch_config = configuration->GetBranchConfig(branch_name_);
    auto column_type = branch_config.GetFieldType(std::string(View));
    switch (column_type) {
      case Types::kBool :
        return "bool";
      case Types::kInteger :
        return "int";
      case Types::kFloat :
        return "float";
      default:;
    }
    return {};
  }
  std::vector<std::pair<ULong64_t, ULong64_t>> GetEntryRanges() override {
    auto last_entry = slots_.back()->ientry;
    auto total_entries = slots_.back()->tree->GetEntries();
    if (last_entry == total_entries-1) {
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
  }

 protected:
  Record_t GetColumnReadersImpl(std::string_view name, const std::type_info& Info) override {
    /* type-erased vector of pointers to pointers to column values - one per slot  */
    Record_t result;
    result.resize(slots_.size());
    for (int i_slot = 0; i_slot < slots_.size(); ++i_slot) {
      result[i_slot] = slots_[i_slot]->column_reader.readData(slots_[i_slot]->branch_config,
                                                              std::string(name),
                                                              slots_[i_slot]->entry_ptr);
    }
    return result;
  }

 private:
  struct Slot {
    BranchConfig branch_config;
    TFile *file{nullptr};
    TTree *tree{nullptr};
    T *entry_ptr{nullptr};
    ColumnReader<T> column_reader;
    ULong64_t ientry{0ul};
  };

  std::shared_ptr<Slot> makeSlot() const {
    auto new_slot = std::make_shared<Slot>();
    new_slot->file = TFile::Open(filename_.c_str(), "READ");
    new_slot->tree = new_slot->file->template Get<TTree>(tree_name_.c_str());
    new_slot->entry_ptr = new T;
    new_slot->tree->template SetBranchAddress(branch_name_.c_str(), &new_slot->entry_ptr);

    auto configuration = new_slot->file->template Get<Configuration>("Configuration");
    new_slot->branch_config = configuration->GetBranchConfig(branch_name_);
    return new_slot;
  }

  std::vector<std::string> getColumnNames() const {
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


  std::string filename_;
  std::string tree_name_;
  std::string branch_name_;

  std::vector<std::shared_ptr<Slot>> slots_;
  std::vector<std::string> column_names_;

};

}// namespace Impl

}// namespace AnalysisTree
