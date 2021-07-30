#include "Task.hpp"
#include "TaskManager.hpp"

namespace AnalysisTree {

ANALYSISTREE_ATTR_NODISCARD bool Task::IsGoodEvent(const Chain& t) const {
  if (!event_cuts_) return true;
  auto br_name = event_cuts_->GetBranches().begin();
  // Here EventHeader expected
  //TODO throw exeption otherwise
  const auto cut = apply_cut(0, event_cuts_);
  const auto data_ptr = t.GetPointerToBranch(*br_name);
  return ANALYSISTREE_UTILS_VISIT(cut, data_ptr);
}

void Task::PreInit() {
  const auto* const man = TaskManager::GetInstance();
  config_ = man->GetConfig();
  data_header_ = man->GetDataHeader();

  if (event_cuts_) {
    event_cuts_->Init(*config_);
  }
}

}// namespace AnalysisTree