
find_package(Boost QUIET)

set(SOURCES
  SimpleCut.cpp
  Cuts.cpp
  Field.cpp
  Variable.cpp
  Task.cpp
  AnalysisTask.cpp
  TaskManager.cpp
  PlainTreeFiller.cpp
  Chain.cpp

  Branch.cpp
  BranchChannel.cpp
#  ValueHolder.cpp
        )

if(CMAKE_CXX_STANDARD GREATER_EQUAL 17 OR Boost_FOUND)
    list(APPEND SOURCES "AnalysisTask.cpp" "BranchReader.cpp" "AnalysisEntry.cpp")
endif()

if (AnalysisTree_BUILD_EXPERIMENTAL)
    list(APPEND SOURCES BranchView.cpp)
endif ()

message(STATUS "CMAKE_PROJECT_NAME ${CMAKE_PROJECT_NAME}")

string(REPLACE ".cpp" ".hpp" HEADERS "${SOURCES}")
list(APPEND HEADERS "VariantMagic.hpp" "ToyMC.hpp" "Utils.hpp")

include_directories(${CMAKE_SOURCE_DIR}/core ${CMAKE_CURRENT_SOURCE_DIR} $<$<BOOL:${Boost_FOUND}>:${Boost_INCLUDE_DIRS}>)
add_library(AnalysisTreeInfra SHARED ${SOURCES} G__AnalysisTreeInfra.cxx)
target_compile_definitions(AnalysisTreeInfra PUBLIC
        $<$<BOOL:${Boost_FOUND}>:ANALYSISTREE_BOOST_FOUND>)

ROOT_GENERATE_DICTIONARY(G__AnalysisTreeInfra ${HEADERS}
        LINKDEF AnalysisTreeInfraLinkDef.h
        OPTIONS
            -I${CMAKE_BINARY_DIR}/include
            $<$<BOOL:${Boost_FOUND}>:-DANALYSISTREE_BOOST_FOUND>
        )
target_link_libraries(AnalysisTreeInfra
        PUBLIC
            ${ROOT_LIBRARIES}
            AnalysisTreeBase
            $<$<BOOL:${Boost_FOUND}>:${Boost_LIBRARIES}>
        )

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
            Variable.test.cpp
            Cuts.test.cpp
            Field.test.cpp
            SimpleCut.test.cpp
            PlainTreeFiller.test.cpp
            AnalysisTask.test.cpp
            Branch.test.cpp
            )

    if (AnalysisTree_BUILD_EXPERIMENTAL)
        list(APPEND TEST_SOURCES BranchView.test.cpp)
    endif()

    add_executable(AnalysisTreeInfra_UnitTests ${TEST_SOURCES})
    target_link_libraries(AnalysisTreeInfra_UnitTests PRIVATE AnalysisTreeInfra gtest_main)
    gtest_discover_tests(AnalysisTreeInfra_UnitTests TEST_PREFIX AnalysisTreeInfra_)
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
