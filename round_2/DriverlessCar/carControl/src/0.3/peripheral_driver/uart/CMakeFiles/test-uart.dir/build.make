# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3

# Include any dependencies generated for this target.
include peripheral_driver/uart/CMakeFiles/test-uart.dir/depend.make

# Include the progress variables for this target.
include peripheral_driver/uart/CMakeFiles/test-uart.dir/progress.make

# Include the compile flags for this target's objects.
include peripheral_driver/uart/CMakeFiles/test-uart.dir/flags.make

peripheral_driver/uart/CMakeFiles/test-uart.dir/test_uart.cpp.o: peripheral_driver/uart/CMakeFiles/test-uart.dir/flags.make
peripheral_driver/uart/CMakeFiles/test-uart.dir/test_uart.cpp.o: peripheral_driver/uart/test_uart.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object peripheral_driver/uart/CMakeFiles/test-uart.dir/test_uart.cpp.o"
	cd /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/peripheral_driver/uart && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/test-uart.dir/test_uart.cpp.o -c /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/peripheral_driver/uart/test_uart.cpp

peripheral_driver/uart/CMakeFiles/test-uart.dir/test_uart.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-uart.dir/test_uart.cpp.i"
	cd /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/peripheral_driver/uart && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/peripheral_driver/uart/test_uart.cpp > CMakeFiles/test-uart.dir/test_uart.cpp.i

peripheral_driver/uart/CMakeFiles/test-uart.dir/test_uart.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-uart.dir/test_uart.cpp.s"
	cd /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/peripheral_driver/uart && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/peripheral_driver/uart/test_uart.cpp -o CMakeFiles/test-uart.dir/test_uart.cpp.s

peripheral_driver/uart/CMakeFiles/test-uart.dir/test_uart.cpp.o.requires:
.PHONY : peripheral_driver/uart/CMakeFiles/test-uart.dir/test_uart.cpp.o.requires

peripheral_driver/uart/CMakeFiles/test-uart.dir/test_uart.cpp.o.provides: peripheral_driver/uart/CMakeFiles/test-uart.dir/test_uart.cpp.o.requires
	$(MAKE) -f peripheral_driver/uart/CMakeFiles/test-uart.dir/build.make peripheral_driver/uart/CMakeFiles/test-uart.dir/test_uart.cpp.o.provides.build
.PHONY : peripheral_driver/uart/CMakeFiles/test-uart.dir/test_uart.cpp.o.provides

peripheral_driver/uart/CMakeFiles/test-uart.dir/test_uart.cpp.o.provides.build: peripheral_driver/uart/CMakeFiles/test-uart.dir/test_uart.cpp.o

# Object files for target test-uart
test__uart_OBJECTS = \
"CMakeFiles/test-uart.dir/test_uart.cpp.o"

# External object files for target test-uart
test__uart_EXTERNAL_OBJECTS =

bin/Release/test-uart: peripheral_driver/uart/CMakeFiles/test-uart.dir/test_uart.cpp.o
bin/Release/test-uart: peripheral_driver/uart/CMakeFiles/test-uart.dir/build.make
bin/Release/test-uart: bin/Release/libuart.a
bin/Release/test-uart: peripheral_driver/uart/CMakeFiles/test-uart.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../../bin/Release/test-uart"
	cd /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/peripheral_driver/uart && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test-uart.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
peripheral_driver/uart/CMakeFiles/test-uart.dir/build: bin/Release/test-uart
.PHONY : peripheral_driver/uart/CMakeFiles/test-uart.dir/build

peripheral_driver/uart/CMakeFiles/test-uart.dir/requires: peripheral_driver/uart/CMakeFiles/test-uart.dir/test_uart.cpp.o.requires
.PHONY : peripheral_driver/uart/CMakeFiles/test-uart.dir/requires

peripheral_driver/uart/CMakeFiles/test-uart.dir/clean:
	cd /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/peripheral_driver/uart && $(CMAKE_COMMAND) -P CMakeFiles/test-uart.dir/cmake_clean.cmake
.PHONY : peripheral_driver/uart/CMakeFiles/test-uart.dir/clean

peripheral_driver/uart/CMakeFiles/test-uart.dir/depend:
	cd /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3 /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/peripheral_driver/uart /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3 /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/peripheral_driver/uart /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/peripheral_driver/uart/CMakeFiles/test-uart.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : peripheral_driver/uart/CMakeFiles/test-uart.dir/depend

