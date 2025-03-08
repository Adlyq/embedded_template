import os
import sys
import xml.etree.ElementTree as et

TARGET_NAME = 'Target_1'
GROUP_NAME = 'Source Group 1'
PROJECT_DIR = os.getcwd()
KEIL_DIR = f'{os.getcwd()}/keil'
DEFAULT_FILE_TYPE = 9
FILE_TYPE = {'c': 1, 'cpp': 8, 's': 2, 'h': 5, 'txt': 5}

flag = True
argInc = []
argSrc = []
for arg in sys.argv[1:]:
    if arg == '-i':
        flag = True
    elif arg == '-s':
        flag = False
    else:
        if flag:
            argInc.append(arg)
        else:
            argSrc.append(arg)

tree = et.parse('keil/keil.uvprojx')
root = tree.getroot()

# find the incdir
target = root.find(f'.//Targets/Target[TargetName="{TARGET_NAME}"]')
incdir = target.find('.//VariousControls/IncludePath')

# find the groupFiles
groupFiles = target.find(f'.//Groups/Group[GroupName="{GROUP_NAME}"]/Files')
if groupFiles is None:
    groupFiles = et.Element('Files')
    group = target.find(f'.//Groups/Group[GroupName="{GROUP_NAME}"]')
    if group is None:
        group = et.Element('Group')
        group.append(et.Element('GroupName', text=GROUP_NAME))
        target.find('.//Groups').append(group)
    group.append(groupFiles)

# generate the new xml
incDirs = set(incdir.text.split(';') if incdir.text else [])
newIncDirs = set(os.path.relpath(ai, 'keil') for ai in argInc)
for d in incDirs:
    absD = os.path.abspath(os.path.join('keil', d.replace('\\', '/')))
    if (os.path.isabs(d)) or (
            os.path.commonpath([absD, PROJECT_DIR]) != PROJECT_DIR) or (
            os.path.commonpath([absD, KEIL_DIR]) == KEIL_DIR):
        newIncDirs.add(d)

incdir.text = ';'.join(newIncDirs)

newFiles = [
    {
        'FileName': os.path.basename(asc),
        'FileType': FILE_TYPE.get(os.path.splitext(asc)[1][1:], DEFAULT_FILE_TYPE),
        'FilePath': os.path.relpath(asc, 'keil'),
    } for asc in argSrc
]
for file in groupFiles:
    path = file.find('./FilePath').text
    absPath = os.path.abspath(os.path.join('keil', path.replace('\\', '/')))
    if (os.path.isabs(path)) or (
            os.path.commonpath([absPath, PROJECT_DIR]) != PROJECT_DIR) or (
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
        child = et.SubElement(fileElem, key)
        child.text = str(value)
    groupFiles.append(fileElem)

tree.write('keil/keil.uvprojx', xml_declaration=True, short_empty_elements=False)
