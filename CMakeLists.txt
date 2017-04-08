cmake_minimum_required(VERSION 3.5)

project(allium LANGUAGES CXX)

enable_testing()

add_subdirectory(bulb)

add_library(allium_base INTERFACE)
target_link_libraries(allium_base INTERFACE bulb_base bulb::libbulb)
target_include_directories(allium_base
    INTERFACE "write_queue_stream/include" "include")

add_subdirectory(examples)

