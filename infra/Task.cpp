#include "Task.hpp"
#include "TaskManagerNew.hpp"

namespace AnalysisTree{

void Task::PreInit(){
  const auto* const man = TaskManagerNew::GetInstance();
  config_ = man->GetConfig();
  data_header_ = man->GetDataHeader();
}

}