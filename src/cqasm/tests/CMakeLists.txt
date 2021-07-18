
add_executable(test-v1-parsing v1-parsing.cpp)
target_link_libraries(test-v1-parsing gtest cqasm)
add_test(
    NAME test-v1-parsing
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND test-v1-parsing
)

add_executable(test-v2-parsing v2-parsing.cpp)
target_link_libraries(test-v2-parsing gtest cqasm)
add_test(
    NAME test-v2-parsing
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND test-v2-parsing
)

add_executable(tutorial tutorial.cpp)
target_link_libraries(tutorial gtest_main cqasm)
add_test(
    NAME tutorial
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND tutorial
)
