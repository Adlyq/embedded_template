# n32g031-cmake-template
n32g031的cmake模板，包含了keil工程文件，cmake文件，和一些常用的库

可以做到与keil工程文件同步，方便使用cmake编译和调试

## 约定
> 1. keil工程文件在./keil/keil.uvprojx
> 2. 格式化代码使用clang-format， 格式使用.clang-format文件中的配置
> 3. **头文件放在./Core/inc中, 源文件放在./Core/src中**
> 4. **cmake中，除syscall.c的源文件都需要放在`SRC_LIST`变量中统一add_executable，以保证与keil同步**
> 5. 若完全遵循第三条则可忽略第四条，无需改动CMakeLists.txt