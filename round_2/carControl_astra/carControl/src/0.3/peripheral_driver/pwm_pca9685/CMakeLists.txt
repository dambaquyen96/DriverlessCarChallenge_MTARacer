
cmake_minimum_required(VERSION 2.8)

project( pwmDriver )

set( lib_name pca9685)

set(source_files
    pca9685.h
    pca9685.cpp
    api_control_steering_throttle.cpp
    api_control_steering_throttle.h
    api_uart.cpp
    api_uart.h
)

add_library( ${lib_name} ${source_files})

set( test_exec_name test-steering-throttle)

add_executable( ${test_exec_name} test_steering_throttle.cpp)

target_link_libraries( ${test_exec_name} ${lib_name})
