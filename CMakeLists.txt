# Minimum 3.5 version is determined by cmake -E copy_if_different command
# that was used to copy headers to the build directory:
# multiple files are supported since cmake 3.5
cmake_minimum_required(VERSION 3.5)
project(AnalysisTree CXX)
set(PROJECT_VERSION 1.1)

# by default build optimized code
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
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# CMake options
set(AnalysisTree_BUILD_TESTS OFF CACHE BOOL "Build tests for the AnalysisTree")
set(AnalysisTree_BUILD_EXAMPLES ON CACHE BOOL "Build examples for the AnalysisTree")
set(AnalysisTree_BUILD_EXPERIMENTAL OFF CACHE BOOL "Build experimental features for the AnalysisTree")

# Tests
include(TestsSetup)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -g -DDEBUG -D__DEBUG -Wall -Wextra")
#set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -g -DDEBUG -D__DEBUG -Wall -Wextra --coverage")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ftree-vectorize -ffast-math -DNODEBUG")
message(STATUS "Using CXX flags for ${CMAKE_BUILD_TYPE}: ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
list(APPEND CMAKE_PREFIX_PATH ${ROOTSYS})

find_package(ROOT REQUIRED)

message(STATUS "Using ROOT: ${ROOT_VERSION} <${ROOT_CONFIG}>")
include_directories(${ROOT_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} core)
include(${ROOT_USE_FILE})

add_subdirectory(core)
add_subdirectory(infra)

if(AnalysisTree_BUILD_EXAMPLES)
#    add_subdirectory(examples)
    add_executable(example examples/test.cpp)
    target_link_libraries(example AnalysisTreeBase AnalysisTreeInfra ${ROOT_LIBRARIES} EG)
endif()

#if(AnalysisTree_BUILD_TESTS)
#    include(GoogleTest)
#    enable_testing()
#    add_subdirectory(test)
#endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

export(EXPORT ${PROJECT_NAME}Targets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}Targets.cmake")

message(STATUS "${PROJECT_NAME}Targets: ${${PROJECT_NAME}Targets}")
        
set(ConfigPackageLocation lib/cmake/${PROJECT_NAME})

set(${PROJECT_NAME}_INCLUDE_DIR "include")
set(${PROJECT_NAME}_LIBRARY_DIR "lib")

configure_package_config_file(
        cmake/${PROJECT_NAME}Config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${ConfigPackageLocation}
        PATH_VARS ${PROJECT_NAME}_INCLUDE_DIR ${PROJECT_NAME}_LIBRARY_DIR
)

install(EXPORT ${PROJECT_NAME}Targets
        FILE
        ${PROJECT_NAME}Targets.cmake
        DESTINATION
        ${ConfigPackageLocation}
        )

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION
        ${ConfigPackageLocation}
        COMPONENT
        Devel
        )
