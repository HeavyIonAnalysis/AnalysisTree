cmake_minimum_required(VERSION 3.5)
project(AnalysisTreeTask CXX)
set(PROJECT_VERSION 1.0)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()

# in DEBUG mode make verbose Makefile
if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif ()

if(CMAKE_CXX_STANDARD LESS 17)
    find_package(Boost)
endif()

if(Boost_FOUND)
    message(STATUS "Boost version ${Boost_VERSION_STRING} is found!")
    message(STATUS "Boost include dir: ${Boost_INCLUDE_DIRS}")
    add_definitions(-DUSEBOOST)
endif()

find_package(ROOT REQUIRED)
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(AnalysisTree REQUIRED)

message(STATUS "${ROOT_INCLUDE_DIRS} ${AnalysisTree_INCLUDE_DIR}")
message(STATUS "${ROOT_LIBRARIES} ${AnalysisTree_INCLUDE_DIR}")

include(${ROOT_USE_FILE})
include_directories(${ROOT_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS} ${AnalysisTree_INCLUDE_DIR})

add_executable(example example.cpp)
target_link_libraries(example AnalysisTreeBase AnalysisTreeInfra ${ROOT_LIBRARIES} EG)
