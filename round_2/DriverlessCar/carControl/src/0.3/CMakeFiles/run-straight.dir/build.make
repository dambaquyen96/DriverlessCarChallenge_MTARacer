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
include CMakeFiles/run-straight.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/run-straight.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/run-straight.dir/flags.make

CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.o: CMakeFiles/run-straight.dir/flags.make
CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.o: main_pid/runStraight.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.o -c /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/main_pid/runStraight.cpp

CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/main_pid/runStraight.cpp > CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.i

CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/main_pid/runStraight.cpp -o CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.s

CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.o.requires:
.PHONY : CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.o.requires

CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.o.provides: CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.o.requires
	$(MAKE) -f CMakeFiles/run-straight.dir/build.make CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.o.provides.build
.PHONY : CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.o.provides

CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.o.provides.build: CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.o

# Object files for target run-straight
run__straight_OBJECTS = \
"CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.o"

# External object files for target run-straight
run__straight_EXTERNAL_OBJECTS =

bin/Release/run-straight: CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.o
bin/Release/run-straight: CMakeFiles/run-straight.dir/build.make
bin/Release/run-straight: bin/Release/libi2c-pwm.a
bin/Release/run-straight: bin/Release/libuart.a
bin/Release/run-straight: CMakeFiles/run-straight.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable bin/Release/run-straight"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/run-straight.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/run-straight.dir/build: bin/Release/run-straight
.PHONY : CMakeFiles/run-straight.dir/build

CMakeFiles/run-straight.dir/requires: CMakeFiles/run-straight.dir/main_pid/runStraight.cpp.o.requires
.PHONY : CMakeFiles/run-straight.dir/requires

CMakeFiles/run-straight.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/run-straight.dir/cmake_clean.cmake
.PHONY : CMakeFiles/run-straight.dir/clean

CMakeFiles/run-straight.dir/depend:
	cd /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3 /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3 /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3 /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3 /home/ubuntu/Downloads/Lou_pid_30_4_vong2/DriverlessCar/carControl/src/0.3/CMakeFiles/run-straight.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/run-straight.dir/depend

