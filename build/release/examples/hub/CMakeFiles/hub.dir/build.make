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
include examples/hub/CMakeFiles/hub.dir/depend.make

# Include the progress variables for this target.
include examples/hub/CMakeFiles/hub.dir/progress.make

# Include the compile flags for this target's objects.
include examples/hub/CMakeFiles/hub.dir/flags.make

examples/hub/CMakeFiles/hub.dir/hub.cc.o: examples/hub/CMakeFiles/hub.dir/flags.make
examples/hub/CMakeFiles/hub.dir/hub.cc.o: /home/wjs/code/vscode/muduo/muduo-master/examples/hub/hub.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wjs/code/vscode/muduo/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/hub/CMakeFiles/hub.dir/hub.cc.o"
	cd /home/wjs/code/vscode/muduo/build/release/examples/hub && g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/hub.dir/hub.cc.o -c /home/wjs/code/vscode/muduo/muduo-master/examples/hub/hub.cc

examples/hub/CMakeFiles/hub.dir/hub.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/hub.dir/hub.cc.i"
	cd /home/wjs/code/vscode/muduo/build/release/examples/hub && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wjs/code/vscode/muduo/muduo-master/examples/hub/hub.cc > CMakeFiles/hub.dir/hub.cc.i

examples/hub/CMakeFiles/hub.dir/hub.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/hub.dir/hub.cc.s"
	cd /home/wjs/code/vscode/muduo/build/release/examples/hub && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wjs/code/vscode/muduo/muduo-master/examples/hub/hub.cc -o CMakeFiles/hub.dir/hub.cc.s

examples/hub/CMakeFiles/hub.dir/hub.cc.o.requires:

.PHONY : examples/hub/CMakeFiles/hub.dir/hub.cc.o.requires

examples/hub/CMakeFiles/hub.dir/hub.cc.o.provides: examples/hub/CMakeFiles/hub.dir/hub.cc.o.requires
	$(MAKE) -f examples/hub/CMakeFiles/hub.dir/build.make examples/hub/CMakeFiles/hub.dir/hub.cc.o.provides.build
.PHONY : examples/hub/CMakeFiles/hub.dir/hub.cc.o.provides

examples/hub/CMakeFiles/hub.dir/hub.cc.o.provides.build: examples/hub/CMakeFiles/hub.dir/hub.cc.o


examples/hub/CMakeFiles/hub.dir/codec.cc.o: examples/hub/CMakeFiles/hub.dir/flags.make
examples/hub/CMakeFiles/hub.dir/codec.cc.o: /home/wjs/code/vscode/muduo/muduo-master/examples/hub/codec.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wjs/code/vscode/muduo/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object examples/hub/CMakeFiles/hub.dir/codec.cc.o"
	cd /home/wjs/code/vscode/muduo/build/release/examples/hub && g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/hub.dir/codec.cc.o -c /home/wjs/code/vscode/muduo/muduo-master/examples/hub/codec.cc

examples/hub/CMakeFiles/hub.dir/codec.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/hub.dir/codec.cc.i"
	cd /home/wjs/code/vscode/muduo/build/release/examples/hub && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wjs/code/vscode/muduo/muduo-master/examples/hub/codec.cc > CMakeFiles/hub.dir/codec.cc.i

examples/hub/CMakeFiles/hub.dir/codec.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/hub.dir/codec.cc.s"
	cd /home/wjs/code/vscode/muduo/build/release/examples/hub && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wjs/code/vscode/muduo/muduo-master/examples/hub/codec.cc -o CMakeFiles/hub.dir/codec.cc.s

examples/hub/CMakeFiles/hub.dir/codec.cc.o.requires:

.PHONY : examples/hub/CMakeFiles/hub.dir/codec.cc.o.requires

examples/hub/CMakeFiles/hub.dir/codec.cc.o.provides: examples/hub/CMakeFiles/hub.dir/codec.cc.o.requires
	$(MAKE) -f examples/hub/CMakeFiles/hub.dir/build.make examples/hub/CMakeFiles/hub.dir/codec.cc.o.provides.build
.PHONY : examples/hub/CMakeFiles/hub.dir/codec.cc.o.provides

examples/hub/CMakeFiles/hub.dir/codec.cc.o.provides.build: examples/hub/CMakeFiles/hub.dir/codec.cc.o


# Object files for target hub
hub_OBJECTS = \
"CMakeFiles/hub.dir/hub.cc.o" \
"CMakeFiles/hub.dir/codec.cc.o"

# External object files for target hub
hub_EXTERNAL_OBJECTS =

bin/hub: examples/hub/CMakeFiles/hub.dir/hub.cc.o
bin/hub: examples/hub/CMakeFiles/hub.dir/codec.cc.o
bin/hub: examples/hub/CMakeFiles/hub.dir/build.make
bin/hub: lib/libmuduo_inspect.a
bin/hub: lib/libmuduo_http.a
bin/hub: lib/libmuduo_net.a
bin/hub: lib/libmuduo_base.a
bin/hub: examples/hub/CMakeFiles/hub.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wjs/code/vscode/muduo/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../../bin/hub"
	cd /home/wjs/code/vscode/muduo/build/release/examples/hub && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/hub.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/hub/CMakeFiles/hub.dir/build: bin/hub

.PHONY : examples/hub/CMakeFiles/hub.dir/build

examples/hub/CMakeFiles/hub.dir/requires: examples/hub/CMakeFiles/hub.dir/hub.cc.o.requires
examples/hub/CMakeFiles/hub.dir/requires: examples/hub/CMakeFiles/hub.dir/codec.cc.o.requires

.PHONY : examples/hub/CMakeFiles/hub.dir/requires

examples/hub/CMakeFiles/hub.dir/clean:
	cd /home/wjs/code/vscode/muduo/build/release/examples/hub && $(CMAKE_COMMAND) -P CMakeFiles/hub.dir/cmake_clean.cmake
.PHONY : examples/hub/CMakeFiles/hub.dir/clean

examples/hub/CMakeFiles/hub.dir/depend:
	cd /home/wjs/code/vscode/muduo/build/release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wjs/code/vscode/muduo/muduo-master /home/wjs/code/vscode/muduo/muduo-master/examples/hub /home/wjs/code/vscode/muduo/build/release /home/wjs/code/vscode/muduo/build/release/examples/hub /home/wjs/code/vscode/muduo/build/release/examples/hub/CMakeFiles/hub.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/hub/CMakeFiles/hub.dir/depend

