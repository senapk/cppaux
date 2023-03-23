#!/usr/bin/env python3

import os

# get all files in the current directory that end with .cpp
cpp_files = [file[:-4] for file in os.listdir() if file.endswith('.cpp')]

for file in cpp_files:
    executable_name = file + '.out'
    compile_command = f"g++ -Wall -std=c++17 {file}.cpp -o {executable_name}"
    print("$ " + compile_command)
    os.system(compile_command)
    execute_command = f"./{executable_name} > {file}.received"
    print("$ " + execute_command)
    os.system(execute_command)
    diff_command = f"diff {file}.received {file}.expected"
    print("$ " + diff_command)
    os.system(diff_command)
    os.system(f"rm {executable_name} {file}.received")