#!/usr/bin/env python3
import json
import os
import shutil
import subprocess
import sys

script_dir = os.path.dirname(os.path.abspath(__file__))
presets_path = os.path.join(script_dir, 'CMakePresets.json')
dist_dir = os.path.join(script_dir, 'build/release-dist')


def get_release_presets() -> list[str]:
    """从 CMakePresets.json 中解析所有非隐藏的 Release 预设"""
    with open(presets_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    presets = []
    for preset in data.get('configurePresets', []):
        name = preset.get('name', '')
        # 获取所有以 Release- 开头且非隐藏的预设
        if name.startswith('Release-') and not preset.get('hidden', False):
            presets.append(name)
    return presets


def has_uncommitted_changes():
    try:
        # 运行 git status --porcelain
        result = subprocess.run(
            ['git', 'status', '--porcelain'],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            check=True
        )
        # 如果输出不为空，则说明有未提交的变更
        return len(result.stdout.strip()) > 0
    except subprocess.CalledProcessError:
        # 如果不是git目录或者git命令不存在
        print("Not a git repository or git not installed.")
        exit(1)


def main(args: Arguments):
    """
    使用 CMake Presets 构建所有 Release 版本预设
    """

    if not args.skip_git_check and has_uncommitted_changes():
        print("检测到未提交的变更，请先提交或暂存这些变更后再运行此脚本。")
        exit(1)

    presets = get_release_presets()
    if not presets:
        print("未在 CMakePresets.json 中找到任何匹配的 Release 预设。")
        return

    if os.path.exists(dist_dir):
        shutil.rmtree(dist_dir)
    os.makedirs(dist_dir, exist_ok=True)

    print(f"找到以下预设: {', '.join(presets)}")

    for preset in presets:
        print(f"\n>>> 正在构建预设: {preset}")

        build_path = os.path.join(script_dir, 'build', preset)
        if os.path.exists(build_path):
            shutil.rmtree(build_path)

        # 1. 配置预设
        config_cmd = f'cmake --preset {preset}'
        try:
            subprocess.run(config_cmd, shell=True, check=True)
        except subprocess.CalledProcessError as e:
            print(f"`{config_cmd}` run fail")
            print(f"配置预设 {preset} 失败: {e}")
            exit(1)

        # 2. 执行构建
        # 注意：构建预设如果未定义，可以直接指定构建目录
        # 这里假设 binaryDir 在 presets 中已定义为 build/{presetName}
        build_cmd = f'cmake --build {build_path} -j 14'
        try:
            subprocess.run(build_cmd, shell=True, check=True)
        except subprocess.CalledProcessError as e:
            print(f"`{build_cmd}` run fail")
            print(f"构建预设 {preset} 失败: {e}")
            exit(1)

        # 3. 收集产物 (.hex 文件)
        # 查找构建目录下的所有 hex 文件并复制到发布目录
        for root, _, files in os.walk(build_path):
            for name in files:
                if name.endswith(".hex"):
                    src_file = os.path.join(root, name)
                    shutil.copy(src_file, dist_dir)
                    print(f"已收集产物: {name}")

    # 4. 执行哈希校验 (可选)
    try:
        subprocess.run(f'hex-hash -anq {dist_dir}', shell=True, check=True)
    except Exception:
        print("提示: 未能运行 hex-hash，请确保已安装该工具。")

    print(f"\n全部 Release 版本已构建完成，产物位于: {dist_dir}")


class Arguments:
    def __init__(self):
        for arg in sys.argv:
            if arg == '--skip-git-check' or arg == '-k':
                self.skip_git_check = True

    skip_git_check = False


if __name__ == "__main__":
    main(args=Arguments())
