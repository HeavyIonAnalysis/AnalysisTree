set(SOURCES
  SimpleCut.cpp
  Cuts.cpp
  Field.cpp
  Variable.cpp
  FillTask.cpp
  TreeReader.cpp
  TaskManager.cpp
  PlainTreeFiller.cpp
  BranchView.cpp
        )

if(CMAKE_CXX_STANDARD GREATER_EQUAL 17 OR Boost_FOUND)
    list(APPEND SOURCES "VarManager.cpp" "BranchReader.cpp" "VarManagerEntry.cpp")
endif()

message(STATUS "CMAKE_PROJECT_NAME ${CMAKE_PROJECT_NAME}")


string(REPLACE ".cpp" ".hpp" HEADERS "${SOURCES}")
list(APPEND HEADERS "VariantMagic.hpp")

include_directories(${CMAKE_SOURCE_DIR}/core ${CMAKE_CURRENT_SOURCE_DIR})
add_library(AnalysisTreeInfra SHARED ${SOURCES} G__AnalysisTreeInfra.cxx)
        
ROOT_GENERATE_DICTIONARY(G__AnalysisTreeInfra ${HEADERS}
        LINKDEF AnalysisTreeInfraLinkDef.h
        OPTIONS -I${CMAKE_BINARY_DIR}/include)
target_link_libraries(AnalysisTreeInfra ${ROOT_LIBRARIES} AnalysisTreeBase)

foreach(HEADER ${HEADERS})
    set(HEADER_ABS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER})
    set(HEADERS_BINARY_DIR ${CMAKE_BINARY_DIR}/include/AnalysisTree)
    set(HEADER_SYMLINK_PATH ${HEADERS_BINARY_DIR}/${HEADER})
    add_custom_command(TARGET AnalysisTreeInfra PRE_BUILD
        COMMAND ${CMAKE_COMMAND} ARGS -E make_directory ${HEADERS_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} ARGS -E create_symlink ${HEADER_ABS_PATH} ${HEADER_SYMLINK_PATH}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
endforeach()
target_include_directories(AnalysisTreeInfra
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC
            $<INSTALL_INTERFACE:include>
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        )

if(AnalysisTree_BUILD_TESTS)
    set(TEST_SOURCES
            BranchView.test.cpp
            Variable.test.cpp)

    add_executable(AnalysisTreeInfra_UnitTests ${TEST_SOURCES})
    target_link_libraries(AnalysisTreeInfra_UnitTests PRIVATE AnalysisTreeInfra gtest_main)


endif()

install(TARGETS AnalysisTreeInfra EXPORT AnalysisTreeTargets
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

set(PCM_FILE_NAME libAnalysisTreeInfra)

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
