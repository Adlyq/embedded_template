set(CMAKE_EXECUTABLE_SUFFIX_C   .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

# This should be safe to set for a bare-metal cross-compiler
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3 -Wall")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3 -Wall")
else ()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS} -O0 -g -Wall")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -O0 -g -Wall")
endif ()

add_compile_options(-mcpu=cortex-m0 -mthumb -mthumb-interwork)
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -fno-exceptions)
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

# Custom linker script
set(TARGET_LD_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/n32g031_flash.ld)
# Custom startup
set(TARGET_STARTUP_ASM ${CMAKE_CURRENT_LIST_DIR}/startup_n32g031_gcc.s)

add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=cortex-m0 -mthumb -mthumb-interwork)
add_link_options(-T ${TARGET_LD_SCRIPT})

if (NOT Python_EXECUTABLE)
    message(FATAL_ERROR "Python executable not found")
endif ()

function(gen_keil_proj)
    execute_process(
            COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/cmake/keil_sync.py ${ARGN}
            WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
            ERROR_VARIABLE ERROR_VAR
    )
    if (ERROR_VAR)
        message(FATAL_ERROR "exec gen_keil_proj failed: ${ERROR_VAR}")
    endif ()
endfunction()