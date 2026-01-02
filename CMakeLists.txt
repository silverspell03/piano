cmake_minimum_required(VERSION 3.16)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(piano)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

# Create your game executable target as usual
add_executable(piano src/main.c src/app.c src/ringbuffer.c src/audio.c src/graphics.c src/ui.c)

target_include_directories(piano PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Link to the actual SDL3 library.
target_link_libraries(piano PRIVATE SDL3::SDL3 m)
