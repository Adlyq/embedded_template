#!/usr/bin/env python3
import os
import shutil
import sys
from pathlib import Path

def main():
    if len(sys.argv) < 2:
        print(f"用法: python {sys.argv[0]} <新项目名称>")
        sys.exit(1)

    new_name = sys.argv[1].strip()
    
    # 基础校验：仅允许字母、数字、下划线和连字符
    if not all(c.isalnum() or c in '-_' for c in new_name):
        print("错误: 项目名称只能包含字母、数字、下划线和连字符。")
        sys.exit(1)

    project_root = Path(__file__).parent.absolute()
    old_names = [
        'n32g031_template',
        'n32g031-template',
        'embedded-template',
        'embedded_template',
    ]

    print(f"🔄 正在将项目重命名为: {new_name}...")

    # 1. 递归替换文件内容
    # 定义需要扫描的文件类型或特定文件
    target_files = []
    # 扫描特定目录
    for folder in ['.idea', '.run', '.github', 'keil']:
        folder_path = project_root / folder
        if folder_path.exists():
            target_files.extend(list(folder_path.rglob('*')))
    
    # 添加根目录下的核心配置文件
    for root_file in ['CMakeLists.txt', 'README.md', 'pyproject.toml']:
        target_files.append(project_root / root_file)

    for file_path in target_files:
        if file_path.is_file() and not file_path.name.endswith(('.pyc', '.pyo', '.git')):
            try:
                content = file_path.read_text(encoding='utf-8')
                new_content = content
                for old in old_names:
                    new_content = new_content.replace(old, new_name)
                
                if new_content != content:
                    file_path.write_text(new_content, encoding='utf-8')
                    print(f"  📝 已更新内容: {file_path.relative_to(project_root)}")
            except (UnicodeDecodeError, PermissionError):
                continue

    # 2. 重命名包含旧名称的文件或文件夹 (特别是 .iml 和 .run.xml)
    # 再次扫描以进行物理命名更改
    for folder in ['.idea', '.run']:
        folder_path = project_root / folder
        if not folder_path.exists(): continue
        
        # 使用 list 固化搜索结果，避免在重命名过程中改变迭代结构
        items = sorted(list(folder_path.rglob('*')), key=lambda x: len(str(x)), reverse=True)
        for item in items:
            for old in old_names:
                if old in item.name:
                    new_item_name = item.name.replace(old, new_name)
                    new_item_path = item.parent / new_item_name
                    item.rename(new_item_path)
                    print(f"  📂 已重命名: {item.relative_to(project_root)} -> {new_item_name}")
                    break

    # 3. 清理旧的 SDK 和 Git 信息 (模板初始化逻辑)
    print("🧹 正在清理 SDK 缓存和 Git 历史...")
    sdk_dirs = ['sdk/cmsis-n32g031', 'sdk/cmsis-gd32f30x', 'sdk/cmsis-n32g031-dsp']
    for s_dir in sdk_dirs:
        path = project_root / s_dir
        if path.exists():
            shutil.rmtree(path)

    git_dir = project_root / ".git"
    if git_dir.exists():
        shutil.rmtree(git_dir)

    # 4. 初始化新 Git 仓库并添加 SDK 子模块
    print("🌐 正在初始化新 Git 仓库并拉取 SDK...")
    try:
        subprocess_args = {"cwd": str(project_root), "shell": True, "check": True}
        import subprocess
        subprocess.run("git init", **subprocess_args)
        subprocess.run("git submodule add -f --depth 1 https://github.com/Adlyq/n32g031-cmsis.git sdk/cmsis-n32g031", **subprocess_args)
    except Exception as e:
        print(f"⚠️ Git 初始化失败 (可能未安装 Git 或网络连接问题): {e}")

    # 5. 重命名项目根目录 (如果当前目录名不匹配)
    parent_dir = project_root.parent
    new_project_root = parent_dir / new_name

    # 6. 删除脚本自身并尝试重命名根目录
    script_path = Path(__file__)
    print(f"✨ 重命名完成。脚本 {script_path.name} 即将自毁。")
    script_path.unlink()

    if project_root.name != new_name:
        if new_project_root.exists():
            print(f"⚠️ 警告: 目标目录 {new_project_root} 已存在，无法自动重命名根目录。")
        else:
            print(f"🚀 请手动或等待脚本尝试重命名根目录为: {new_name}")
            try:
                # 注意：在某些系统上，重命名正在运行脚本的当前目录可能会受限
                os.rename(str(project_root), str(new_project_root))
            except Exception:
                print("💡 请手动将当前文件夹重命名以完成最后一步。")

if __name__ == "__main__":
    main()
