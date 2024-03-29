# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /tmp/semestralka

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/semestralka/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/semestralka.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/semestralka.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/semestralka.dir/flags.make

CMakeFiles/semestralka.dir/server.cpp.o: CMakeFiles/semestralka.dir/flags.make
CMakeFiles/semestralka.dir/server.cpp.o: ../server.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/semestralka/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/semestralka.dir/server.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/semestralka.dir/server.cpp.o -c /tmp/semestralka/server.cpp

CMakeFiles/semestralka.dir/server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/semestralka.dir/server.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/semestralka/server.cpp > CMakeFiles/semestralka.dir/server.cpp.i

CMakeFiles/semestralka.dir/server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/semestralka.dir/server.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/semestralka/server.cpp -o CMakeFiles/semestralka.dir/server.cpp.s

CMakeFiles/semestralka.dir/client.cpp.o: CMakeFiles/semestralka.dir/flags.make
CMakeFiles/semestralka.dir/client.cpp.o: ../client.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/semestralka/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/semestralka.dir/client.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/semestralka.dir/client.cpp.o -c /tmp/semestralka/client.cpp

CMakeFiles/semestralka.dir/client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/semestralka.dir/client.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/semestralka/client.cpp > CMakeFiles/semestralka.dir/client.cpp.i

CMakeFiles/semestralka.dir/client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/semestralka.dir/client.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/semestralka/client.cpp -o CMakeFiles/semestralka.dir/client.cpp.s

CMakeFiles/semestralka.dir/database.cpp.o: CMakeFiles/semestralka.dir/flags.make
CMakeFiles/semestralka.dir/database.cpp.o: ../database.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/semestralka/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/semestralka.dir/database.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/semestralka.dir/database.cpp.o -c /tmp/semestralka/database.cpp

CMakeFiles/semestralka.dir/database.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/semestralka.dir/database.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/semestralka/database.cpp > CMakeFiles/semestralka.dir/database.cpp.i

CMakeFiles/semestralka.dir/database.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/semestralka.dir/database.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/semestralka/database.cpp -o CMakeFiles/semestralka.dir/database.cpp.s

# Object files for target semestralka
semestralka_OBJECTS = \
"CMakeFiles/semestralka.dir/server.cpp.o" \
"CMakeFiles/semestralka.dir/client.cpp.o" \
"CMakeFiles/semestralka.dir/database.cpp.o"

# External object files for target semestralka
semestralka_EXTERNAL_OBJECTS =

semestralka: CMakeFiles/semestralka.dir/server.cpp.o
semestralka: CMakeFiles/semestralka.dir/client.cpp.o
semestralka: CMakeFiles/semestralka.dir/database.cpp.o
semestralka: CMakeFiles/semestralka.dir/build.make
semestralka: CMakeFiles/semestralka.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/semestralka/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable semestralka"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/semestralka.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/semestralka.dir/build: semestralka

.PHONY : CMakeFiles/semestralka.dir/build

CMakeFiles/semestralka.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/semestralka.dir/cmake_clean.cmake
.PHONY : CMakeFiles/semestralka.dir/clean

CMakeFiles/semestralka.dir/depend:
	cd /tmp/semestralka/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/semestralka /tmp/semestralka /tmp/semestralka/cmake-build-debug /tmp/semestralka/cmake-build-debug /tmp/semestralka/cmake-build-debug/CMakeFiles/semestralka.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/semestralka.dir/depend

