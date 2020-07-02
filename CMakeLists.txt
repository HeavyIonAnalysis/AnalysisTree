cmake_minimum_required(VERSION 3.0)
project(AnalysisTree CXX)
set(PROJECT_VERSION 1.1)

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

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -g -DDEBUG -D__DEBUG -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ftree-vectorize -ffast-math -DNODEBUG")
message(STATUS "Using CXX flags for ${CMAKE_BUILD_TYPE}: ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")

find_package(ROOT REQUIRED)
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

message(STATUS "Using ROOT: ${ROOT_VERSION} <${ROOT_CONFIG}>")
include_directories(${ROOT_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} core)
include(${ROOT_USE_FILE})


add_subdirectory(core)
add_subdirectory(infra)
add_subdirectory(examples)

IF (CMAKE_BUILD_TYPE MATCHES Debug)
    include(GoogleTest)
    enable_testing()
    add_subdirectory(test)
ENDIF (CMAKE_BUILD_TYPE MATCHES Debug)

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
