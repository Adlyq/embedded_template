# =============================================================================
# 系统与基础配置
# =============================================================================
set(CMAKE_SYSTEM_NAME Generic CACHE STRING "System Name")
set(CMAKE_SYSTEM_PROCESSOR arm CACHE STRING "System Processor")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 交叉编译安全设置
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 指定后缀
set(CMAKE_EXECUTABLE_SUFFIX .elf)

# =============================================================================
# 语言标准与扩展
# =============================================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# =============================================================================
# 核心编译参数 (MCU架构 + Picolibc)
# =============================================================================
# 定义通用的MCU和Specs标志，避免Compile和Link时重复
set(MCU_FLAGS
        -mcpu=cortex-m0
        -mthumb
        -mthumb-interwork
        --specs=picolibc.specs
)

# 基础编译选项
add_compile_options(
        ${MCU_FLAGS}
        -ffunction-sections
        -fdata-sections
        -fno-common
        -fmessage-length=0
        -fno-exceptions
        $<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp> # ASM处理
)

# 针对不同构建类型的优化选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g -Wall)
else()
    add_compile_options(-Ofast -Wall)
endif()
# =============================================================================
# 链接选项
# =============================================================================
set(TARGET_LD_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/n32g031_flash.ld)
set(TARGET_STARTUP_ASM ${CMAKE_CURRENT_LIST_DIR}/startup_n32g031_gcc.s)

add_link_options(
        ${MCU_FLAGS}
        -T ${TARGET_LD_SCRIPT}
        -Wl,-gc-sections
        -Wl,--print-memory-usage
        -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map
        -Wl,--no-warn-execstack
)

# =============================================================================
# 工具链查找 (Objcopy, Ccache, Python)
# =============================================================================
# 查找 Objcopy (优先查找编译器同级目录)
if(NOT CMAKE_OBJCOPY)
    get_filename_component(COMPILER_DIR ${CMAKE_C_COMPILER} DIRECTORY)
    find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy HINTS ${COMPILER_DIR})
    if(NOT CMAKE_OBJCOPY)
        message(WARNING "objcopy not found!")
    else()
        message(STATUS "Found objcopy: ${CMAKE_OBJCOPY}")
    endif()
endif()

# 查找并启用 Ccache
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

# 查找 Python (使用更标准的查找方式)
find_package(Python3 COMPONENTS Interpreter REQUIRED)
set(Python_EXECUTABLE ${Python3_EXECUTABLE}) # 兼容旧变量名

# =============================================================================
# 辅助函数
# =============================================================================
function(gen_keil_proj)
    execute_process(
            COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/cmake/keil_sync.py ${ARGN}
            WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
            ERROR_VARIABLE ERROR_VAR
    )
    if(ERROR_VAR)
        message(FATAL_ERROR "exec gen_keil_proj failed: ${ERROR_VAR}")
    endif()
endfunction()