set(SOURCES
        UserTaskWrite.cpp
        UserTaskRead.cpp
        )

message(STATUS "CMAKE_PROJECT_NAME ${CMAKE_PROJECT_NAME}")

string(REPLACE ".cpp" ".hpp" HEADERS "${SOURCES}")

include_directories(${CMAKE_SOURCE_DIR}/core ${CMAKE_SOURCE_DIR}/infra ${CMAKE_CURRENT_SOURCE_DIR} $<$<BOOL:${Boost_FOUND}>:${Boost_INCLUDE_DIRS}>)
add_library(AnalysisTreeUser SHARED ${SOURCES} G__AnalysisTreeUser.cxx)
target_compile_definitions(AnalysisTreeUser PUBLIC
        $<$<BOOL:${Boost_FOUND}>:ANALYSISTREE_BOOST_FOUND>)

ROOT_GENERATE_DICTIONARY(G__AnalysisTreeUser ${HEADERS}
        LINKDEF AnalysisTreeUserLinkDef.h
        OPTIONS
        -I${CMAKE_BINARY_DIR}/include
        $<$<BOOL:${Boost_FOUND}>:-DANALYSISTREE_BOOST_FOUND>
        )
target_link_libraries(AnalysisTreeUser
        PUBLIC
        ${ROOT_LIBRARIES}
        AnalysisTreeBase AnalysisTreeInfra
        $<$<BOOL:${Boost_FOUND}>:${Boost_LIBRARIES}>
        )

include(${ROOT_USE_FILE})
include_directories(${ROOT_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS} ${AnalysisTree_INCLUDE_DIR})

add_executable(example example.cpp)
target_link_libraries(example AnalysisTreeBase AnalysisTreeInfra AnalysisTreeUser ${ROOT_LIBRARIES} EG)







#cmake_minimum_required(VERSION 3.5)
#project(AnalysisTreeTask CXX)
#set(PROJECT_VERSION 1.0)
#
#if(NOT DEFINED CMAKE_BUILD_TYPE)
#    set(CMAKE_BUILD_TYPE RELEASE)
#endif()
#
#if(NOT DEFINED CMAKE_CXX_STANDARD)
#    set(CMAKE_CXX_STANDARD 17)
#endif()
#
## in DEBUG mode make verbose Makefile
#if (CMAKE_BUILD_TYPE MATCHES DEBUG)
#    set(CMAKE_VERBOSE_MAKEFILE ON)
#endif ()
#
#if(CMAKE_CXX_STANDARD LESS 17)
#    find_package(Boost)
#endif()
#
#if(Boost_FOUND)
#    message(STATUS "Boost version ${Boost_VERSION_STRING} is found!")
#    message(STATUS "Boost include dir: ${Boost_INCLUDE_DIRS}")
#    add_definitions(-DUSEBOOST)
#endif()
#
#find_package(ROOT REQUIRED)
#list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
#find_package(AnalysisTree REQUIRED)
#
#message(STATUS "${ROOT_INCLUDE_DIRS} ${AnalysisTree_INCLUDE_DIR}")
#message(STATUS "${ROOT_LIBRARIES} ${AnalysisTree_INCLUDE_DIR}")
