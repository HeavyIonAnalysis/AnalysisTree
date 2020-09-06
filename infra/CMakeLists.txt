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

message(STATUS "CMAKE_PROJECT_NAME ${CMAKE_PROJECT_NAME}")

string(REPLACE ".cpp" ".hpp" HEADERS "${SOURCES}")
list(APPEND HEADERS "VariantMagic.hpp")

if(Boost_FOUND)
  include_directories(${CMAKE_SOURCE_DIR}/core ${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIR})
else()
  include_directories(${CMAKE_SOURCE_DIR}/core ${CMAKE_CURRENT_SOURCE_DIR})
endif()
add_library(AnalysisTreeInfra SHARED ${SOURCES} G__AnalysisTreeInfra.cxx)
        
ROOT_GENERATE_DICTIONARY(G__AnalysisTreeInfra ${HEADERS} LINKDEF AnalysisTreeInfraLinkDef.h)
target_link_libraries(AnalysisTreeInfra ${ROOT_LIBRARIES} AnalysisTreeBase)
add_custom_command(TARGET AnalysisTreeInfra PRE_BUILD
        COMMAND ${CMAKE_COMMAND} ARGS -E make_directory ${CMAKE_BINARY_DIR}/include/AnalysisTree
        COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${HEADERS} ${CMAKE_BINARY_DIR}/include/AnalysisTree
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
target_include_directories(AnalysisTreeInfra
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
            $<INSTALL_INTERFACE:include>
        )

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
