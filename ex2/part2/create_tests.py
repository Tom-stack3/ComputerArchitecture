#!/usr/bin/env python3

# Made by Tommy Zaft

import random

NUM_OF_TESTS = 100
RANGE_OF_NUMBERS = 69

test_file = "#include <stdio.h>\n\
int even(int num, int i);\n\
int go(int A[10]);\n\
int main()\n\
{\n"
for i in range(NUM_OF_TESTS):
    randomlist = []
    for _ in range(0, 10):
        n = random.randint(-RANGE_OF_NUMBERS, RANGE_OF_NUMBERS)
        randomlist.append(n)
    test_file += f'\tint array{i}[10] = {{ {", ".join(str(x) for x in randomlist)} }};\n' + \
        f'\tprintf("%d\\n", go(array{i}));\n'
test_file += "\treturn 0;\n}"

with open("main2__test.c", "w") as f:
    f.write(test_file)

correct_funcs_file = "#include <stdio.h>\nint even(int num, int i){\n\treturn num << i;\n}\n\
int go(int A[10]){\n\tint sum = 0;\n\tint i = 0;\n\twhile (i < 10){\n\
\t\tif (A[i] % 2 == 0){\n\t\t\tint num = even(A[i], i);\n\t\t\tsum += num;\n\t\t}\n\t\telse{\n\t\t\tsum += A[i];\n\t\t}\n\t\ti++;\n\t}\n\treturn sum;\n}"

with open("correct_funcs.c", "w") as f:
    f.write(correct_funcs_file)
