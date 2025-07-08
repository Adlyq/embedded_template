cmake_minimum_required(VERSION 3.10)

option(CMSIS_DSP "CMSIS DSP" OFF)

add_subdirectory(cmsis)
target_compile_options(N32G031_SDK PRIVATE -w)

if (CMSIS_DSP)
    if (NOT EXISTS ${CMAKE_HOME_DIRECTORY}/cmsis-dsp)
        execute_process(
                COMMAND git submodule add -f --depth 0 https://github.com/Adlyq/n32g031-cmsis-dsp.git sdk/cmsis-dsp
                WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
                ERROR_VARIABLE ERROR_VAR
        )
        if (ERROR_VAR)
            message(FATAL_ERROR "init cmsis-dsp submodule failed: ${ERROR_VAR}")
        endif ()
    endif ()
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