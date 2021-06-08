
find_package(Boost QUIET)

set(SOURCES
  SimpleCut.cpp
  Cuts.cpp
  Field.cpp
  Variable.cpp
  FillTask.cpp
  TreeReader.cpp
  TaskManager.cpp
  PlainTreeFiller.cpp
        )

if(CMAKE_CXX_STANDARD GREATER_EQUAL 17 OR Boost_FOUND)
    list(APPEND SOURCES "VarManager.cpp" "BranchReader.cpp" "VarManagerEntry.cpp")
endif()

if (AnalysisTree_BUILD_EXPERIMENTAL)
    list(APPEND SOURCES BranchView.cpp)
endif ()

message(STATUS "CMAKE_PROJECT_NAME ${CMAKE_PROJECT_NAME}")

string(REPLACE ".cpp" ".hpp" HEADERS "${SOURCES}")
list(APPEND HEADERS
        VariantMagic.hpp
        Utils.hpp
        )

include_directories(${CMAKE_SOURCE_DIR}/core ${CMAKE_CURRENT_SOURCE_DIR})
add_library(AnalysisTreeInfraVersion1 SHARED ${SOURCES} G__AnalysisTreeInfraVersion1.cxx)
target_compile_definitions(AnalysisTreeInfraVersion1 PUBLIC
        $<$<BOOL:${Boost_FOUND}>:ANALYSISTREE_BOOST_FOUND>)
        
ROOT_GENERATE_DICTIONARY(G__AnalysisTreeInfraVersion1 ${HEADERS}
        LINKDEF AnalysisTreeInfraLinkDef.h
        OPTIONS
            -I${CMAKE_BINARY_DIR}/include
            $<$<BOOL:${Boost_FOUND}>:-DANALYSISTREE_BOOST_FOUND>
        )
target_link_libraries(AnalysisTreeInfraVersion1
        PUBLIC
            ${ROOT_LIBRARIES}
            AnalysisTreeBase
            $<$<BOOL:${Boost_FOUND}>:${Boost_LIBRARIES}>
        )

foreach(HEADER ${HEADERS})
    set(HEADER_ABS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER})
    set(HEADERS_BINARY_DIR ${CMAKE_BINARY_DIR}/include/AnalysisTree)
    set(HEADER_SYMLINK_PATH ${HEADERS_BINARY_DIR}/${HEADER})
    add_custom_command(TARGET AnalysisTreeInfraVersion1 PRE_BUILD
        COMMAND ${CMAKE_COMMAND} ARGS -E make_directory ${HEADERS_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} ARGS -E create_symlink ${HEADER_ABS_PATH} ${HEADER_SYMLINK_PATH}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
endforeach()
target_include_directories(AnalysisTreeInfraVersion1
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
            PlainTreeFiller.test.cpp
            SimpleCut.test.cpp
            VarManager.test.cpp
            )

    if (AnalysisTree_BUILD_EXPERIMENTAL)
        list(APPEND TEST_SOURCES BranchView.test.cpp)
    endif()

    add_executable(AnalysisTreeInfraVersion1_UnitTests ${TEST_SOURCES})
    target_link_libraries(AnalysisTreeInfraVersion1_UnitTests PRIVATE AnalysisTreeInfraVersion1 gtest_main)
    gtest_discover_tests(AnalysisTreeInfraVersion1_UnitTests TEST_PREFIX AnalysisTreeInfraVersion1_)
endif()

install(TARGETS AnalysisTreeInfraVersion1 EXPORT AnalysisTreeTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include/AnalysisTree/Version1
        )

install(
        FILES
        ${HEADERS}
        DESTINATION
        include/AnalysisTree/Version1
        COMPONENT
        Devel
)

set(PCM_FILE_NAME libAnalysisTreeInfraVersion1)

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
