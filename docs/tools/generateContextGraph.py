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

def findWebsocketProtocolReceive():
    currentProtocol = {'name':'', 'commands':[]} # no protocol
    res=[currentProtocol]
    currentCommand = None
    protocol_regex = re.compile('.* if[ \t]*\\(protocol == "([^"]+)"\\)')
    cmd_regex = re.compile('.* if[ ]*\\(cmd == "([^"]+)"\\)')
    json_element_regex = re.compile('.*root\\.get\\("([^"]+)"[^)]+\\)\\.as([^)]+)\\(\\)')
    with open('src/server/ws/WebsocketHandler.cpp', 'r') as f:
        lines = f.read().split('\n')
    for line in lines:
        m = protocol_regex.match(line)
        if not m is None:
            currentProtocol = {'name': m.group(1), 'commands':[]}
            res.append(currentProtocol)
        m = cmd_regex.match(line)
        if not m is None:
            currentCommand = {'name':m.group(1), 'elements':[]}
            currentProtocol['commands'].append(currentCommand)
        m = json_element_regex.match(line)
        if not m is None:
            currentCommand['elements'].append({'name':m.group(1), 'type':m.group(2)})
    return res

if __name__ == '__main__':
    modules=findUsedModules()
    providers=findModuleProviders()
    inheritance=findInheritance()
    websocket_protocol_receive=findWebsocketProtocolReceive()
    commandIndex = 0

    with open('docs/input/initialization.dot', 'w') as f:
        f.write('digraph Initialization {\n')
        f.write('rankdir=LR;\n')
        f.write('app[label="Application\nsrc/app/Application.cpp"];\n')
        f.write('servprov[label="ModuleProvider\nsrc/utils/ModuleProvider.cpp"];\n')
        f.write('app -> servprov [label="'+'\n'.join(modules)+'"];\n')
        
        for provider in providers:
            f.write('servprov -> "'+provider['class']+'\n'+provider['file']+'" [label="'+provider['category']+':'+provider['name']+'"];\n')
        f.write('}\n')

    with open('docs/input/inheritance.dot', 'w') as f:
        f.write('digraph Initialization {\n')
        f.write('rankdir=LR;\n')
        f.write('\n'.join(inheritance))
        f.write('\n}\n')

    with open('docs/input/websocketprotocol.dot', 'w') as f:
        f.write('digraph Initialization {\n')
        f.write('rankdir=LR;\n')
        f.write('fontname="Bitstream Vera Sans";\n');
        #f.write('fontsize=8;\n');
        f.write('node [\n');
        f.write('fontname="Bitstream Vera Sans";\n');
        #f.write('fontsize=8;\n');
        f.write('shape="record";\n');
        f.write(']\n');
        f.write('edge [\n');
        f.write('fontname="Bitstream Vera Sans";\n');
        #f.write('fontsize=8;\n');
        f.write(']\n');
        for protocol in websocket_protocol_receive:
            if protocol['name'] == '':
                protocol['name'] = '--WITHOUT--'
            for command in protocol['commands']:
                f.write('command_'+str(commandIndex)+' [ label="{'+command['name']+'|')
                if not command['name'] == 'LOGIN':
                    for element in command['elements']:
                        f.write('+ '+element['name']+' : '+element['type']+'\l')
                f.write('}" ];\n')
                f.write('"'+protocol['name']+'" -> command_'+str(commandIndex)+';\n')
                commandIndex = commandIndex + 1
                    
        
        f.write('\n}\n')

