#!/bin/python3

def is_block_comment(line):
    if line.startswith("/**"):
        return True    
    if line.startswith(" *"):
        return True
    if line.startswith("*/"):
        return True
    if line.startswith(" */"):
        return True
    return False

lines = open("fn.hpp").read().split("\n")

output = []
size = len(lines)
i = 0

while(i < size):
    if lines[i].startswith("//__guide"):
        label = lines[i].split(" ")[1]
        print("label: ", label)
        output.append(lines[i])
        i += 1
        if i < size and lines[i].startswith("//"):
            output.append(lines[i])
            i += 1
        while (True):
            if is_block_comment(lines[i]):
                i += 1
                continue
            output.append(lines[i])
            i += 1
            break
    else:
        output.append(lines[i])
        i += 1


with open("fn.hpp", "w") as f:
    f.write("\n".join(output))



                