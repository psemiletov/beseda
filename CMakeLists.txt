cmake_minimum_required(VERSION 3.5) # setting this is required
project(beseda VERSION 1.2.1)            # this sets the project name
add_definitions(-DVERSION_NUMBER="${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")



###############################################################################
## file globbing ##############################################################
###############################################################################

# These instructions search the directory tree when cmake is
# invoked and put all files that match the pattern in the variables
# `sources` and `data`.
file(GLOB_RECURSE sources  src/*.cpp src/*.h src/*.hpp src/*.c)
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


# This allows to include files relative to the root of the src directory with a <> pair
target_include_directories(beseda PUBLIC src ${SPEECH_INCLUDE_DIRS})

#find_package(Curses REQUIRED)


pkg_check_modules(CURSES REQUIRED ncursesw>=6.0.0)


include_directories(${CURSES_INCLUDE_DIR})

target_link_libraries(beseda ${SPEECH_LIBRARIES} ${CURSES_LIBRARIES})

target_compile_options(beseda PUBLIC -Wall -Wfloat-conversion)


install (TARGETS beseda DESTINATION bin)
