cmake_minimum_required(VERSION 3.10)
include(cmake/toolchain.cmake)

project(n32g031_template)
set(VERSION 1.00)

enable_language(C)
enable_language(CXX)
enable_language(ASM)

# Add N32G031 SDK
add_subdirectory(sdk)

# Add project sources
aux_source_directory(Core/src SRC_LIST)
add_executable(${PROJECT_NAME}
        cmake/syscall.c
        ${SRC_LIST}
)

target_include_directories(${PROJECT_NAME} PUBLIC Core/inc)

target_link_sdk_libraries(${PROJECT_NAME})

set_source_files_properties(cmake/syscall.c PROPERTIES COMPILE_FLAGS "-Wno-unused-parameter")
target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wextra)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-unused-variable -Wno-unused-function)
endif ()

get_target_property(INC_DIRS ${PROJECT_NAME} INCLUDE_DIRECTORIES)
gen_keil_proj(-i ${INC_DIRS} -s ${SRC_LIST})

set(FILE_NAME ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_V${VERSION})
set(HEX_FILE ${FILE_NAME}.hex)
set(BIN_FILE ${FILE_NAME}.bin)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}"
)