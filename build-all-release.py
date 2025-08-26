#!/usr/bin/env python3
import itertools
import os
import subprocess

# 可扩展参数字典
params = {
    "type": ["PNP", "NPN"],
    "mode": ["LIGHT_CLOSE", "LIGHT_OPEN"],
    # 以后可继续添加参数，如 "feature": ["A", "B"]
}

# 生成所有排列组合
keys = list(params.keys())
combinations = list(itertools.product(*params.values()))

for combo in combinations:
    combo_dict = dict(zip(keys, combo))
    # 构建 build 目录名
    build_dir = "cmake-build-release-all"
    os.makedirs(build_dir, exist_ok=True)
    # 构建 Output 变量，分号分隔
    output_val = ";".join(combo)
    
    # STM32 特定的 CMake 配置命令
    cmake_config_cmd = (
        f'cmake -DCMAKE_BUILD_TYPE=Release '
        f'-DCMAKE_MAKE_PROGRAM=ninja '
        f'-DCMAKE_C_COMPILER=arm-none-eabi-gcc '
        f'-DCMAKE_CXX_COMPILER=arm-none-eabi-g++ '
        f'-G Ninja '
        f'-DOutput="{output_val}" '
        f'-S . -B {build_dir}'
    )
    
    # STM32 特定的构建命令
    cmake_build_cmd = f'cmake --build {build_dir} -j 14'
    
    # 执行配置
    subprocess.run(cmake_config_cmd, shell=True, check=True)
    
    # 执行构建
    subprocess.run(cmake_build_cmd, shell=True, check=True)

for root, dirs, files in os.walk("cmake-build-release-all", topdown=False):
    for name in files:
        if not name.endswith(".hex"):
            os.remove(os.path.join(root, name))
    for name in dirs:
        dir_path = os.path.join(root, name)
        try:
            os.rmdir(dir_path)
        except OSError:
            pass
print("全部release版本已构建完成。")
