import ctags
from ctags import CTags, TagEntry
import sys

tagFile = CTags('tags')
entry = TagEntry()
status = tagFile.first(entry)

function_list = {}

while True:
    if entry['kind'] == b'function' and entry['file'].decode()[-2:] == '.c':
        functions = function_list.get(entry['file'], [])
        functions.append({'line': entry['lineNumber']-1, 'pattern': entry['pattern']})
        function_list[entry['file']] = functions

    if tagFile.next(entry) != 1:
        break

for file_name, functions in function_list.items():
    with open(file_name, 'rb') as source_file:
        source = source_file.readlines()
    
    functions.sort(key=lambda func: func['line'])
    line_added = 0
    for func in functions:
        if b"fptrgroup" not in source[func['line']+line_added-1] and \
           b"fptrgroup" not in source[func['line']+line_added] and \
           b"LV_ATTRIBUTE_FAST_MEM" not in source[func['line']+line_added-1] and \
           b"LV_ATTRIBUTE_FAST_MEM" not in source[func['line']+line_added] and \
           b"IS_NOT_USED" not in source[func['line']+line_added]:
            source.insert(func['line']+line_added, b"LV_FUNC_SECTION\n")
            line_added += 1

    with open(file_name, 'wb') as source_file:
        source_file.writelines(source)
    print(file_name, len(functions))
    # break