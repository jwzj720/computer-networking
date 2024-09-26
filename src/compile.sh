#!bin/bash

echo "What file do you want to compile?"
read file
echo "Compiling main.c"
gcc -Wall -pthread -o main $file -lpigpiod_if2 -lrt