# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_SOURCE_DIR = /home/aman/Desktop/Intern/fast_em_pca

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/aman/Desktop/Intern/fast_em_pca/temp2

# Include any dependencies generated for this target.
include CMakeFiles/em_naive.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/em_naive.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/em_naive.dir/flags.make

CMakeFiles/em_naive.dir/src/em_naive.cpp.o: CMakeFiles/em_naive.dir/flags.make
CMakeFiles/em_naive.dir/src/em_naive.cpp.o: ../src/em_naive.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/aman/Desktop/Intern/fast_em_pca/temp2/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/em_naive.dir/src/em_naive.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/em_naive.dir/src/em_naive.cpp.o -c /home/aman/Desktop/Intern/fast_em_pca/src/em_naive.cpp

CMakeFiles/em_naive.dir/src/em_naive.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/em_naive.dir/src/em_naive.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/aman/Desktop/Intern/fast_em_pca/src/em_naive.cpp > CMakeFiles/em_naive.dir/src/em_naive.cpp.i

CMakeFiles/em_naive.dir/src/em_naive.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/em_naive.dir/src/em_naive.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/aman/Desktop/Intern/fast_em_pca/src/em_naive.cpp -o CMakeFiles/em_naive.dir/src/em_naive.cpp.s

CMakeFiles/em_naive.dir/src/em_naive.cpp.o.requires:
.PHONY : CMakeFiles/em_naive.dir/src/em_naive.cpp.o.requires

CMakeFiles/em_naive.dir/src/em_naive.cpp.o.provides: CMakeFiles/em_naive.dir/src/em_naive.cpp.o.requires
	$(MAKE) -f CMakeFiles/em_naive.dir/build.make CMakeFiles/em_naive.dir/src/em_naive.cpp.o.provides.build
.PHONY : CMakeFiles/em_naive.dir/src/em_naive.cpp.o.provides

CMakeFiles/em_naive.dir/src/em_naive.cpp.o.provides.build: CMakeFiles/em_naive.dir/src/em_naive.cpp.o

CMakeFiles/em_naive.dir/src/genotype.cpp.o: CMakeFiles/em_naive.dir/flags.make
CMakeFiles/em_naive.dir/src/genotype.cpp.o: ../src/genotype.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/aman/Desktop/Intern/fast_em_pca/temp2/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/em_naive.dir/src/genotype.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/em_naive.dir/src/genotype.cpp.o -c /home/aman/Desktop/Intern/fast_em_pca/src/genotype.cpp

CMakeFiles/em_naive.dir/src/genotype.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/em_naive.dir/src/genotype.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/aman/Desktop/Intern/fast_em_pca/src/genotype.cpp > CMakeFiles/em_naive.dir/src/genotype.cpp.i

CMakeFiles/em_naive.dir/src/genotype.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/em_naive.dir/src/genotype.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/aman/Desktop/Intern/fast_em_pca/src/genotype.cpp -o CMakeFiles/em_naive.dir/src/genotype.cpp.s

CMakeFiles/em_naive.dir/src/genotype.cpp.o.requires:
.PHONY : CMakeFiles/em_naive.dir/src/genotype.cpp.o.requires

CMakeFiles/em_naive.dir/src/genotype.cpp.o.provides: CMakeFiles/em_naive.dir/src/genotype.cpp.o.requires
	$(MAKE) -f CMakeFiles/em_naive.dir/build.make CMakeFiles/em_naive.dir/src/genotype.cpp.o.provides.build
.PHONY : CMakeFiles/em_naive.dir/src/genotype.cpp.o.provides

CMakeFiles/em_naive.dir/src/genotype.cpp.o.provides.build: CMakeFiles/em_naive.dir/src/genotype.cpp.o

# Object files for target em_naive
em_naive_OBJECTS = \
"CMakeFiles/em_naive.dir/src/em_naive.cpp.o" \
"CMakeFiles/em_naive.dir/src/genotype.cpp.o"

# External object files for target em_naive
em_naive_EXTERNAL_OBJECTS =

em_naive: CMakeFiles/em_naive.dir/src/em_naive.cpp.o
em_naive: CMakeFiles/em_naive.dir/src/genotype.cpp.o
em_naive: CMakeFiles/em_naive.dir/build.make
em_naive: CMakeFiles/em_naive.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable em_naive"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/em_naive.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/em_naive.dir/build: em_naive
.PHONY : CMakeFiles/em_naive.dir/build

CMakeFiles/em_naive.dir/requires: CMakeFiles/em_naive.dir/src/em_naive.cpp.o.requires
CMakeFiles/em_naive.dir/requires: CMakeFiles/em_naive.dir/src/genotype.cpp.o.requires
.PHONY : CMakeFiles/em_naive.dir/requires

CMakeFiles/em_naive.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/em_naive.dir/cmake_clean.cmake
.PHONY : CMakeFiles/em_naive.dir/clean

CMakeFiles/em_naive.dir/depend:
	cd /home/aman/Desktop/Intern/fast_em_pca/temp2 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/aman/Desktop/Intern/fast_em_pca /home/aman/Desktop/Intern/fast_em_pca /home/aman/Desktop/Intern/fast_em_pca/temp2 /home/aman/Desktop/Intern/fast_em_pca/temp2 /home/aman/Desktop/Intern/fast_em_pca/temp2/CMakeFiles/em_naive.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/em_naive.dir/depend

