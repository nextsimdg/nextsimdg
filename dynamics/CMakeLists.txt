### Subsidiary CMAKE-FILE for Nextsim-DG dynamics
#
# Setup compiler flags and compile definitions:
#
#SET(NEXTSIM_CXX_FLAGS
#  "-std=c++11 -pedantic -Wall -Wno-unknown-pragmas -march=native" CACHE STRING "Configuration flags"
#  )

################################################## This project
### directory to sources
SET (NextsimDG_DIR ${CMAKE_CURRENT_SOURCE_DIR})

### directory for binaries -> per default to build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY .)

### Name of the NextsimDG Library
set(NextsimDGLib NextsimDG)

### By standard, build a shared library
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

### By standard, release
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if (DEFINED CMAKE_CXX_FLAGS_RELEASE_INIT
   AND "${CMAKE_CXX_FLAGS_RELEASE_INIT}" STREQUAL " ${CMAKE_CXX_FLAGS_RELEASE}")
     # Overwrite the init values choosen by CMake
#     if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
         set(CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE STRING "" FORCE)
#     endif()
 endif()


################################################## Includes
LIST(APPEND Nextsim_INCLUDE_DIRS "${NextsimDG_DIR}/src/include")

################################################## Compile the Library
add_subdirectory(src)

################################################## Compile the test cases
add_subdirectory(tests)

################################################## Compile larger application examples
add_subdirectory(applications)
