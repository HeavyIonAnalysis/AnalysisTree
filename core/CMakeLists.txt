
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
list(APPEND HEADERS "Constants.hpp" "ToyMC.hpp")

add_library(AnalysisTreeBase SHARED ${SOURCES} G__AnalysisTreeBase.cxx)
ROOT_GENERATE_DICTIONARY(G__AnalysisTreeBase ${HEADERS} LINKDEF AnalysisTreeCoreLinkDef.h)
target_link_libraries(AnalysisTreeBase ${ROOT_LIBRARIES} EG)
add_custom_command(TARGET AnalysisTreeBase PRE_BUILD
        COMMAND ${CMAKE_COMMAND} ARGS -E make_directory ${CMAKE_BINARY_DIR}/include/AnalysisTree
        COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${HEADERS} ${CMAKE_BINARY_DIR}/include/AnalysisTree
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(AnalysisTreeBase
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
            $<INSTALL_INTERFACE:include>
        )


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

install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PCM_FILE_NAME}.rootmap"
        DESTINATION
        lib
        OPTIONAL
)


