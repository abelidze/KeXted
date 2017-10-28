#ifndef KEXUTILS_INCLUDED
#define KEXUTILS_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kex.h>

#define KEX_CAPTION "KeXted 2.2.8"

#define WIDTH  120
#define HEIGHT 40
#define TITLEH 1
#define MENUH 1
#define ROWSW 4

#define BUFFERSIZE 1024
#define STACKSIZE 2048
#define BRAINSIZE 65536
#define BRAINCOMMANDS 8

extern char file_name[BUFFERSIZE];

extern void new_file();
extern void open_file();
extern void load_file();
extern void save_file();
extern void build_file();

#endif