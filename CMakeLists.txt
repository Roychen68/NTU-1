cmake_minimum_required(VERSION 3.16)
project(CumulativeSum LANGUAGES CXX)

function(configure_cpp_target target)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

add_executable(cumulative_sum main.cpp)
configure_cpp_target(cumulative_sum)

include(CTest)
if(BUILD_TESTING)
    add_executable(cumulative_sum_tests tests/test_cumulative_sum.cpp)
    configure_cpp_target(cumulative_sum_tests)
    add_test(NAME histogram_tests COMMAND cumulative_sum_tests)
endif()
