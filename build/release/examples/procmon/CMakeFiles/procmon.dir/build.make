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
include examples/procmon/CMakeFiles/procmon.dir/depend.make

# Include the progress variables for this target.
include examples/procmon/CMakeFiles/procmon.dir/progress.make

# Include the compile flags for this target's objects.
include examples/procmon/CMakeFiles/procmon.dir/flags.make

examples/procmon/CMakeFiles/procmon.dir/procmon.cc.o: examples/procmon/CMakeFiles/procmon.dir/flags.make
examples/procmon/CMakeFiles/procmon.dir/procmon.cc.o: /home/wjs/code/vscode/muduo/muduo-master/examples/procmon/procmon.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wjs/code/vscode/muduo/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/procmon/CMakeFiles/procmon.dir/procmon.cc.o"
	cd /home/wjs/code/vscode/muduo/build/release/examples/procmon && g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/procmon.dir/procmon.cc.o -c /home/wjs/code/vscode/muduo/muduo-master/examples/procmon/procmon.cc

examples/procmon/CMakeFiles/procmon.dir/procmon.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/procmon.dir/procmon.cc.i"
	cd /home/wjs/code/vscode/muduo/build/release/examples/procmon && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wjs/code/vscode/muduo/muduo-master/examples/procmon/procmon.cc > CMakeFiles/procmon.dir/procmon.cc.i

examples/procmon/CMakeFiles/procmon.dir/procmon.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/procmon.dir/procmon.cc.s"
	cd /home/wjs/code/vscode/muduo/build/release/examples/procmon && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wjs/code/vscode/muduo/muduo-master/examples/procmon/procmon.cc -o CMakeFiles/procmon.dir/procmon.cc.s

examples/procmon/CMakeFiles/procmon.dir/procmon.cc.o.requires:

.PHONY : examples/procmon/CMakeFiles/procmon.dir/procmon.cc.o.requires

examples/procmon/CMakeFiles/procmon.dir/procmon.cc.o.provides: examples/procmon/CMakeFiles/procmon.dir/procmon.cc.o.requires
	$(MAKE) -f examples/procmon/CMakeFiles/procmon.dir/build.make examples/procmon/CMakeFiles/procmon.dir/procmon.cc.o.provides.build
.PHONY : examples/procmon/CMakeFiles/procmon.dir/procmon.cc.o.provides

examples/procmon/CMakeFiles/procmon.dir/procmon.cc.o.provides.build: examples/procmon/CMakeFiles/procmon.dir/procmon.cc.o


examples/procmon/CMakeFiles/procmon.dir/plot.cc.o: examples/procmon/CMakeFiles/procmon.dir/flags.make
examples/procmon/CMakeFiles/procmon.dir/plot.cc.o: /home/wjs/code/vscode/muduo/muduo-master/examples/procmon/plot.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wjs/code/vscode/muduo/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object examples/procmon/CMakeFiles/procmon.dir/plot.cc.o"
	cd /home/wjs/code/vscode/muduo/build/release/examples/procmon && g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/procmon.dir/plot.cc.o -c /home/wjs/code/vscode/muduo/muduo-master/examples/procmon/plot.cc

examples/procmon/CMakeFiles/procmon.dir/plot.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/procmon.dir/plot.cc.i"
	cd /home/wjs/code/vscode/muduo/build/release/examples/procmon && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wjs/code/vscode/muduo/muduo-master/examples/procmon/plot.cc > CMakeFiles/procmon.dir/plot.cc.i

examples/procmon/CMakeFiles/procmon.dir/plot.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/procmon.dir/plot.cc.s"
	cd /home/wjs/code/vscode/muduo/build/release/examples/procmon && g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wjs/code/vscode/muduo/muduo-master/examples/procmon/plot.cc -o CMakeFiles/procmon.dir/plot.cc.s

examples/procmon/CMakeFiles/procmon.dir/plot.cc.o.requires:

.PHONY : examples/procmon/CMakeFiles/procmon.dir/plot.cc.o.requires

examples/procmon/CMakeFiles/procmon.dir/plot.cc.o.provides: examples/procmon/CMakeFiles/procmon.dir/plot.cc.o.requires
	$(MAKE) -f examples/procmon/CMakeFiles/procmon.dir/build.make examples/procmon/CMakeFiles/procmon.dir/plot.cc.o.provides.build
.PHONY : examples/procmon/CMakeFiles/procmon.dir/plot.cc.o.provides

examples/procmon/CMakeFiles/procmon.dir/plot.cc.o.provides.build: examples/procmon/CMakeFiles/procmon.dir/plot.cc.o


# Object files for target procmon
procmon_OBJECTS = \
"CMakeFiles/procmon.dir/procmon.cc.o" \
"CMakeFiles/procmon.dir/plot.cc.o"

# External object files for target procmon
procmon_EXTERNAL_OBJECTS =

bin/procmon: examples/procmon/CMakeFiles/procmon.dir/procmon.cc.o
bin/procmon: examples/procmon/CMakeFiles/procmon.dir/plot.cc.o
bin/procmon: examples/procmon/CMakeFiles/procmon.dir/build.make
bin/procmon: lib/libmuduo_http.a
bin/procmon: lib/libmuduo_net.a
bin/procmon: lib/libmuduo_base.a
bin/procmon: examples/procmon/CMakeFiles/procmon.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wjs/code/vscode/muduo/build/release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../../bin/procmon"
	cd /home/wjs/code/vscode/muduo/build/release/examples/procmon && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/procmon.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/procmon/CMakeFiles/procmon.dir/build: bin/procmon

.PHONY : examples/procmon/CMakeFiles/procmon.dir/build

examples/procmon/CMakeFiles/procmon.dir/requires: examples/procmon/CMakeFiles/procmon.dir/procmon.cc.o.requires
examples/procmon/CMakeFiles/procmon.dir/requires: examples/procmon/CMakeFiles/procmon.dir/plot.cc.o.requires

.PHONY : examples/procmon/CMakeFiles/procmon.dir/requires

examples/procmon/CMakeFiles/procmon.dir/clean:
	cd /home/wjs/code/vscode/muduo/build/release/examples/procmon && $(CMAKE_COMMAND) -P CMakeFiles/procmon.dir/cmake_clean.cmake
.PHONY : examples/procmon/CMakeFiles/procmon.dir/clean

examples/procmon/CMakeFiles/procmon.dir/depend:
	cd /home/wjs/code/vscode/muduo/build/release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wjs/code/vscode/muduo/muduo-master /home/wjs/code/vscode/muduo/muduo-master/examples/procmon /home/wjs/code/vscode/muduo/build/release /home/wjs/code/vscode/muduo/build/release/examples/procmon /home/wjs/code/vscode/muduo/build/release/examples/procmon/CMakeFiles/procmon.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/procmon/CMakeFiles/procmon.dir/depend

