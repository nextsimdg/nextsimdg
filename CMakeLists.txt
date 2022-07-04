cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)

project(nextsim_dg CXX)

set (NETCDF_CXX "YES")
find_package(netCDF REQUIRED)
if ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Darwin")
    set(NSDG_NetCDF_Library "netcdf-cxx4")
else()
    set(NSDG_NetCDF_Library "netcdf_c++4")
endif()

# Additional compile options
add_compile_options(-pedantic -Wall -Wno-unknown-pragmas)

# OpenMP support taken from the dynamics CMakeLists.txt
option(WITH_THREADS "Build with support for OpenMP" OFF)

find_package(OpenMP)
if (OPENMP_FOUND)
    if(WITH_THREADS)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    endif()
endif()

# Regarding Boost.Log, if our application consists of more
# than one modules that use it, we must link to the shared
# version. If we have a single executable or a single module
# that works, we may use the static version.
# By default, it is assumed that the library is built in
# static mode. Use the following definition to indicate that
# the code will be linked against dynamically loaded boost
# libraries.
add_definitions(-DBOOST_ALL_DYN_LINK)
find_package(Boost COMPONENTS program_options log REQUIRED)

find_package(Catch2 REQUIRED)
find_package(Eigen3 3.3 REQUIRED)

# To add netCDF to a target:
# target_include_directories(target PUBLIC ${netCDF_INCLUDE_DIR})
# target_link_directories(target PUBLIC ${netCDF_LIB_DIR})
# target_link_libraries(target LINK_PUBLIC "${NSDG_NetCDF_Library}")

### Use clang-format
OPTION(USE_CLANG_FORMAT "Formats the codebase" ON)
if(USE_CLANG_FORMAT)
  FILE(GLOB_RECURSE ALL_CXX_SOURCE_FILES core/*.cpp core/*.hpp  dynamics/*.cpp dynamics/*.hpp physics/*.cpp  physics/*.hpp)

  # Adding clang-format target if executable is found
  find_program(CLANG_FORMAT "clang-format")
  if(CLANG_FORMAT)
    add_custom_target(
      clang-format ALL
      COMMAND ${CLANG_FORMAT}
      -i
      -style=WebKit
      ${ALL_CXX_SOURCE_FILES}
      )
  else()
    message(STATUS "Could not find clang-format")
  endif()
endif()

# Set the list of components that define modules. Component subdirectories are
# contained in this directory level and contain src/modules/ and 
# src/modules/include/ subdirectories 
set(ModularComponents
"physics"
)

# Set the components which provide source or object code to the main model
set(CodeComponents
"physics"
"dynamics"
)

# Set an empty list of sources
set(NextsimSources "")

# Set an empty list of include directories
set(NextsimIncludeDirs "")

# Build the core model. Defines the 'parse_modules' target
add_subdirectory(core)

# Build all components
foreach(compo ${CodeComponents})
    add_subdirectory("${compo}")
endforeach()

# Link the model
add_executable(nextsim "${NextsimSources}")

target_include_directories(nextsim PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${ModuleLoaderIppTargetDirectory}"
    "${NextsimIncludeDirs}"
    "${netCDF_INCLUDE_DIR}"
    )
target_link_directories(nextsim PUBLIC "${netCDF_LIB_DIR}")
target_link_libraries(nextsim LINK_PUBLIC Boost::program_options Boost::log "${NSDG_NetCDF_Library}" Eigen3::Eigen)
