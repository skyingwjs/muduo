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
include examples/wordcount/CMakeFiles/wordcount_receiver.dir/depend.make

# Include the progress variables for this target.
include examples/wordcount/CMakeFiles/wordcount_receiver.dir/progress.make

# Include the compile flags for this target's objects.
include examples/wordcount/CMakeFiles/wordcount_receiver.dir/flags.make

examples/wordcount/CMakeFiles/wordcount_receiver.dir/receiver.cc.o: examples/wordcount/CMakeFiles/wordcount_receiver.dir/flags.make
examples/wordcount/CMakeFiles/wordcount_receiver.dir/receiver.cc.o: /home/wjs/code/vscode/muduo/muduo-master/examples/wordcount/receiver.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wjs/code/vscode/muduo/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/wordcount/CMakeFiles/wordcount_receiver.dir/receiver.cc.o"
	cd /home/wjs/code/vscode/muduo/build/release/examples/wordcount && g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wordcount_receiver.dir/receiver.cc.o -c /home/wjs/code/vscode/muduo/muduo-master/examples/wordcount/receiver.cc

examples/wordcount/CMakeFiles/wordcount_receiver.dir/receiver.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wordcount_receiver.dir/receiver.cc.i"
	cd /home/wjs/code/vscode/muduo/build/release/examples/wordcount && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wjs/code/vscode/muduo/muduo-master/examples/wordcount/receiver.cc > CMakeFiles/wordcount_receiver.dir/receiver.cc.i

examples/wordcount/CMakeFiles/wordcount_receiver.dir/receiver.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wordcount_receiver.dir/receiver.cc.s"
	cd /home/wjs/code/vscode/muduo/build/release/examples/wordcount && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wjs/code/vscode/muduo/muduo-master/examples/wordcount/receiver.cc -o CMakeFiles/wordcount_receiver.dir/receiver.cc.s

examples/wordcount/CMakeFiles/wordcount_receiver.dir/receiver.cc.o.requires:

.PHONY : examples/wordcount/CMakeFiles/wordcount_receiver.dir/receiver.cc.o.requires

examples/wordcount/CMakeFiles/wordcount_receiver.dir/receiver.cc.o.provides: examples/wordcount/CMakeFiles/wordcount_receiver.dir/receiver.cc.o.requires
	$(MAKE) -f examples/wordcount/CMakeFiles/wordcount_receiver.dir/build.make examples/wordcount/CMakeFiles/wordcount_receiver.dir/receiver.cc.o.provides.build
.PHONY : examples/wordcount/CMakeFiles/wordcount_receiver.dir/receiver.cc.o.provides

examples/wordcount/CMakeFiles/wordcount_receiver.dir/receiver.cc.o.provides.build: examples/wordcount/CMakeFiles/wordcount_receiver.dir/receiver.cc.o


# Object files for target wordcount_receiver
wordcount_receiver_OBJECTS = \
"CMakeFiles/wordcount_receiver.dir/receiver.cc.o"

# External object files for target wordcount_receiver
wordcount_receiver_EXTERNAL_OBJECTS =

bin/wordcount_receiver: examples/wordcount/CMakeFiles/wordcount_receiver.dir/receiver.cc.o
bin/wordcount_receiver: examples/wordcount/CMakeFiles/wordcount_receiver.dir/build.make
bin/wordcount_receiver: lib/libmuduo_net.a
bin/wordcount_receiver: lib/libmuduo_base.a
bin/wordcount_receiver: examples/wordcount/CMakeFiles/wordcount_receiver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wjs/code/vscode/muduo/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../bin/wordcount_receiver"
	cd /home/wjs/code/vscode/muduo/build/release/examples/wordcount && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/wordcount_receiver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/wordcount/CMakeFiles/wordcount_receiver.dir/build: bin/wordcount_receiver

.PHONY : examples/wordcount/CMakeFiles/wordcount_receiver.dir/build

examples/wordcount/CMakeFiles/wordcount_receiver.dir/requires: examples/wordcount/CMakeFiles/wordcount_receiver.dir/receiver.cc.o.requires

.PHONY : examples/wordcount/CMakeFiles/wordcount_receiver.dir/requires

examples/wordcount/CMakeFiles/wordcount_receiver.dir/clean:
	cd /home/wjs/code/vscode/muduo/build/release/examples/wordcount && $(CMAKE_COMMAND) -P CMakeFiles/wordcount_receiver.dir/cmake_clean.cmake
.PHONY : examples/wordcount/CMakeFiles/wordcount_receiver.dir/clean

examples/wordcount/CMakeFiles/wordcount_receiver.dir/depend:
	cd /home/wjs/code/vscode/muduo/build/release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wjs/code/vscode/muduo/muduo-master /home/wjs/code/vscode/muduo/muduo-master/examples/wordcount /home/wjs/code/vscode/muduo/build/release /home/wjs/code/vscode/muduo/build/release/examples/wordcount /home/wjs/code/vscode/muduo/build/release/examples/wordcount/CMakeFiles/wordcount_receiver.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/wordcount/CMakeFiles/wordcount_receiver.dir/depend

