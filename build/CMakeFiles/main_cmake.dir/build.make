# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/tcy/gaobingfa

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tcy/gaobingfa/build

# Include any dependencies generated for this target.
include CMakeFiles/main_cmake.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/main_cmake.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/main_cmake.dir/flags.make

CMakeFiles/main_cmake.dir/main.cc.o: CMakeFiles/main_cmake.dir/flags.make
CMakeFiles/main_cmake.dir/main.cc.o: ../main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tcy/gaobingfa/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/main_cmake.dir/main.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main_cmake.dir/main.cc.o -c /home/tcy/gaobingfa/main.cc

CMakeFiles/main_cmake.dir/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main_cmake.dir/main.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tcy/gaobingfa/main.cc > CMakeFiles/main_cmake.dir/main.cc.i

CMakeFiles/main_cmake.dir/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main_cmake.dir/main.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tcy/gaobingfa/main.cc -o CMakeFiles/main_cmake.dir/main.cc.s

CMakeFiles/main_cmake.dir/main.cc.o.requires:

.PHONY : CMakeFiles/main_cmake.dir/main.cc.o.requires

CMakeFiles/main_cmake.dir/main.cc.o.provides: CMakeFiles/main_cmake.dir/main.cc.o.requires
	$(MAKE) -f CMakeFiles/main_cmake.dir/build.make CMakeFiles/main_cmake.dir/main.cc.o.provides.build
.PHONY : CMakeFiles/main_cmake.dir/main.cc.o.provides

CMakeFiles/main_cmake.dir/main.cc.o.provides.build: CMakeFiles/main_cmake.dir/main.cc.o


CMakeFiles/main_cmake.dir/server.cc.o: CMakeFiles/main_cmake.dir/flags.make
CMakeFiles/main_cmake.dir/server.cc.o: ../server.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tcy/gaobingfa/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/main_cmake.dir/server.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main_cmake.dir/server.cc.o -c /home/tcy/gaobingfa/server.cc

CMakeFiles/main_cmake.dir/server.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main_cmake.dir/server.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tcy/gaobingfa/server.cc > CMakeFiles/main_cmake.dir/server.cc.i

CMakeFiles/main_cmake.dir/server.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main_cmake.dir/server.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tcy/gaobingfa/server.cc -o CMakeFiles/main_cmake.dir/server.cc.s

CMakeFiles/main_cmake.dir/server.cc.o.requires:

.PHONY : CMakeFiles/main_cmake.dir/server.cc.o.requires

CMakeFiles/main_cmake.dir/server.cc.o.provides: CMakeFiles/main_cmake.dir/server.cc.o.requires
	$(MAKE) -f CMakeFiles/main_cmake.dir/build.make CMakeFiles/main_cmake.dir/server.cc.o.provides.build
.PHONY : CMakeFiles/main_cmake.dir/server.cc.o.provides

CMakeFiles/main_cmake.dir/server.cc.o.provides.build: CMakeFiles/main_cmake.dir/server.cc.o


CMakeFiles/main_cmake.dir/common/common.cc.o: CMakeFiles/main_cmake.dir/flags.make
CMakeFiles/main_cmake.dir/common/common.cc.o: ../common/common.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tcy/gaobingfa/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/main_cmake.dir/common/common.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main_cmake.dir/common/common.cc.o -c /home/tcy/gaobingfa/common/common.cc

CMakeFiles/main_cmake.dir/common/common.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main_cmake.dir/common/common.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tcy/gaobingfa/common/common.cc > CMakeFiles/main_cmake.dir/common/common.cc.i

CMakeFiles/main_cmake.dir/common/common.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main_cmake.dir/common/common.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tcy/gaobingfa/common/common.cc -o CMakeFiles/main_cmake.dir/common/common.cc.s

CMakeFiles/main_cmake.dir/common/common.cc.o.requires:

.PHONY : CMakeFiles/main_cmake.dir/common/common.cc.o.requires

