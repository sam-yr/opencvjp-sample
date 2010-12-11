include_directories(${OPENCV_INCLUDE_DIRS})
find_path(EIGEN2_INCLUDE_PATH "Eigen/Core"
            PATHS "/usr/local/include/eigen2" "/opt/include/eigen2" "/usr/include/eigen2"
            DOC "The path to Eigen2 headers")
if(EIGEN2_INCLUDE_PATH)
  include_directories(${EIGEN2_INCLUDE_PATH})
endif()
aux_source_directory("." SOURCES)

foreach(P ${SOURCES})
  string(REPLACE "./" "" SOURCE ${P})
  string(REPLACE ".cpp" "" EXEFILE ${SOURCE})
  if(NOT ${SOURCE} MATCHES "^[.](/[.])*[^/]+$")
    add_executable(${EXEFILE} ${SOURCE})
    target_link_libraries(${EXEFILE} ${OPENCV_LIBRARIES} ${OPENCV_LIBRARIES_DEBUG})
  endif()
endforeach()


