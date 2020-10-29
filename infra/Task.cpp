#include "Task.hpp"
#include "TaskManager.hpp"

namespace AnalysisTree{

void Task::PreInit(){
  const auto* const man = TaskManager::GetInstance();
  config_ = man->GetConfig();
  data_header_ = man->GetDataHeader();
}

}