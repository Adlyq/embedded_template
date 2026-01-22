#!/usr/bin/env python3
import itertools
import os
import shutil
import subprocess

script_dir = os.path.dirname(__file__)

build_dir = os.path.join(script_dir, 'build/cmake-build-release-all')

CMAKE_C_COMPILER='/home/adlyq/.local/share/LLVM-ET-Arm/bin/clang'
CMAKE_CXX_COMPILER='/home/adlyq/.local/share/LLVM-ET-Arm/bin/clang++'

def get_params() -> dict[str, list[str]]:
    return {
        "type": ["PNP", "NPN"],
        "mode": ["LIGHT_CLOSE", "LIGHT_OPEN"],
        # 以后可继续添加参数，如 "feature": ["A", "B"]
    }


def base_name(combo_dict: dict[str, str]) -> str | None:
    return None


def main(params: dict[str, list[str]]):
    """
    构建所有参数组合的 release 版本
    :param params: 参数字典，键为参数名，值为参数取值列表
    :return: None
    """
    # 生成所有排列组合
    keys = list(params.keys())
    combinations = list(itertools.product(*params.values()))

    if os.path.exists(build_dir):
        shutil.rmtree(build_dir)

    os.makedirs(build_dir, exist_ok=True)

    for combo in combinations:
        combo_dict = dict(zip(keys, combo))
        # 构建 Output 变量，分号分隔
        output_val = ";".join(combo)

        base_file_name = base_name(combo_dict)

        # STM32 特定的 CMake 配置命令
        cmake_config_cmd = (
            f'cmake -DCMAKE_BUILD_TYPE=Release '
            f'-DCMAKE_MAKE_PROGRAM=ninja '
            f'-DCMAKE_C_COMPILER={CMAKE_C_COMPILER} '
            f'-DCMAKE_CXX_COMPILER={CMAKE_CXX_COMPILER} '
            f'-DCMAKE_TOOLCHAIN_FILE=cmake/toolchain.cmake '
            f'-G Ninja '
            f'-DOutput="{output_val}" '
            f'{f'-DFILE_NAME={base_file_name} ' if base_file_name else ''}'
            f'-DCMAKE_C_FLAGS="-DLOCK_MCU" '
            f'-DCMAKE_CXX_FLAGS="-DLOCK_MCU" '
            f'-S . -B {build_dir}'
        )

        # STM32 特定的构建命令
        cmake_build_cmd = f'cmake --build {build_dir} -j 14'

        # 执行配置
        subprocess.run(cmake_config_cmd, shell=True, check=True)

        # 执行构建
        subprocess.run(cmake_build_cmd, shell=True, check=True)

    for root, dirs, files in os.walk(build_dir, topdown=False):
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


if __name__ == "__main__":
    main(params=get_params())
