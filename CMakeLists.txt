cmake_minimum_required(VERSION 3.10)
include(cmake/toolchain.cmake)

project(dvue)

enable_language(C)
enable_language(CXX)
enable_language(ASM)

# Add N32G031 SDK
add_subdirectory(sdk)

# Add project sources
aux_source_directory(Core/src SRC_LIST)
# send
set(TARGET_SEND ${PROJECT_NAME}_send)
aux_source_directory(Core/src/send SRC_LIST_SEND)
add_executable(${TARGET_SEND}
        cmake/syscall.c
        ${SRC_LIST}
        ${SRC_LIST_SEND}
)

target_include_directories(${TARGET_SEND} PUBLIC Core/inc Core/inc/send)

target_link_sdk_libraries(${TARGET_SEND})

target_compile_options(${TARGET_SEND} PRIVATE -Werror)

get_target_property(INC_DIRS ${TARGET_SEND} INCLUDE_DIRECTORIES)
set(keil_project_name KEIL=${TARGET_SEND})
gen_keil_proj(-i ${INC_DIRS} -s ${SRC_LIST} ${SRC_LIST_SEND} ${keil_project_name})

set(HEX_FILE ${PROJECT_BINARY_DIR}/${TARGET_SEND}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${TARGET_SEND}.bin)
add_custom_command(TARGET ${TARGET_SEND} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${TARGET_SEND}> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${TARGET_SEND}> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}"
)

# receive
aux_source_directory(Core/src/receive SRC_LIST_RECEIVE)
set(TARGET_RECEIVE ${PROJECT_NAME}_receive)
add_executable(${TARGET_RECEIVE}
        cmake/syscall.c
        ${SRC_LIST}
        ${SRC_LIST_RECEIVE}
)

target_include_directories(${TARGET_RECEIVE} PUBLIC Core/inc Core/inc/receive)

target_link_sdk_libraries(${TARGET_RECEIVE})

target_compile_options(${TARGET_RECEIVE} PRIVATE -Werror)

get_target_property(INC_DIRS ${TARGET_RECEIVE} INCLUDE_DIRECTORIES)
set(keil_project_name KEIL=${TARGET_RECEIVE})
gen_keil_proj(-i ${INC_DIRS} -s ${SRC_LIST} ${SRC_LIST_RECEIVE} ${keil_project_name})

set(HEX_FILE ${PROJECT_BINARY_DIR}/${TARGET_RECEIVE}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${TARGET_RECEIVE}.bin)
add_custom_command(TARGET ${TARGET_RECEIVE} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${TARGET_RECEIVE}> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${TARGET_RECEIVE}> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}"
)