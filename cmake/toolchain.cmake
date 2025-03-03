function(_generate_file TARGET OUTPUT_EXTENSION OBJCOPY_BFD_OUTPUT)
    get_target_property(TARGET_OUTPUT_NAME ${TARGET} OUTPUT_NAME)
    if (TARGET_OUTPUT_NAME)
        set(OUTPUT_FILE_NAME "${TARGET_OUTPUT_NAME}.${OUTPUT_EXTENSION}")
    else()
        set(OUTPUT_FILE_NAME "${TARGET}.${OUTPUT_EXTENSION}")
    endif()

    get_target_property(RUNTIME_OUTPUT_DIRECTORY ${TARGET} RUNTIME_OUTPUT_DIRECTORY)
    if(RUNTIME_OUTPUT_DIRECTORY)
        set(OUTPUT_FILE_PATH "${RUNTIME_OUTPUT_DIRECTORY}/${OUTPUT_FILE_NAME}")
    else()
        set(OUTPUT_FILE_PATH "${OUTPUT_FILE_NAME}")
    endif()

    add_custom_command(
            TARGET ${TARGET}
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -O ${OBJCOPY_BFD_OUTPUT} "$<TARGET_FILE:${TARGET}>" ${OUTPUT_FILE_PATH}
            BYPRODUCTS ${OUTPUT_FILE_PATH}
            COMMENT "Generating ${OBJCOPY_BFD_OUTPUT} file ${OUTPUT_FILE_NAME}"
    )
endfunction()

function(generate_binary_file TARGET)
    _generate_file(${TARGET} "bin" "binary")
endfunction()

function(generate_hex_file TARGET)
    _generate_file(${TARGET} "hex" "ihex")
endfunction()

set(CMAKE_EXECUTABLE_SUFFIX_C   .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

# This should be safe to set for a bare-metal cross-compiler
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)


# Extra CFlags
set(TARGET_CFLAGS_EXTRA   "-Wall -fdata-sections -ffunction-sections")
set(TARGET_CXXFLAGS_EXTRA "-Wall -fdata-sections -ffunction-sections")
set(TARGET_LDFLAGS_EXTRA  "-Wl,--print-memory-usage")

# Device specific settings, goes to CFLAGS and LDFLAGS
set(TARGET_CFLAGS_HARDWARE "-mcpu=cortex-m0 -mthumb")

# Conditional flags
# DEBUG
set(CMAKE_C_FLAGS_DEBUG     "-DDEBUG=0 -O0 -g")
set(CMAKE_CXX_FLAGS_DEBUG   "-DDEBUG=0 -O0 -g")
set(CMAKE_ASM_FLAGS_DEBUG   "-DDEBUG=0 -O0 -g")

# RELEASE
set(CMAKE_C_FLAGS_RELEASE        "-DNDEBUG -Os") #  -flto
set(CMAKE_CXX_FLAGS_RELEASE      "-DNDEBUG -Os") #  -flto
set(CMAKE_ASM_FLAGS_RELEASE      "-DNDEBUG -Os") #  -flto
#set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto")

# Final compiler flags
set(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS} ${TARGET_CFLAGS_HARDWARE} ${TARGET_CFLAGS_EXTRA}")
set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} ${TARGET_CFLAGS_HARDWARE} ${TARGET_CXXFLAGS_EXTRA}")
set(CMAKE_ASM_FLAGS        "${CMAKE_ASM_FLAGS} ${CMAKE_C_FLAGS} -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS "-specs=nosys.specs -Wl,--gc-sections ${TARGET_LDFLAGS_EXTRA}")

