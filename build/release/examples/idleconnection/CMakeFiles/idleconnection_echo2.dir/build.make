# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_SOURCE_DIR = /home/wjs/code/vscode/muduo/muduo-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wjs/code/vscode/muduo/build/release

# Include any dependencies generated for this target.
include examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/depend.make

# Include the progress variables for this target.
include examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/progress.make

# Include the compile flags for this target's objects.
include examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/flags.make

examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.o: examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/flags.make
examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.o: /home/wjs/code/vscode/muduo/muduo-master/examples/idleconnection/sortedlist.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wjs/code/vscode/muduo/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.o"
	cd /home/wjs/code/vscode/muduo/build/release/examples/idleconnection && g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.o -c /home/wjs/code/vscode/muduo/muduo-master/examples/idleconnection/sortedlist.cc

examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.i"
	cd /home/wjs/code/vscode/muduo/build/release/examples/idleconnection && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wjs/code/vscode/muduo/muduo-master/examples/idleconnection/sortedlist.cc > CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.i

examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.s"
	cd /home/wjs/code/vscode/muduo/build/release/examples/idleconnection && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wjs/code/vscode/muduo/muduo-master/examples/idleconnection/sortedlist.cc -o CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.s

examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.o.requires:

.PHONY : examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.o.requires

examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.o.provides: examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.o.requires
	$(MAKE) -f examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/build.make examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.o.provides.build
.PHONY : examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.o.provides

examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.o.provides.build: examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.o


# Object files for target idleconnection_echo2
idleconnection_echo2_OBJECTS = \
"CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.o"

# External object files for target idleconnection_echo2
idleconnection_echo2_EXTERNAL_OBJECTS =

bin/idleconnection_echo2: examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.o
bin/idleconnection_echo2: examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/build.make
bin/idleconnection_echo2: lib/libmuduo_net.a
bin/idleconnection_echo2: lib/libmuduo_base.a
bin/idleconnection_echo2: examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wjs/code/vscode/muduo/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../bin/idleconnection_echo2"
	cd /home/wjs/code/vscode/muduo/build/release/examples/idleconnection && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/idleconnection_echo2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/build: bin/idleconnection_echo2

.PHONY : examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/build

examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/requires: examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/sortedlist.cc.o.requires

.PHONY : examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/requires

examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/clean:
	cd /home/wjs/code/vscode/muduo/build/release/examples/idleconnection && $(CMAKE_COMMAND) -P CMakeFiles/idleconnection_echo2.dir/cmake_clean.cmake
.PHONY : examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/clean

examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/depend:
	cd /home/wjs/code/vscode/muduo/build/release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wjs/code/vscode/muduo/muduo-master /home/wjs/code/vscode/muduo/muduo-master/examples/idleconnection /home/wjs/code/vscode/muduo/build/release /home/wjs/code/vscode/muduo/build/release/examples/idleconnection /home/wjs/code/vscode/muduo/build/release/examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/idleconnection/CMakeFiles/idleconnection_echo2.dir/depend

