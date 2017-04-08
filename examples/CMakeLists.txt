cmake_minimum_required(VERSION 3.5)

project(allium_example LANGUAGES CXX)

find_package(Boost 1.59 REQUIRED COMPONENTS system coroutine)

add_library(allium_example INTERFACE)
target_link_libraries(allium_example INTERFACE allium_base)
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
    add_custom_target(all_v${version}_example)
    foreach(example IN LISTS allium_v${version}_examples)
        add_executable(${example}${version}
            v${version}/${example}/${example}.cpp)
        target_link_libraries(${example}${version} PRIVATE allium_example)
        add_dependencies(all_v${version}_example ${example}${version})
    endforeach()
endforeach()

target_link_libraries(transaction_switch10 PRIVATE Boost::coroutine)
target_link_libraries(transaction_controller13 PRIVATE Boost::coroutine)
target_link_libraries(coroutine13 PRIVATE Boost::coroutine)

