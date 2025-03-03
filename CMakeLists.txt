cmake_minimum_required(VERSION 3.10)
include(cmake/toolchain.cmake)

project(n32g031_template)

enable_language(C)
enable_language(CXX)
enable_language(ASM)

# Add N32G031 SDK
add_subdirectory(sdk)

# Add project sources
add_executable(${PROJECT_NAME}
        cmake/syscall.c
        Core/src/main.c
        Core/src/n32g031_it.c
)
target_include_directories(${PROJECT_NAME} PUBLIC Core/inc)

target_link_sdk_libraries(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} PRIVATE -Werror)

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}"
)