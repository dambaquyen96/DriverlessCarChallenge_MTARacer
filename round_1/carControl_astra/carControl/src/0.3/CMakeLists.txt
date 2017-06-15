cmake_minimum_required(VERSION 2.8)

project( CarControl-0.3 )

find_package( OpenCV REQUIRED )
set( OPENNI2_INCLUDE /home/ubuntu/Downloads/2-Linux/OpenNI-Linux-Arm-2.3/Include)
set( OPENNI2_REDIST  /home/ubuntu/Downloads/2-Linux/OpenNI-Linux-Arm-2.3/Redist)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY
               STRINGS "Debug" "Release")
endif()

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

include_directories(
      ${OpenCV_INCLUDE_DIRS}
      ${CMAKE_CURRENT_SOURCE_DIR}/stereo_vision
      ${CMAKE_CURRENT_SOURCE_DIR}/lane_detection
      ${CMAKE_CURRENT_SOURCE_DIR}/lane_detection/msac
      ${CMAKE_CURRENT_SOURCE_DIR}/peripheral_driver/i2c
      ${CMAKE_CURRENT_SOURCE_DIR}/peripheral_driver/uart
      ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

link_directories( ${LIB_FREENECT_INSTALL_DIR}/lib
                  ${LIBRARY_OUTPUT_PATH}
)

include_directories( ${OPENNI2_INCLUDE} )

link_directories ( ${OPENNI2_REDIST} )

find_package(OpenMP)
if(OPENMP_FOUND)
    message("Find OpenMP: ${OpenMP_C_FLAGS}, ${OpenMP_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

add_subdirectory(lane_detection)

add_subdirectory(peripheral_driver)

add_subdirectory(stereo_vision)

set( exec_name1  test-omp)

set( source_files1
    main_pid/main3.cpp
    utils/utils.h
    utils/utils.cpp
)
add_executable( ${exec_name1} ${source_files1} )

target_link_libraries( ${exec_name1} vanishing-point i2c-pwm uart kinect-cv2 OpenNI2)
