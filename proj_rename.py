#!/usr/bin/env python3

import os
import shutil
import sys

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

oldStr = [
    'n32g031_template',
    'n32g031-template',
    'embedded-template',
    'embedded_template',
]
ctx_list = []
for path, dirs, files in os.walk('./.idea'):
    for file in files:
        if file.find('embedded-template') != -1:
            ctx_list.append((path, file))
        with open(os.path.join(path, file), 'r', encoding='utf-8') as f:
            content = f.read()
            for osr in oldStr:
                content = content.replace(osr, sys.argv[1])
        with open(os.path.join(path, file), 'w', encoding='utf-8') as f:
            f.write(content)
    for _dir in dirs:
        if _dir.find('embedded-template') != -1:
            ctx_list.append((path, _dir))

for path, file in ctx_list:
    os.renames(os.path.join(path, file), os.path.join(path, file.replace('embedded-template', sys.argv[1])))

knownFiles = [
    './CMakeLists.txt',
    '.github/workflows/build.yml',
    '.run/n32g031_template.run.xml',
]
for kf in knownFiles:
    with (open('.github/workflows/build.yml', 'r', encoding='utf-8') as f):
        content = f.read()
        for osr in oldStr:
            content = content.replace(osr, sys.argv[1])
    with open('.github/workflows/build.yml', 'w', encoding='utf-8') as f:
        f.write(content)

os.renames('.run/n32g031_template.run.xml', '.run/' + sys.argv[1] + '.run.xml')
if os.path.basename(os.getcwd()) != sys.argv[1]:
    os.renames(os.getcwd(), os.path.abspath('../' + sys.argv[1]))

if os.path.exists('sdk/cmsis-n32g031'):
    shutil.rmtree('sdk/cmsis-n32g031')
if os.path.exists('sdk/cmsis-gd32f30x'):
    shutil.rmtree('sdk/cmsis-gd32f30x')
if os.path.exists('sdk/cmsis-dsp'):
    shutil.rmtree('sdk/cmsis-dsp')
if os.path.exists('.git'):
    shutil.rmtree('.git')

os.system('git init && git submodule add -f --depth 1 https://github.com/Adlyq/cmsis-gd32f30x.git sdk/cmsis-gd32f30x')

os.remove('proj_rename.py')
