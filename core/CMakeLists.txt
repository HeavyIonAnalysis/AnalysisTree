
set(SOURCES
  IndexedObject.cpp
  Track.cpp
  Hit.cpp
  EventHeader.cpp
  Module.cpp
  Configuration.cpp
  BranchConfig.cpp
  Container.cpp
  DataHeader.cpp
  Matching.cpp
  Particle.cpp)

string(REPLACE ".cpp" ".hpp" HEADERS "${SOURCES}")
list(APPEND HEADERS "Constants.hpp" "Detector.hpp")

add_library(AnalysisTreeBase SHARED ${SOURCES} G__AnalysisTreeBase.cxx)
ROOT_GENERATE_DICTIONARY(G__AnalysisTreeBase ${HEADERS} LINKDEF AnalysisTreeCoreLinkDef.h)
target_link_libraries(AnalysisTreeBase ${ROOT_LIBRARIES} EG)

add_custom_target(AnalysisTreeBaseCopyHeaders ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/../include/AnalysisTree
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/../include/AnalysisTree
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_dependencies(AnalysisTreeBase AnalysisTreeBaseCopyHeaders)


target_include_directories(AnalysisTreeBase
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include/AnalysisTree>
            $<INSTALL_INTERFACE:include>
        )

if(AnalysisTree_BUILD_TESTS)
    set(TEST_SOURCES
            BranchConfig.test.cpp
            Configuration.test.cpp
            Container.test.cpp
            DataHeader.test.cpp
            Detector.test.cpp
            EventHeader.test.cpp
            IndexedObject.test.cpp
            Matching.test.cpp
            Module.test.cpp
            Particle.test.cpp
            Track.test.cpp
            )

    add_executable(AnalysisTreeBase_UnitTests ${TEST_SOURCES})
    target_link_libraries(AnalysisTreeBase_UnitTests  PRIVATE AnalysisTreeBase gtest_main)
    gtest_discover_tests(AnalysisTreeBase_UnitTests TEST_PREFIX AnalysisTreeBase_ )
endif()

install(TARGETS AnalysisTreeBase EXPORT AnalysisTreeTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include/AnalysisTree
        )

install(
        FILES
        ${HEADERS}
        DESTINATION
        include/AnalysisTree
        COMPONENT
        Devel
)

set(PCM_FILE_NAME libAnalysisTreeBase)

install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PCM_FILE_NAME}_rdict.pcm"
        DESTINATION
        lib
        OPTIONAL
)

install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PCM_FILE_NAME}.rootmap"
        DESTINATION
        lib
        OPTIONAL
)


