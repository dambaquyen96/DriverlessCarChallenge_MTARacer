
cmake_minimum_required(VERSION 2.8)

project( LaneDetection )

set( source_files
    msac/MSAC.cpp
    msac/MSAC.h
    msac/errorNIETO.h
    msac/errorNIETO.cpp
    msac/lmmin.h
    msac/lmmin.cpp
    api_lane_detection.h
    #api_lane_detection.cpp
    api_lane_detection_2016_07_26.cpp
)

set( lib_name vanishing-point)
add_library( ${lib_name} ${source_files} )
target_link_libraries( ${lib_name} ${OpenCV_LIBS})


set( test_exec_name test-vanishing-point)
add_executable( ${test_exec_name}  test_lane_detection.cpp )

target_link_libraries( ${test_exec_name} ${lib_name} ${OpenCV_LIBS})
