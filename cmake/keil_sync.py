#!/usr/bin/env python3

import os
import sys
import xml.etree.ElementTree as et

TARGET_NAME = 'Target_1'
GROUP_NAME = 'Source Group 1'
PROJECT_DIR = os.getcwd()
KEIL_DIR = f'{os.getcwd()}/keil'
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
        (argInc if flag else argSrc).append(arg)

tree = et.parse('keil/keil.uvprojx')
root = tree.getroot()

target = root.find(f'.//Targets/Target[TargetName="{TARGET_NAME}"]')
incdir = target.find('.//VariousControls/IncludePath')

group = target.find(f'.//Groups/Group[GroupName="{GROUP_NAME}"]')
groupFiles = group.find('Files') if group else None
if groupFiles is None:
    groupFiles = et.Element('Files')
    if group is None:
        group = et.Element('Group')
        group.append(et.Element('GroupName', text=GROUP_NAME))
        target.find('.//Groups').append(group)
    group.append(groupFiles)

incDirs = set(incdir.text.split(';') if incdir.text else [])
newIncDirs = {os.path.relpath(ai, 'keil') for ai in argInc}
for d in incDirs:
    absD = os.path.abspath(os.path.join('keil', d.replace('\\', '/')))
    if (os.path.isabs(d) or
            os.path.commonpath([absD, PROJECT_DIR]) != PROJECT_DIR or
            os.path.commonpath([absD, KEIL_DIR]) == KEIL_DIR):
        newIncDirs.add(d)
incdir.text = ';'.join(sorted(newIncDirs))

newFiles = [
    {
        'FileName': os.path.basename(asc),
        'FileType': FILE_TYPE.get(os.path.splitext(asc)[1][1:], DEFAULT_FILE_TYPE),
        'FilePath': os.path.relpath(asc, 'keil'),
    } for asc in argSrc
]
for file in groupFiles:
    path = file.find('FilePath').text
    absPath = os.path.abspath(os.path.join('keil', path.replace('\\', '/')))
    if (os.path.isabs(path) or
            os.path.commonpath([absPath, PROJECT_DIR]) != PROJECT_DIR or
            os.path.commonpath([absPath, KEIL_DIR]) == KEIL_DIR):
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
    if ele.tag in ['LayerInfo', 'RTE'] else
    et.tostring(ele, encoding='utf-8', xml_declaration=False).decode('utf-8')
    for ele in root.findall('*')
])}</Project>
'''

if output == backup:
    print('No changes detected.')
    sys.exit(0)

with open('keil/keil.uvprojx', 'w') as f:
    f.write(output)
