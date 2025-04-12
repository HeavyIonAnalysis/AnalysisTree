//
// Created by oleksii on 09.04.25.
//

#include "GenericContainerFiller.hpp"

#include <utility>

using namespace AnalysisTree;

GenericContainerFiller::GenericContainerFiller(std::string  fileInName, std::string  treeInName) : file_in_name_(std::move(fileInName)),
                                                                                                   tree_in_name_(std::move(treeInName)) {}

void GenericContainerFiller::Run(size_t nEntries) const {
  TFile* fileIn = TFile::Open(file_in_name_.c_str(), "read");
  if(fileIn == nullptr) throw std::runtime_error("GenericContainerFiller::Run(): fileIn == nullptr");

  TTree* treeIn = fileIn->Get<TTree>(tree_in_name_.c_str());
  if(treeIn == nullptr) throw std::runtime_error("GenericContainerFiller::Run(): treeIn == nullptr");

  if(!fields_to_ignore_.empty() && !fields_to_preserve_.empty()) throw std::runtime_error("GenericContainerFiller::Run(): !fields_to_ignore_.empty() && !fields_to_preserve_.empty()");

  const size_t nTreeEntries = treeIn->GetEntries();
  const size_t nRunEntries = (nEntries<0 || nEntries>nTreeEntries) ? nTreeEntries : nEntries;

  BranchConfig branchConfig(branch_out_name_, DetType::kGeneric);
  std::vector<IndexMap> branchMap;
  std::vector<FICS> branchValues;

  auto lol = treeIn->GetListOfLeaves();
  const int nLeaves = lol->GetEntries();
  for(int iLeave=0; iLeave<nLeaves; iLeave++) {
    auto leave = lol->At(iLeave);
    const std::string fieldName = leave->GetName();
    const std::string fieldType = leave->ClassName();
    if (!fields_to_ignore_.empty() && (std::find(fields_to_ignore_.begin(), fields_to_ignore_.end(), fieldName) != fields_to_ignore_.end())) continue;
    if (!fields_to_preserve_.empty() && (std::find(fields_to_preserve_.begin(), fields_to_preserve_.end(), fieldName) == fields_to_preserve_.end())) continue;
    if (fieldType == "TLeafF") {
      branchConfig.AddField<float>(fieldName);
    } else if (fieldType == "TLeafI" || fieldType == "TLeafB" || fieldType == "TLeafS") {
      branchConfig.AddField<int>(fieldName);
    }
    branchMap.emplace_back((IndexMap){fieldName, fieldType, branchConfig.GetFieldId(fieldName)});
  }
  branchValues.resize(branchMap.size());

  Configuration config;
  config.AddBranchConfig(branchConfig);

  for(int iV=0; iV<branchValues.size(); iV++) {
    TBranch* branch = treeIn->GetBranch(branchMap.at(iV).name_.c_str());
    SetAddressFICS(branch, branchMap.at(iV), branchValues.at(iV));
  }

  GenericDetector* genericDetector = new GenericDetector(branchConfig.GetId());

  const int entrySwitchTriggerId = entry_switch_trigger_var_name_.empty() ? -999 : DetermineFieldIdByName(branchMap, entry_switch_trigger_var_name_);

  TFile* fileOut = TFile::Open(file_out_name_.c_str(), "recreate");
  TTree* treeOut = new TTree(tree_out_name_.c_str(), "Analysis Tree");
  treeOut->SetAutoSave(0);
  treeOut->Branch((branchConfig.GetName() + ".").c_str(), "AnalysisTree::GenericDetector", &genericDetector);

  int previousTriggerVar{-799};
  for(int iEntry=0; iEntry<nRunEntries; iEntry++) {
    treeIn->GetEntry(iEntry);
    const int currentTriggerVar = entrySwitchTriggerId >= 0 ? branchValues.at(entrySwitchTriggerId).int_ : previousTriggerVar;
    auto isNewATEntry = [&]() {return iEntry == 0 ||
                                   (currentTriggerVar != previousTriggerVar) ||
                                   (n_channels_per_entry_ >= 0 && iEntry % n_channels_per_entry_ == 0); };

    if(isNewATEntry()) genericDetector->ClearChannels();
    auto& channel = genericDetector->AddChannel(config.GetBranchConfig(genericDetector->GetId()));
    SetFieldsFICS(branchMap, channel, branchValues);
    if(isNewATEntry()) treeOut->Fill();
  } // iEntry

  fileOut->cd();
  config.Write("Configuration");
  treeOut->Write();
  fileOut->Close();
  fileIn->Close();
}

int GenericContainerFiller::DetermineFieldIdByName(const std::vector<IndexMap>& iMap, const std::string& name) {
  auto distance = std::distance(iMap.begin(),std::find_if(iMap.begin(), iMap.end(), [&name](const IndexMap& p) { return p.name_ == name; }));
  if(distance == iMap.size()) throw std::runtime_error("DetermineFieldIdByName(): name " + name + " is missing");
  return distance;
}

void GenericContainerFiller::SetAddressFICS(TBranch* branch, const IndexMap& imap, FICS& ficc) {
  if     (imap.field_type_ == "TLeafF") branch->SetAddress(&ficc.float_);
  else if(imap.field_type_ == "TLeafI") branch->SetAddress(&ficc.int_);
  else if(imap.field_type_ == "TLeafB") branch->SetAddress(&ficc.char_);
  else if(imap.field_type_ == "TLeafS") branch->SetAddress(&ficc.short_);
  else throw std::runtime_error("GenericContainerFiller::SetAddressFICS(): unsupported filed type " + imap.field_type_);
}

void GenericContainerFiller::SetFieldsFICS(const std::vector<IndexMap>& imap, Container& container, const std::vector<FICS>& ficc) {
  for(int iV=0; iV<ficc.size(); iV++) {
    if     (imap.at(iV).field_type_ == "TLeafF") container.SetField(ficc.at(iV).float_, imap.at(iV).index_);
    else if(imap.at(iV).field_type_ == "TLeafI") container.SetField(ficc.at(iV).int_, imap.at(iV).index_);
    else if(imap.at(iV).field_type_ == "TLeafB") container.SetField(static_cast<int>(ficc.at(iV).char_), imap.at(iV).index_);
    else if(imap.at(iV).field_type_ == "TLeafS") container.SetField(static_cast<int>(ficc.at(iV).short_), imap.at(iV).index_);
    else throw std::runtime_error("GenericContainerFiller::SetFieldsFICS(): unsupported filed type " + imap.at(iV).field_type_);
  }
}