
add_executable(tests test_runner.cpp)
target_link_libraries(tests gtest AnalysisTreeInfra ${ROOT_LIBRARIES})
gtest_discover_tests(tests TEST_PREFIX new:)
