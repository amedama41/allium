cmake_minimum_required(VERSION 3.5)

project(allium_example LANGUAGES CXX)

find_package(Boost 1.59 REQUIRED COMPONENTS system coroutine)

add_library(allium_example INTERFACE)
target_compile_options(allium_example INTERFACE "-Wall" "-pedantic" "-std=c++11")
target_link_libraries(allium_example INTERFACE Boost::system)
message(STATUS "Target compiler is \"${CMAKE_CXX_COMPILER_ID}\"")
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Use libc++ instead of stdlibc++")
    target_compile_options(allium_example INTERFACE "-stdlib=libc++")
    target_link_libraries(allium_example INTERFACE "-stdlib=libc++")
endif()
if(UNIX AND NOT APPLE)
    target_link_libraries(allium_example INTERFACE "pthread")
endif()

set(allium_v10_examples
    repeater_hub learning_switch switch_description decorator_switch cbench_switch
    transaction_switch)
set(allium_v13_examples
    repeater_hub learning_switch dump_table_features decorator_controller
    transaction_controller coroutine)
set(allium_ofp_versions 10 13)

foreach(version IN LISTS allium_ofp_versions)
    foreach(example IN LISTS allium_v${version}_examples)
        add_executable(${example}${version}
            v${version}/${example}/${example}.cpp)
        target_include_directories(${example}${version}
            PUBLIC "../bulb/include" "../write_queue_stream/include" "../include")
        target_link_libraries(${example}${version} PRIVATE allium_example)
    endforeach()
endforeach()

target_link_libraries(transaction_switch10 PRIVATE Boost::coroutine)
target_link_libraries(transaction_controller13 PRIVATE Boost::coroutine)
target_link_libraries(coroutine13 PRIVATE Boost::coroutine)

