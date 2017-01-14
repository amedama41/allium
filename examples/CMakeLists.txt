cmake_minimum_required(VERSION 3.5)

project(allium_example LANGUAGES CXX)

find_package(Boost 1.59 REQUIRED COMPONENTS system)

add_library(allium_example INTERFACE)
target_compile_options(allium_example INTERFACE "-Wall" "-pedantic" "-std=c++11")
target_link_libraries(allium_example INTERFACE Boost::system)
message(STATUS "target compiler is \"${CMAKE_CXX_COMPILER_ID}\"")
if(CMAKE_CXX_COMPILER_ID MATCHES "clang")
    target_compile_options(allium_example INTERFACE "-stdlib=libc++")
    target_link_libraries(allium_example INTERFACE "-stdlib=libc++")
endif()
if(UNIX AND NOT APPLE)
    target_link_libraries(allium_example INTERFACE "pthread")
endif()

set(allium_v10_examples
    repeater_hub learning_switch switch_description decorator_switch cbench_switch)
set(allium_ofp_versions 10)

foreach(version IN LISTS allium_ofp_versions)
    foreach(example IN LISTS allium_v${version}_examples)
        add_executable(${example}${version}
            v${version}/${example}/${example}.cpp)
        target_include_directories(${example}${version}
            PUBLIC "../bulb/include" "../write_queue_stream/include" "../include")
        target_link_libraries(${example}${version} PRIVATE allium_example)
    endforeach()
endforeach()
