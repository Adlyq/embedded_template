#!/usr/bin/env python3
import json
import os
import subprocess
import sys
import re

script_dir = os.path.dirname(os.path.abspath(__file__))
presets_path = os.path.join(script_dir, 'CMakePresets.json')
cmakelists_path = os.path.join(script_dir, 'CMakeLists.txt')

def get_project_name() -> str:
    """从 CMakeLists.txt 中解析项目名称"""
    if not os.path.exists(cmakelists_path):
        return "project"

    with open(cmakelists_path, 'r', encoding='utf-8') as f:
        content = f.read()
        # 寻找 project(NAME ...) 或 add_executable(NAME ...)
        # 优先寻找 project()
        match = re.search(r'project\s*\(\s*(\w+)', content, re.IGNORECASE)
        if match:
            return match.group(1)

        # 降级寻找 add_executable
        match = re.search(r'add_executable\s*\(\s*\${PROJECT_NAME}', content, re.IGNORECASE)
        if not match:
            match = re.search(r'add_executable\s*\(\s*(\w+)', content, re.IGNORECASE)
            if match:
                return match.group(1)

    return "project"

def get_debug_presets() -> list[str]:
    """从 CMakePresets.json 中解析所有非隐藏的 Debug 预设"""
    if not os.path.exists(presets_path):
        print(f"错误: 找不到 {presets_path}")
        return []

    with open(presets_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    presets = []
    for preset in data.get('configurePresets', []):
        name = preset.get('name', '')
        if name.startswith('Debug-') and not preset.get('hidden', False):
            presets.append(name)
    return presets

def select_preset(presets: list[str]) -> str:
    """如果存在多个预设，让用户进行选择"""
    if len(presets) == 1:
        return presets[0]

    print("\n找到多个 Debug 预设，请选择:")
    for i, name in enumerate(presets):
        print(f"[{i + 1}] {name}")

    while True:
        try:
            choice = input(f"\n请输入序号 (1-{len(presets)}): ")
            idx = int(choice) - 1
            if 0 <= idx < len(presets):
                return presets[idx]
        except ValueError:
            pass
        print(f"输入无效，请输入 1 到 {len(presets)} 之间的数字。")

def main():
    project_name = get_project_name()
    presets = get_debug_presets()
    if not presets:
        print("未在 CMakePresets.json 中找到任何匹配的 Debug 预设 (以 'Debug-' 开头)。")
        sys.exit(1)

    selected_preset = select_preset(presets)
    print(f"\n>>> 项目名称: {project_name}")
    print(f">>> 选定预设: {selected_preset}")

    # 1. 配置预设
    print(f"\n>>> 正在配置预设...")
    config_cmd = f'cmake --preset {selected_preset}'
    subprocess.run(config_cmd, shell=True, check=True)

    # 2. 执行构建
    build_path = os.path.join(script_dir, 'build', selected_preset)
    print(f"\n>>> 正在构建项目 (目录: {build_path})...")
    # 不指定 target，编译默认 target (通常就是可执行文件)
    build_cmd = f'cmake --build {build_path} -j 14'
    subprocess.run(build_cmd, shell=True, check=True)

    if '-k' in sys.argv or '--skip' in sys.argv: return

    # 3. 寻找生成的二进制文件
    # 策略：在构建根目录下寻找与项目名同名的文件 (不带后缀或带 .elf)
    possible_names = [project_name, f"{project_name}.elf"]
    bin_file = None

    for root, _, files in os.walk(build_path):
        for name in files:
            if name in possible_names:
                bin_file = os.path.join(root, name)
                break
        if bin_file: break

    if not bin_file:
        print(f"错误: 找不到生成的二进制文件 (预期名称: {project_name})。")
        sys.exit(1)

    # 4. 使用 probe-rs 运行
    print(f"\n>>> 正在使用 probe-rs 运行: {bin_file}")

    # 构造参数列表列表
    args = [
        "probe-rs",
        "run",
        "--chip=N32G031K8",
        "--protocol=swd",
        "--chip-description-path=/home/adlyq/.config/probe-rs/targets/N32G031_Series.yaml",
        bin_file
    ]

    # 直接执行 exec 替换当前进程，该函数不会返回
    os.execvp(args[0], args)

if __name__ == "__main__":
    main()