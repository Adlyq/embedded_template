# =============================================================================
# 系统与基础配置
# =============================================================================
set(CMAKE_SYSTEM_NAME Generic CACHE STRING "System Name")
set(CMAKE_SYSTEM_PROCESSOR arm CACHE STRING "System Processor")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 交叉编译安全设置
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_EXECUTABLE_SUFFIX .elf)

# =============================================================================
# 编译器探测与环境检测
# =============================================================================
set(USING_CLANG OFF)
set(USING_GCC OFF)

if(DEFINED CMAKE_C_COMPILER)
    get_filename_component(C_COMPILER_NAME ${CMAKE_C_COMPILER} NAME)
    if(C_COMPILER_NAME MATCHES "clang")
        set(USING_CLANG ON)
    elseif(C_COMPILER_NAME MATCHES "gcc")
        set(USING_GCC ON)
    endif()
endif()

# =============================================================================
# 编译器路径配置
# =============================================================================
if(USING_CLANG)
    message(STATUS "[Toolchain] Configuring for Clang/LLVM...")

    set(TRIPLE "arm-none-eabi")
    set(CMAKE_C_COMPILER_TARGET ${TRIPLE})
    set(CMAKE_CXX_COMPILER_TARGET ${TRIPLE})
    set(CMAKE_ASM_COMPILER_TARGET ${TRIPLE})

elseif(USING_GCC)
    message(STATUS "[Toolchain] Configuring for ARM GCC...")

endif()

# 辅助工具
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy)
find_program(CMAKE_SIZE arm-none-eabi-size)

# =============================================================================
# 通用系统标志 (MCU Arch)
# =============================================================================
# 这些标志对 Compile 和 Link 都有效
set(MCU_FLAGS -mcpu=cortex-m0 -mthumb)

add_compile_options(${MCU_FLAGS})
add_link_options(${MCU_FLAGS})

# =============================================================================
# 编译器特定标志
# =============================================================================
if(USING_CLANG)
    # 编译选项
    add_compile_options(
        -fshort-enums 
        -fno-rtti
        
        # 仅 C++ 编译时需要 stdlib 定义 (解决 C 编译警告的核心)
        $<$<COMPILE_LANGUAGE:CXX>:-stdlib=libc++>
    )
    
    # 链接选项
    add_link_options(
        -Wno-unused-command-line-argument
        -fuse-ld=lld
        -stdlib=libc++
        -rtlib=compiler-rt
        -unwindlib=libunwind
        -nostartfiles
    )

elseif(USING_GCC)
    # GCC 编译选项
    add_compile_options(
        --specs=picolibc.specs
        -mthumb-interwork
    )
    # GCC 链接选项 (specs 需要传递给 Linker)
    add_link_options(
        --specs=picolibc.specs
    )
endif()

# =============================================================================
# 基础构建与优化选项
# =============================================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

add_compile_options(
    -ffunction-sections 
    -fdata-sections 
    -fno-common 
    -fmessage-length=0 
    -fno-exceptions
    $<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g -Wall)
else()
    add_compile_options(-Ofast -Wall)
endif()

# =============================================================================
# 链接脚本与其他
# =============================================================================
set(TARGET_LD_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/n32g031_flash.ld)
set(TARGET_STARTUP_ASM ${CMAKE_CURRENT_LIST_DIR}/startup_n32g031_gcc.s)

add_link_options(
    -T ${TARGET_LD_SCRIPT}
    -Wl,-gc-sections
    -Wl,--print-memory-usage
    -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map
)

# =============================================================================
# 辅助函数
# =============================================================================
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

find_package(Python3 COMPONENTS Interpreter REQUIRED)
set(Python_EXECUTABLE ${Python3_EXECUTABLE})

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
