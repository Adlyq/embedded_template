#!/usr/bin/env python3
import itertools
import subprocess
import shutil
from pathlib import Path
import time

# --- 配置区 ---
SCRIPT_DIR = Path(__file__).parent.absolute()
BUILD_ROOT = SCRIPT_DIR / "build" / "release_all"
DIST_DIR = SCRIPT_DIR / "dist"

# 编译器路径 (与 CMakePresets.json 保持一致)
COMPILERS = {
    "CMAKE_C_COMPILER": "/home/adlyq/.local/share/LLVM-ET-Arm/bin/clang",
    "CMAKE_CXX_COMPILER": "/home/adlyq/.local/share/LLVM-ET-Arm/bin/clang++",
    "CMAKE_ASM_COMPILER": "/home/adlyq/.local/share/LLVM-ET-Arm/bin/clang",
}

def get_params() -> dict[str, list[str]]:
    return {
        "type": ["PNP", "NPN"],
        "mode": ["LIGHT_CLOSE", "LIGHT_OPEN"],
    }

def main():
    params = get_params()
    keys = list(params.keys())
    combinations = list(itertools.product(*params.values()))

    # 准备目录 (保留构建根目录，清空产物输出目录)
    if DIST_DIR.exists():
        shutil.rmtree(DIST_DIR)
    DIST_DIR.mkdir(parents=True, exist_ok=True)
    
    # 第一次运行建议清空构建根目录，或者保留它进行增量更新
    # if BUILD_ROOT.exists(): shutil.rmtree(BUILD_ROOT)
    
    print(f"🚀 开始构建所有组合 (共 {len(combinations)} 个)...")
    start_time = time.time()

    for combo in combinations:
        # combo = ('PNP', 'LIGHT_CLOSE')
        combo_id = "_".join(combo)
        # 为每个组合创建独立的子构建目录，支持增量编译且不互干扰
        combo_build_dir = BUILD_ROOT / combo_id
        
        # 构造 Output 宏变量 (分号分隔)
        output_val = ";".join(combo)
        
        print(f"\n📦 构建组合: {combo_id}")

        # 1. CMake 配置
        config_cmd = [
            "cmake",
            "-S", str(SCRIPT_DIR),
            "-B", str(combo_build_dir),
            "-G", "Ninja",
            "-DCMAKE_BUILD_TYPE=Release",
            f"-DOutput={output_val}",
            "-DCMAKE_C_FLAGS=-DLOCK_MCU",
            "-DCMAKE_CXX_FLAGS=-DLOCK_MCU",
            "--toolchain=cmake/toolchain.cmake",
        ]
        # 添加编译器路径
        for k, v in COMPILERS.items():
            config_cmd.append(f"-D{k}={v}")

        subprocess.run(config_cmd, check=True)

        # 2. CMake 构建
        build_cmd = ["cmake", "--build", str(combo_build_dir), "-j", "14"]
        subprocess.run(build_cmd, check=True)

        # 3. 收集产物
        # 扫描构建目录下的 .hex 文件 (CMakeLists.txt 已根据 Output 修改了文件名)
        hex_files = list(combo_build_dir.glob("*.hex"))
        for hf in hex_files:
            shutil.copy2(hf, DIST_DIR / hf.name)
            print(f"  ✅ 产物已拷贝: {hf.name}")

    print("\n" + "="*40)
    # 4. 后处理 (Hash 计算)
    try:
        # 尝试运行 hex-hash
        subprocess.run(f"hex-hash -anq {DIST_DIR}", shell=True, check=True)
    except Exception:
        print("💡 提示: hex-hash 执行失败或未找到工具，跳过 Hash 计算。")

    end_time = time.time()
    print(f"🎉 全部版本构建完成！耗时: {end_time - start_time:.2f}s")
    print(f"📂 最终产物存放在: {DIST_DIR}")

if __name__ == "__main__":
    main()
