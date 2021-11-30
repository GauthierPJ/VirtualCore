#!/bin/sh

echo "\n#-------------- PYTHON COMPILATOR --------------#\n";
./compilator.py asm_1.s
make clean 
make 
echo "\n#-------------- VIRTUAL CORE --------------#\n";
./main code.bin