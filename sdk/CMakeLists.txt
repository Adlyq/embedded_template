cmake_minimum_required(VERSION 3.10)

add_subdirectory(cmsis-n32g031)
target_compile_options(N32G031_SDK PRIVATE -w)

function(TARGET_LINK_SDK_LIBRARIES target)
    target_link_libraries(${target} N32G031_SDK)
endfunction()