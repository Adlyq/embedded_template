cmake_minimum_required(VERSION 3.10)

option(CMSIS_DSP "CMSIS DSP" ON)

add_subdirectory(cmsis)
target_compile_options(N32G031_SDK PRIVATE -w)

if (CMSIS_DSP)
    set(CMSISCORE ${CMAKE_CURRENT_LIST_DIR}/cmsis/CMSIS/core)
    add_subdirectory(cmsis-dsp/Source)
    target_compile_options(CMSISDSP PRIVATE -w)
endif ()


function(TARGET_LINK_SDK_LIBRARIES target)
    target_link_libraries(${target} N32G031_SDK)
    if (CMSIS_DSP)
        target_link_libraries(${target} CMSISDSP)
    endif ()
endfunction()