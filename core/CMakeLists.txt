
set(SOURCES
  IndexedObject.cpp
  Track.cpp
  Detector.cpp
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
list(APPEND HEADERS "Constants.hpp")

add_library(AnalysisTreeBase SHARED ${SOURCES} G__AnalysisTreeBase.cxx)
ROOT_GENERATE_DICTIONARY(G__AnalysisTreeBase ${HEADERS} LINKDEF AnalysisTreeCoreLinkDef.h)
target_link_libraries(AnalysisTreeBase ${ROOT_LIBRARIES})

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
