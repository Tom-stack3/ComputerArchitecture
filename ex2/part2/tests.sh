#!/bin/bash

# Made by Tommy Zaft

python3 create_tests.py # create the main2__test.c and the correct_funcs.c functions files

gcc main2__test.c correct_funcs.c -o __correct
./__correct > correct_results.txt
rm -f __correct # delete the unnecessary binary

gcc main2__test.c ex2.s -o __p2
./__p2 > results_got.txt
rm -f __p2 # delete the unnecessary binary

diff -s correct_results.txt results_got.txt # compare your results to the correct ones
