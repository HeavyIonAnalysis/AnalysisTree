//
// Created by eugene on 17/10/2021.
//

#pragma once

#include <ROOT/RDataSource.hxx>
#include <TFile.h>
#include <TTree.h>
#include <map>
#include <utility>

#include <AnalysisTree/BranchReader.hpp>
#include <AnalysisTree/Configuration.hpp>
#include <AnalysisTree/EventHeader.hpp>
#include <EventHeader.hpp>

namespace AnalysisTree {

namespace Impl {

template<typename T>
struct ColumnReader {
  ColumnReader(BranchConfig& config, std::string_view field_name) {
    field_id = config.GetFieldId(std::string(field_name));
    field_type = config.GetFieldType(std::string(field_name));
  }

  void update(T* ptr) {
    data = Data(ptr, field_id, field_type);
  }

  void* dataPtr() {
    return std::addressof(data);
  }

  Integer_t field_id{};
  Types field_type{};

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
  AnalysisTreeRDFImplT(const std::string& Filename, const std::string& TreeName, const std::string& BranchName) : AnalysisTreeRDFBase(Filename, TreeName, BranchName) {}

  void SetNSlots(unsigned int nSlots) override {
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
    return std::vector<std::pair<ULong64_t, ULong64_t>>();
  }
  bool SetEntry(unsigned int slot, ULong64_t entry) override {
    return false;
  }

 protected:
  Record_t GetColumnReadersImpl(std::string_view name, const std::type_info& Info) override {
    return ROOT::RDF::RDataSource::Record_t();
  }
};

}// namespace Impl

}// namespace AnalysisTree
