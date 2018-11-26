#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(GBSFit)

# I wanna compile with C++11
add_definitions("-std=c++11 -march=native -mtune=native")

# Making json file for auto complete
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Searching ROOT. FindROOT.cmake was copied from ROOT source
include(FindROOT.cmake)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${ROOT_INCLUDE_DIR})
link_directories(${ROOT_LIBRARY_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hpp)

# Generating Dictionary for ROOT
set(MY_LINKDEF LinkDef.h )
set(MY_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/MyDict.cxx)
ROOT_GENERATE_DICTIONARY("${headers}" "${MY_LINKDEF}" "${MY_DICTIONARY}" "${INCLUDE_DIRECTORIES}")
#----------------------------------------------------------------------------
# Generating executable file and linking
#
add_executable(GBSFit main.cpp MyDict.cxx ${sources} ${headers})
target_link_libraries(GBSFit ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS GBSFit DESTINATION bin)
