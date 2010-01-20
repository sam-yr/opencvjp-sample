include_directories(${OPENCV_INCLUDE_DIRS})
aux_source_directory("." SOURCES)

foreach(P ${SOURCES})
  string(REPLACE "./" "" SOURCE ${P})
  string(REPLACE ".cpp" "" EXEFILE ${SOURCE})
  message(${SOURCE})
  add_executable(${EXEFILE} ${SOURCE})
  target_link_libraries(${EXEFILE} ${OPENCV_LIBRARIES})
endforeach()

