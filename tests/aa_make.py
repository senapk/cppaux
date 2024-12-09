#!/usr/bin/env python3

import os

# get all files in the current directory that end with .cpp
cpp_files = [file[:-4] for file in os.listdir() if file.endswith('.cpp')]

for file in cpp_files:
    
    test_command = f"tko test {file}.cpp zz_{file}.tio"
    print("$ " + test_command)
    os.system(test_command)
