# 设置语言标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# 设置C/C++/ASM可执行文件后缀为.elf
set(CMAKE_EXECUTABLE_SUFFIX_C   .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

# 针对裸机交叉编译器的安全设置
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 根据构建类型设置编译选项
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    # Release模式优化级别O3，开启警告
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3 -Wall")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3 -Wall")
else ()
    # Debug模式不优化，保留调试信息
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS} -O0 -g -Wall")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -O0 -g -Wall")
endif ()

# 添加ARM Cortex-M0目标处理器相关编译选项
add_compile_options(-mcpu=cortex-m0 -mthumb -mthumb-interwork)
# 添加优化相关编译选项
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -fno-exceptions)
# 为ASM文件添加特殊编译选项
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

# 自定义链接脚本路径
set(TARGET_LD_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/n32g031_flash.ld)
# 自定义启动文件路径
set(TARGET_STARTUP_ASM ${CMAKE_CURRENT_LIST_DIR}/startup_n32g031_gcc.s)

# 添加链接选项
add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=cortex-m0 -mthumb -mthumb-interwork)
add_link_options(-T ${TARGET_LD_SCRIPT})

if(NOT CMAKE_OBJCOPY)
    # 首先获取编译器的路径
    get_filename_component(COMPILER_PATH ${CMAKE_C_COMPILER} DIRECTORY)
    # 在编译器的同级目录中查找 arm-none-eabi-objcopy
    find_program(
            CMAKE_OBJCOPY arm-none-eabi-objcopy
            HINTS ${COMPILER_PATH}
            REQUIRED
    )
    message(STATUS "Found objcopy: ${CMAKE_OBJCOPY}")
endif()

# 查找Python解释器
if (NOT Python_EXECUTABLE)
    find_package(Python REQUIRED COMPONENTS Interpreter)
    if (NOT Python_EXECUTABLE)
        message(FATAL_ERROR "Python executable not found")
    endif ()
endif ()

# 查找并启用ccache以加速编译
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

# 生成Keil项目函数
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