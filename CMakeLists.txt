cmake_minimum_required(VERSION 3.5) # setting this is required
project(beseda)            # this sets the project name

###############################################################################
## file globbing ##############################################################
###############################################################################

# These instructions search the directory tree when cmake is
# invoked and put all files that match the pattern in the variables
# `sources` and `data`.
file(GLOB_RECURSE sources  src/*.cpp src/*.h)
#file(GLOB_RECURSE data resources/*)
# You can use set(sources src/main.cpp) etc if you don't want to
# use globbing to find files automatically.

###############################################################################
## target definitions #########################################################
###############################################################################

# The data is just added to the executable, because in some IDEs (QtCreator)
# files are invisible when they are not explicitly part of the project.
#add_executable(boltun ${sources} ${data})

add_executable(beseda ${sources})


find_package(PkgConfig)
pkg_check_modules(SPEECH REQUIRED speech-dispatcher>=0.0.1)

target_link_libraries(beseda ${SPEECH_LIBRARIES})

# This allows to include files relative to the root of the src directory with a <> pair
target_include_directories(beseda PUBLIC src ${SPEECH_INCLUDE_DIRS})


# Just for example add some compiler flags.
target_compile_options(beseda PUBLIC -std=c++1y -Wall -Wfloat-conversion)
