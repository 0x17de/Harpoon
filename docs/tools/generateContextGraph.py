#!/usr/bin/env python3
import re
import pathlib
import itertools

moduleMap={}

def findUsedModules():
    with open('./src/app/Application.cpp', 'r') as f:
        lines = f.read().split('\n')

    res=[]
    module_regex = re.compile('.*initializeModule\\(([^,]+),.*')
    for line in lines:
        m = module_regex.match(line)
        if not m is None:
            res.append(m.group(1).replace('"', ''))
    return res

def findModuleProviders():
    res = []
    provide_regex = re.compile('.*PROVIDE_MODULE\\("([^"]+)", "([^"]+)", ([^)]+)\\).*')
    for filepath in pathlib.Path('.').glob("src/**/*.cpp"):
        filepath = str(filepath)
        with open(filepath, 'r') as f:
            lines = f.read().split('\n')
        for line in lines:
            m = provide_regex.match(line)
            if not m is None:
                res.append({'category': m.group(1),
                            'name': m.group(2),
                            'class': m.group(3),
                            'file': filepath})
                break
    return res

def findInheritance():
    res = []
    class_regexp = re.compile('^[ \t]*class .*(public|private|protected)')

    step1_rewrite = re.compile('^[ \t]*class ([^ ]+)[ ]*:')
    step2_remove_scope = re.compile('[ \t]*(public|private|protected)[ \t]+')
    step3_remove_spaces = re.compile('[ \t]*,[ \t]*')
    step4_fix_ending = re.compile('[ \t]*{$')

    p = pathlib.Path('.')
    for filepath in itertools.chain(p.glob('src/**/*.cpp'), p.glob('src/**/*.hpp')):
        filepath = str(filepath)
        with open(filepath, 'r') as f:
            lines = f.read().split('\n')
        for line in lines:
            if not class_regexp.search(line) is None:
                line = re.sub(step1_rewrite, '\"\\1\" -> { "', line, count=1)
                line = re.sub(step2_remove_scope, '', line)
                line = re.sub(step3_remove_spaces, '", "', line)
                line = re.sub(step4_fix_ending, '" }', line, count=1)
                res.append(line)
    return res


if __name__ == '__main__':
    modules=findUsedModules()
    providers=findModuleProviders()
    inheritance=findInheritance()

    with open('docs/input/initialization.dot', 'w') as f:
        f.write('digraph Initialization {\n')
        f.write('rankdir=LR;')
        f.write('app[label="Application\nsrc/app/Application.cpp"];\n')
        f.write('servprov[label="ModuleProvider\nsrc/utils/ModuleProvider.cpp"];\n')
        f.write('app -> servprov [label="'+'\n'.join(modules)+'"];\n')
        
        for provider in providers:
            f.write('servprov -> "'+provider['class']+'\n'+provider['file']+'" [label="'+provider['category']+':'+provider['name']+'"];\n')
        f.write('}\n')

    with open('docs/input/inheritance.dot', 'w') as f:
        f.write('digraph Initialization {\n')
        f.write('rankdir=LR;')
        f.write('\n'.join(inheritance))
        f.write('\n}\n')

