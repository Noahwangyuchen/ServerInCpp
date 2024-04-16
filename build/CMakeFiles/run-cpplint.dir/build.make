# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/noah/project/cppserver

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/noah/project/cppserver/build

# Utility rule file for run-cpplint.

# Include the progress variables for this target.
include CMakeFiles/run-cpplint.dir/progress.make

CMakeFiles/run-cpplint:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/noah/project/cppserver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Running cpplint to check code style"
	/home/noah/.local/bin/cpplint --verbose=2 --quiet --filter=-legal/copyright,-build/include_subdir,-readability/utf8,-whitespace/ending_newline /home/noah/project/cppserver/src/Acceptor.cpp /home/noah/project/cppserver/src/Buffer.cpp /home/noah/project/cppserver/src/Channel.cpp /home/noah/project/cppserver/src/Connection.cpp /home/noah/project/cppserver/src/Epoll.cpp /home/noah/project/cppserver/src/EventLoop.cpp /home/noah/project/cppserver/src/Server.cpp /home/noah/project/cppserver/src/Socket.cpp /home/noah/project/cppserver/src/ThreadPool.cpp /home/noah/project/cppserver/src/util.cpp

run-cpplint: CMakeFiles/run-cpplint
run-cpplint: CMakeFiles/run-cpplint.dir/build.make

.PHONY : run-cpplint

# Rule to build all files generated by this target.
CMakeFiles/run-cpplint.dir/build: run-cpplint

.PHONY : CMakeFiles/run-cpplint.dir/build

CMakeFiles/run-cpplint.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/run-cpplint.dir/cmake_clean.cmake
.PHONY : CMakeFiles/run-cpplint.dir/clean

CMakeFiles/run-cpplint.dir/depend:
	cd /home/noah/project/cppserver/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/noah/project/cppserver /home/noah/project/cppserver /home/noah/project/cppserver/build /home/noah/project/cppserver/build /home/noah/project/cppserver/build/CMakeFiles/run-cpplint.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/run-cpplint.dir/depend