CMakeFiles/main_cmake.dir/common/common.cc.o.provides: CMakeFiles/main_cmake.dir/common/common.cc.o.requires
	$(MAKE) -f CMakeFiles/main_cmake.dir/build.make CMakeFiles/main_cmake.dir/common/common.cc.o.provides.build
.PHONY : CMakeFiles/main_cmake.dir/common/common.cc.o.provides

CMakeFiles/main_cmake.dir/common/common.cc.o.provides.build: CMakeFiles/main_cmake.dir/common/common.cc.o


CMakeFiles/main_cmake.dir/common/config.cc.o: CMakeFiles/main_cmake.dir/flags.make
CMakeFiles/main_cmake.dir/common/config.cc.o: ../common/config.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tcy/gaobingfa/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/main_cmake.dir/common/config.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main_cmake.dir/common/config.cc.o -c /home/tcy/gaobingfa/common/config.cc

CMakeFiles/main_cmake.dir/common/config.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main_cmake.dir/common/config.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tcy/gaobingfa/common/config.cc > CMakeFiles/main_cmake.dir/common/config.cc.i

CMakeFiles/main_cmake.dir/common/config.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main_cmake.dir/common/config.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tcy/gaobingfa/common/config.cc -o CMakeFiles/main_cmake.dir/common/config.cc.s

CMakeFiles/main_cmake.dir/common/config.cc.o.requires:

.PHONY : CMakeFiles/main_cmake.dir/common/config.cc.o.requires

CMakeFiles/main_cmake.dir/common/config.cc.o.provides: CMakeFiles/main_cmake.dir/common/config.cc.o.requires
	$(MAKE) -f CMakeFiles/main_cmake.dir/build.make CMakeFiles/main_cmake.dir/common/config.cc.o.provides.build
.PHONY : CMakeFiles/main_cmake.dir/common/config.cc.o.provides

CMakeFiles/main_cmake.dir/common/config.cc.o.provides.build: CMakeFiles/main_cmake.dir/common/config.cc.o


# Object files for target main_cmake
main_cmake_OBJECTS = \
"CMakeFiles/main_cmake.dir/main.cc.o" \
"CMakeFiles/main_cmake.dir/server.cc.o" \
"CMakeFiles/main_cmake.dir/common/common.cc.o" \
"CMakeFiles/main_cmake.dir/common/config.cc.o"

# External object files for target main_cmake
main_cmake_EXTERNAL_OBJECTS =

main_cmake: CMakeFiles/main_cmake.dir/main.cc.o
main_cmake: CMakeFiles/main_cmake.dir/server.cc.o
main_cmake: CMakeFiles/main_cmake.dir/common/common.cc.o
main_cmake: CMakeFiles/main_cmake.dir/common/config.cc.o
main_cmake: CMakeFiles/main_cmake.dir/build.make
main_cmake: CMakeFiles/main_cmake.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tcy/gaobingfa/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable main_cmake"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/main_cmake.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/main_cmake.dir/build: main_cmake

.PHONY : CMakeFiles/main_cmake.dir/build

CMakeFiles/main_cmake.dir/requires: CMakeFiles/main_cmake.dir/main.cc.o.requires
CMakeFiles/main_cmake.dir/requires: CMakeFiles/main_cmake.dir/server.cc.o.requires
CMakeFiles/main_cmake.dir/requires: CMakeFiles/main_cmake.dir/common/common.cc.o.requires
CMakeFiles/main_cmake.dir/requires: CMakeFiles/main_cmake.dir/common/config.cc.o.requires

.PHONY : CMakeFiles/main_cmake.dir/requires

CMakeFiles/main_cmake.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/main_cmake.dir/cmake_clean.cmake
.PHONY : CMakeFiles/main_cmake.dir/clean

CMakeFiles/main_cmake.dir/depend:
	cd /home/tcy/gaobingfa/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tcy/gaobingfa /home/tcy/gaobingfa /home/tcy/gaobingfa/build /home/tcy/gaobingfa/build /home/tcy/gaobingfa/build/CMakeFiles/main_cmake.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/main_cmake.dir/depend

