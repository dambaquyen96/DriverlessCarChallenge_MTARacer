
cmake_minimum_required(VERSION 2.8)

project( LoadImage )

#set( OpenCV_DIR /home/nguyen/install/opencv/3.1.0/release/share/OpenCV)

find_package( OpenCV REQUIRED )

set( OPENNI2_INCLUDE /home/ubuntu/Downloads/2-Linux/OpenNI-Linux-Arm-2.3/Include)

set( OPENNI2_REDIST  /home/ubuntu/Downloads/2-Linux/OpenNI-Linux-Arm-2.3/Redist)

include_directories( ${OPENNI2_INCLUDE} )

link_directories ( ${OPENNI2_REDIST} )

set( ex1 test-openni-opencv)

set( source_files1
    main.cpp
)

add_executable( ${ex1} ${source_files1} )

target_link_libraries( ${ex1} ${OpenCV_LIBS} OpenNI2)
