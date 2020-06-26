set(SOURCES
  SimpleCut.cpp
  Cuts.cpp
  Field.cpp
  Variable.cpp
  FillTask.cpp
  TreeReader.cpp
  TaskManager.cpp
  VarManager.cpp
  BranchReader.cpp
)

message(STATUS "CMAKE_PROJECT_NAME ${CMAKE_PROJECT_NAME}")

string(REPLACE ".cpp" ".hpp" HEADERS "${SOURCES}")

include_directories(${CMAKE_SOURCE_DIR}/core ${CMAKE_CURRENT_SOURCE_DIR})
add_library(AnalysisTreeInfra SHARED ${SOURCES} G__AnalysisTreeInfra.cxx)
        
ROOT_GENERATE_DICTIONARY(G__AnalysisTreeInfra ${HEADERS} LINKDEF AnalysisTreeInfraLinkDef.h)
target_link_libraries(AnalysisTreeInfra ${ROOT_LIBRARIES} AnalysisTreeBase)

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
