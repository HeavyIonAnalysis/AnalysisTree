#include "Task.hpp"
#include "TaskManagerNew.hpp"

namespace AnalysisTree{

void Task::PreInit(){
  auto* man = TaskManagerNew::GetInstance();
  config_ = man->GetConfig();
  data_header_ = man->GetDataHeader();
}

}