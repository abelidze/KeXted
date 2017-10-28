@echo off
echo Compiling...
if not exist bin mkdir bin
gcc -std=gnu11 main.c kex.c utility.c -s -o .\bin\kexted.exe -L. -l:pdcurses.a -I.\include
echo Done!