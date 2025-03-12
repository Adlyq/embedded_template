#!/usr/bin/env python3
import os
import sys
import xml.etree.ElementTree as et

def normalize_path(_path) -> str:
    return str(os.path.normpath(_path).replace('\\', '/'))


def same_drive(path1, path2):
    if os.name == 'nt':  # Windows
        return os.path.splitdrive(path1)[0] == os.path.splitdrive(path2)[0]
    else:  # Linux and other OS
        return True


TARGET_NAME = 'Target_1'
GROUP_NAME = 'Source Group 1'
PROJECT_DIR = normalize_path(os.getcwd())
KEIL_DIR = normalize_path(f'{os.getcwd()}/keil')
DEFAULT_FILE_TYPE = 9
FILE_TYPE = {'c': 1, 'cpp': 8, 's': 2, 'h': 5, 'txt': 5}

argInc, argSrc = [], []
flag = True
for arg in sys.argv[1:]:
    if arg == '-i':
        flag = True
    elif arg == '-s':
        flag = False
    else:
        (argInc if flag else argSrc).append(normalize_path(arg))

tree = et.parse('keil/keil.uvprojx')
root = tree.getroot()

target = root.find(f'.//Targets/Target[TargetName="{TARGET_NAME}"]')
incdir = target.find('.//VariousControls/IncludePath')

group = target.find(f'.//Groups/Group[GroupName="{GROUP_NAME}"]')
groupFiles = group.find('Files') if group is not None else None
if groupFiles is None:
    groupFiles = et.Element('Files')
    if group is None:
        group = et.Element('Group')
        group.append(et.Element('GroupName', text=GROUP_NAME))
        target.find('.//Groups').append(group)
    group.append(groupFiles)

incDirs = set(incdir.text.split(';') if incdir.text else [])
newIncDirs = {os.path.relpath(ai, 'keil').replace('\\', '/') for ai in argInc}
for d in incDirs:
    absD = normalize_path(os.path.abspath(os.path.join('keil', d)))
    if (os.path.isabs(d) or
            not same_drive(absD, PROJECT_DIR) or
            normalize_path(os.path.commonpath([absD, PROJECT_DIR])) != PROJECT_DIR or
            normalize_path(os.path.commonpath([absD, KEIL_DIR])) == KEIL_DIR):
        newIncDirs.add(d)
incdir.text = ';'.join(sorted(newIncDirs))

newFiles = [
    {
        'FileName': os.path.basename(asc),
        'FileType': FILE_TYPE.get(os.path.splitext(asc)[1][1:], DEFAULT_FILE_TYPE),
        'FilePath': normalize_path(os.path.relpath(asc, 'keil')),
    } for asc in argSrc
]
for file in groupFiles:
    path = file.find('FilePath').text
    absPath = normalize_path(os.path.abspath(os.path.join('keil', path)))
    if (os.path.isabs(path) or
            not same_drive(absPath, PROJECT_DIR) or
            normalize_path(os.path.commonpath([absPath, PROJECT_DIR])) != PROJECT_DIR or
            normalize_path(os.path.commonpath([absPath, KEIL_DIR])) == KEIL_DIR):
        newFiles.append({
            'FileName': file.find('./FileName').text,
            'FileType': file.find('./FileType').text,
            'FilePath': path,
        })

groupFiles.clear()
for file in newFiles:
    fileElem = et.Element('File')
    for key, value in file.items():
        et.SubElement(fileElem, key).text = str(value)
    groupFiles.append(fileElem)
et.indent(group, space='  ', level=4)

with open('keil/keil.uvprojx', 'rb') as f:
    backup = f.readlines()

output = f'''<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<Project xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="project_projx.xsd">

  {''.join([
    et.tostring(ele, encoding='utf-8', xml_declaration=False, short_empty_elements=(ele.tag in ['LayerInfo', 'RTE']))
    .decode('utf-8')
    .replace(' />', '/>')
    for ele in root.findall('*')
])}</Project>
'''

if output == backup:
    print('No changes detected.')
    sys.exit(0)

with open('keil/keil.uvprojx', 'w') as f:
    f.write(output)
