About KeXted
==============

KeXted is a simple console text editor with built-in Brainfuck interpreter.

Written in Native-C using PDCurses library.

Installation
------------

Download project as '.zip' archive and extract it to whatever you like directory or use `git`:
```sh
$ git clone https://github.com/abelidze/KeXted
```
	
Requirements
------------

* PDCurses (tested and working with v3.4)
* Currently not compatible with ncurses stuff

Running
-------

> **MANUAL**

```sh
if not exist bin mkdir bin
gcc -std=gnu11 main.c kex.c utility.c -s -o .\bin\kexted.exe -L. -l:pdcurses.a -I.\include
```

***
> **AUTOMATIC**

There are simple 'Makefile' that you can easily use.
* Tested with MinGW x64 and its `make` utility, but you can use any working C / C++ compiler.

Contact
-------

Developers on Telegram:

[![https://telegram.me/Abelidze](https://img.shields.io/badge/%E2%9E%A4Telegram-@Abelidze-DD2200.svg?style=flat-square&&colorA=2D233B)](https://telegram.me/Abelidze)

License
-------
KeXted is open-sourced software licensed under the [MIT License](http://opensource.org/licenses/MIT).

#### Disclaimer

This program is provided "AS IS" without warranty of any kind.