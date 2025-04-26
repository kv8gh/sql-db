cmake_minimum_required(VERSION 3.10)
project(MiniDB)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directories - expanded to include all potential header locations
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/sql
    ${CMAKE_CURRENT_SOURCE_DIR}/storage
    ${CMAKE_CURRENT_SOURCE_DIR}/executor
)

# Find all source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp" 
    "sql/*.cpp" 
    "storage/*.cpp" 
    "executor/*.cpp"
    "include/*.cpp"
)

# Create executable
add_executable(minidb ${SOURCES})
message(STATUS "Sources found: ${SOURCES}")


# Add compiler warnings
if(MSVC)
    target_compile_options(minidb PRIVATE /W4)
else()
    target_compile_options(minidb PRIVATE -Wall -Wextra -Wpedantic)

    target_link_options(minidb PRIVATE -static -static-libgcc -static-libstdc++)
endif()

# Generate compile_commands.json for better IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)