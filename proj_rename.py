#!/usr/bin/env python3

import os, sys, shutil

if len(sys.argv) < 2:
    print("Usage: python proj_rename.py <new_project_name>")
    sys.exit(1)

for char in sys.argv[1]:
    if char not in 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-':
        print("Invalid project name. Only alphanumeric characters and underscores are allowed.")
        sys.exit(1)

if os.path.exists('./../' + sys.argv[1]) and (os.path.basename(os.getcwd()) != sys.argv[1]):
    print(f"Directory '{sys.argv[1]}' already exists. Please choose a different project name.")
    sys.exit(1)

ctx_list = []
for path, dirs, files in os.walk('./.idea'):
    for file in files:
        if file.find('embedded-template') != -1:
            ctx_list.append((path, file))
        with open(os.path.join(path, file), 'r', encoding='utf-8') as f:
            content = f.read().replace('n32g031_template', sys.argv[1]).replace('embedded-template', sys.argv[1]).replace("embedded_template", sys.argv[1])
        with open(os.path.join(path, file), 'w', encoding='utf-8') as f:
            f.write(content)
    for _dir in dirs:
        if _dir.find('embedded-template') != -1:
            ctx_list.append((path, _dir))


for path, file in ctx_list:
    os.renames(os.path.join(path, file), os.path.join(path, file.replace('embedded-template', sys.argv[1])))

with open('./CMakeLists.txt', 'r', encoding='utf-8') as f:
    content = f.read().replace('n32g031_template', sys.argv[1])
with open('./CMakeLists.txt', 'w', encoding='utf-8') as f:
    f.write(content)

with open('.github/workflows/build.yml', 'r', encoding='utf-8') as f:
    content = f.read().replace('n32g031_template', sys.argv[1]).replace('embedded-template', sys.argv[1]).replace("embedded_template", sys.argv[1])
with open('.github/workflows/build.yml', 'w', encoding='utf-8') as f:
    f.write(content)

if os.path.basename(os.getcwd()) != sys.argv[1]:
    os.renames(os.getcwd(), os.path.abspath('../' + sys.argv[1]))

if os.path.exists('sdk/cmsis'):
    shutil.rmtree('sdk/cmsis')
if os.path.exists('sdk/cmsis-dsp'):
    shutil.rmtree('sdk/cmsis-dsp')
if os.path.exists('.git'):
    shutil.rmtree('.git')

os.system('git init && git submodule add -f --depth 1 https://github.com/Adlyq/n32g031-cmsis.git sdk/cmsis')

os.remove('proj_rename.py')