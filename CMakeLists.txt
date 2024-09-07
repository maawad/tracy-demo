cmake_minimum_required(VERSION 3.10)
project(CustomCounterExample)

# Enable C++11 (or later) support
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include CPM.cmake (download if not available)
set(CPM_SOURCE_CACHE "${CMAKE_SOURCE_DIR}/cmake/CPM")
if(NOT EXISTS "${CPM_SOURCE_CACHE}/CPM.cmake")
    file(DOWNLOAD 
        https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake 
        "${CPM_SOURCE_CACHE}/CPM.cmake"
    )
endif()
include("${CPM_SOURCE_CACHE}/CPM.cmake")

# Add Tracy via CPM
CPMAddPackage(
    NAME Tracy
    GITHUB_REPOSITORY wolfpld/tracy
    VERSION 0.11.1
    OPTIONS "TRACY_ENABLE ON" "TRACY_ON_DEMAND ON"
)

set(SOURCE_FILES
    "multithreaded_single_var.cpp"
    "multithreaded_multiple_vars.cpp"
    "simple.cpp"
)

# Loop over the source files and create executables
foreach(SOURCE_FILE IN LISTS SOURCE_FILES)
    # Extract the base name for each file (without extension)
    get_filename_component(EXEC_NAME ${SOURCE_FILE} NAME_WE)

    # Add an executable for each source file
    add_executable(${EXEC_NAME} ${SOURCE_FILE})

    # Define TRACY_ENABLE for the whole project
    target_compile_definitions(${EXEC_NAME} PRIVATE TRACY_ENABLE)

    # Explicitly include Tracy's public directory to find Tracy.hpp
    target_include_directories(${EXEC_NAME} PRIVATE ${Tracy_SOURCE_DIR}/public)

    # Link Tracy library
    target_link_libraries(${EXEC_NAME} PRIVATE Tracy::TracyClient)
endforeach()