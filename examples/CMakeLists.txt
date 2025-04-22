set(EXECUTABLES
        example
        run_read_task
        run_write_task
)

set(SOURCES
        UserTaskWrite.cpp
        UserTaskRead.cpp
        )

message(STATUS "CMAKE_PROJECT_NAME ${CMAKE_PROJECT_NAME}")

string(REPLACE ".cpp" ".hpp" HEADERS "${SOURCES}")

include_directories(${CMAKE_SOURCE_DIR}/core ${CMAKE_SOURCE_DIR}/infra ${CMAKE_CURRENT_SOURCE_DIR} $<$<BOOL:${Boost_FOUND}>:${Boost_INCLUDE_DIRS}>)
add_library(AnalysisTreeUser SHARED ${SOURCES})
target_compile_definitions(AnalysisTreeUser PUBLIC
        $<$<BOOL:${Boost_FOUND}>:ANALYSISTREE_BOOST_FOUND>)

ROOT_GENERATE_DICTIONARY(G__AnalysisTreeUser ${HEADERS}
        LINKDEF AnalysisTreeUserLinkDef.h
        OPTIONS
        -I${CMAKE_BINARY_DIR}/include
        $<$<BOOL:${Boost_FOUND}>:-DANALYSISTREE_BOOST_FOUND>
        MODULE AnalysisTreeUser
        )
target_link_libraries(AnalysisTreeUser
        PUBLIC
        ${ROOT_LIBRARIES}
        AnalysisTreeBase AnalysisTreeInfra
        $<$<BOOL:${Boost_FOUND}>:${Boost_LIBRARIES}>
        )

include(${ROOT_USE_FILE})
include_directories(${ROOT_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS} ${AnalysisTree_INCLUDE_DIR})

install(TARGETS AnalysisTreeUser EXPORT AnalysisTreeTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include/AnalysisTree
)

install(FILES
        ${HEADERS}
        DESTINATION
        include/AnalysisTree
        COMPONENT
        Devel
)

set(PCM_FILE_NAME libAnalysisTreeUser)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PCM_FILE_NAME}_rdict.pcm"
        DESTINATION
        lib
        OPTIONAL
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PCM_FILE_NAME}.rootmap"
        DESTINATION
        lib
        OPTIONAL
)

foreach(EXE ${EXECUTABLES})
        add_executable(${EXE} ${EXE}.cpp)
        target_link_libraries(${EXE} AnalysisTreeBase AnalysisTreeInfra AnalysisTreeUser ${ROOT_LIBRARIES} EG)
        install (TARGETS ${EXE} RUNTIME DESTINATION bin)
        install (FILES ${EXE}.cpp DESTINATION share)
endforeach()
