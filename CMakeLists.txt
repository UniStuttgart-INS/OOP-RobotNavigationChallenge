cmake_minimum_required(VERSION 3.15) # Only set as high as the features you are using

# Add the subfolders 'cmake' and 'cmake/modules' to the search path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(PROJECT_VERSION "1.0.0") # Defines a variable. Usable as ${PROJECT_VERSION}

# Set the project name, version and language
project(
  "OOP-Robot-Navigation-Challenge"
  VERSION ${PROJECT_VERSION}
  LANGUAGES CXX)

# Convert the project name to lower case and store in variabel PROJECT_NAME_LOWERCASE (will be used
# as executable name later)
string(TOLOWER ${CMAKE_PROJECT_NAME} PROJECT_NAME_LOWERCASE)

message(STATUS "${CMAKE_PROJECT_NAME}: ${PROJECT_VERSION}") # Prints a message

# ######################################################################################################################

# Export the SDKROOT Environment Variable for MacOS
if(APPLE)
  if(NOT DEFINED ENV{SDKROOT})
    execute_process(COMMAND xcrun --show-sdk-path OUTPUT_VARIABLE sdkpath)
    string(REGEX REPLACE "\n$" "" sdkpath "${sdkpath}")
    set(ENV{SDKROOT} ${sdkpath})
    message(STATUS "SDK-Path: $ENV{SDKROOT}")
  endif()
elseif(WIN32)
  add_compile_definitions(_USE_MATH_DEFINES)
endif()

# ##################################################################################################

# Basic settings
include(cmake/StandardProjectSettings.cmake)

# Prevent cmake builds with source dir .
include(cmake/PreventInSourceBuilds.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings) # set standard compiler warnings


# ##################################################################################################
# Conan Packages
# ##################################################################################################

include(cmake/Conan.cmake)
run_conan()

# ##################################################################################################
# Libraries (inlcude as SYSTEM, to prevent Static Analazers to scan them)
# ##################################################################################################

# ---------------------------------------------- imgui ---------------------------------------------

file(
  GLOB _imgui_Sources
  LIST_DIRECTORIES OFF
  "${CMAKE_SOURCE_DIR}/lib/imgui/*.cpp" "${CMAKE_SOURCE_DIR}/lib/imgui/*.h"
  "${CMAKE_SOURCE_DIR}/lib/imgui/misc/cpp/*.cpp" "${CMAKE_SOURCE_DIR}/lib/imgui/misc/cpp/*.h")
add_library(imgui ${_imgui_Sources})
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/lib/imgui")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/lib/imgui/misc/cpp")
target_link_libraries(imgui PRIVATE project_options)
target_compile_definitions(imgui PUBLIC "ImDrawIdx=unsigned int")
set_target_properties(imgui PROPERTIES CXX_CLANG_TIDY "")
set_target_properties(imgui PROPERTIES CXX_CPPCHECK "")

# --------------------------------------------- implot ---------------------------------------------

file(
  GLOB _implot_Sources
  LIST_DIRECTORIES OFF
  "${CMAKE_SOURCE_DIR}/lib/implot/*.cpp" "${CMAKE_SOURCE_DIR}/lib/implot/*.h")
add_library(implot ${_implot_Sources})
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/lib/implot")
target_link_libraries(implot PRIVATE project_options)
target_compile_definitions(implot PUBLIC "ImDrawIdx=unsigned int")
set_target_properties(implot PROPERTIES CXX_CLANG_TIDY "")
set_target_properties(implot PROPERTIES CXX_CPPCHECK "")

# ------------------------------------------- Application ------------------------------------------

add_subdirectory(lib/application)
include_directories(SYSTEM lib/application/include)
target_link_libraries(application PRIVATE project_options imgui implot)
target_compile_definitions(application PUBLIC "_CONSOLE")
set_target_properties(application PROPERTIES CXX_CLANG_TIDY "")
set_target_properties(application PROPERTIES CXX_CPPCHECK "")

# ##################################################################################################

# Specify output path for executable files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

# ##################################################################################################

# Include all project headers. In a standalone project it is common to put headers into the
# 'source' folder together with their source files. In library projects an 'include' folder is used.
include_directories(src)

# Run the CMakeLists.txt file in the specified directory
add_subdirectory(src)

# ##################################################################################################

# Because MSVC sucks and we need to tell the linker to link the release versions manually
if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Release")
  set_target_properties(
    ${PROJECT_NAME_LOWERCASE}
    imgui
    implot
    application
    PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()