cmake_minimum_required(VERSION 3.10)

project(n32g031_template)
set(VERSION 2.0)

enable_language(C)
enable_language(CXX)
enable_language(ASM)

# Add project sources
aux_source_directory(Core/src SRC_LIST)
add_executable(${PROJECT_NAME}
        cmake/syscall.c
        ${SRC_LIST}
)
add_library(BSP_LIB INTERFACE)
target_link_libraries(${PROJECT_NAME} PRIVATE BSP_LIB)

# Add N32G031 SDK
add_subdirectory(sdk)

foreach (def IN LISTS Output)
    message("Using ${def} output")
    target_compile_definitions(${PROJECT_NAME} PRIVATE Output_${def})
endforeach ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug mode")
    add_compile_definitions(DEBUG)
    add_compile_definitions(SEG_RTT)
else ()
    message(STATUS "Building in Release mode")
    add_compile_definitions(RELEASE)
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC Core/inc)

set_source_files_properties(cmake/syscall.c PROPERTIES COMPILE_FLAGS "-Wno-unused-parameter")
target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wextra -Wsign-conversion)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-unused-variable -Wno-unused-function)
endif ()

get_target_property(INC_DIRS ${PROJECT_NAME} INCLUDE_DIRECTORIES)
gen_keil_proj(-i ${INC_DIRS} -s ${SRC_LIST})

# Set the file name based on the output type ->
if (NOT FILE_NAME)
    set(FILE_NAME ${PROJECT_NAME})
endif ()
if ("PNP" IN_LIST Output)
    set(FILE_NAME ${FILE_NAME}P)
elseif ("NPN" IN_LIST Output)
    set(FILE_NAME ${FILE_NAME}N)
endif ()

if ("LIGHT_CLOSE" IN_LIST Output)
    set(FILE_NAME ${FILE_NAME}C)
elseif ("LIGHT_OPEN" IN_LIST Output)
    set(FILE_NAME ${FILE_NAME}O)
endif ()

if (DEFINED VERSION)
    set(FILE_NAME ${FILE_NAME}_V${VERSION})
endif ()
# Set the file name based on the output type <-

set(HEX_FILE ${PROJECT_BINARY_DIR}/${FILE_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${FILE_NAME}.bin)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} ${BIN_FILE}"
)