add_executable(example example.cpp)
add_dependencies(example AnalysisTreeBase AnalysisTreeInfra)
target_link_libraries(example AnalysisTreeBase AnalysisTreeInfra ${ROOT_LIBRARIES})
