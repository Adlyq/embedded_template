cmake_minimum_required(VERSION 3.10)

execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(TIMESTAMP VERSION_TIME "%y.%m.%d")

project(n32g031_template
        VERSION ${VERSION_TIME}
        LANGUAGES C CXX ASM
)

# --- SDK & Library Configuration ---
add_library(BSP_LIB INTERFACE)
add_subdirectory(sdk)

# --- Application Sources ---
aux_source_directory(Core/src SRC_LIST)
list(APPEND APP_SRCS
    cmake/syscall.c
    ${SRC_LIST}
)

# --- Executable Definition ---
add_executable(${PROJECT_NAME} ${APP_SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE BSP_LIB)
target_include_directories(${PROJECT_NAME} PUBLIC Core/inc)

# --- Compile Options & Definitions ---
target_compile_options(${PROJECT_NAME} PRIVATE 
    -Werror -Wextra -Wsign-conversion
    $<$<CONFIG:Debug>:-Wno-unused-variable -Wno-unused-function>
)
set_source_files_properties(cmake/syscall.c PROPERTIES COMPILE_FLAGS "-Wno-unused-parameter")

# Handle Output Configuration from Presets
foreach (def IN LISTS Output)
    message(STATUS "[Config] Using ${def} output")
    target_compile_definitions(${PROJECT_NAME} PRIVATE Output_${def})
endforeach ()

# Build Type Specific Definitions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "[Config] Building in Debug mode")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG SEGGER_RTT)
else ()
    message(STATUS "[Config] Building in Release mode")
    target_compile_definitions(${PROJECT_NAME} PRIVATE RELEASE)
endif ()

# --- Keil Project Synchronization ---
get_target_property(INC_DIRS ${PROJECT_NAME} INCLUDE_DIRECTORIES)
get_target_property(COMPILE_DEFS ${PROJECT_NAME} COMPILE_DEFINITIONS)
if(NOT COMPILE_DEFS)
    set(COMPILE_DEFS "")
endif()
gen_keil_proj(-i ${INC_DIRS} -s ${SRC_LIST} -d ${COMPILE_DEFS})

# --- Binary Artifacts & Naming ---
if (NOT FILE_NAME)
    set(FILE_NAME ${PROJECT_NAME})
endif ()

if ("PNP" IN_LIST Output)
    string(APPEND FILE_NAME "P")
elseif ("NPN" IN_LIST Output)
    string(APPEND FILE_NAME "N")
endif ()

if ("LIGHT_CLOSE" IN_LIST Output)
    string(APPEND FILE_NAME "C")
elseif ("LIGHT_OPEN" IN_LIST Output)
    string(APPEND FILE_NAME "O")
endif ()

if (DEFINED PROJECT_VERSION)
    string(APPEND FILE_NAME "_V${PROJECT_VERSION}.${GIT_HASH}")
endif ()

set(HEX_FILE ${PROJECT_BINARY_DIR}/${FILE_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${FILE_NAME}.bin)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}> ${HEX_FILE}
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${BIN_FILE}
            COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Generating Artifacts: ${HEX_FILE}, ${BIN_FILE}"
    )
endif ()