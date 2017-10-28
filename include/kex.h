#ifndef KEX_INCLUDED
#define KEX_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

#define A_ATTR       (A_ATTRIBUTES ^ A_COLOR)
#define TITLECOLOR   (1 | A_BOLD)
#define MENUCOLOR    (2 | A_BOLD)
#define MSELCOLOR    (2 | A_BOLD | A_BLINK)
#define BODYCOLOR    (3 | A_BOLD)
#define CURSORCOLOR  (4 | A_BLINK)
#define MESSAGECOLOR (5 | A_BOLD)

#define KEY_ESC 0x1b
#define CTRL(c) ((c) & 037)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef void (*FUNC)(void);

typedef struct {
	int x;
	int y;
} pair;

typedef struct {
	char *name;
	FUNC  func;
	int   len;
} GUIButton;

typedef struct {
	int size;
	GUIButton element[];
} GUIMenu;

typedef struct _line {
	struct _line *prev;
	struct _line *next;
	char *text;
	char enter;
	char len;
} LineT;

typedef struct {
	LineT *first;
	LineT *head;
	LineT *current;
	int headnum;
	int headline;
	int curline;
	int count;
	char width;
} LineListT;

extern chtype getcolor(chtype color);
extern void setcolor(WINDOW *win, chtype color);
extern void fillcolor(WINDOW *win, chtype color, int x, int y);
extern void KeX_colors();
extern void KeX_init(int w, int h, int th, int mh, int rw, char *caption, GUIMenu *gui);
extern void KeX_loop();
extern void KeX_exit();
extern void KeX_clear_lines();
extern LineListT* KeX_init_lines(char width);
extern LineListT* KeX_get_lines();
extern LineT* KeX_newline(LineListT *list, LineT *prev, LineT *next);
extern LineT* KeX_deleteline(LineListT *list, LineT *line);
extern LineT* KeX_nextline(LineT *line);
extern LineT* KeX_prevline(LineT *line);
extern LineT* KeX_shiftline(LineT *line, int i);
extern void KeX_updateline(LineT *line, int pos);
extern void KeX_redrawline(LineT *line, int pos);
extern void KeX_redraw();
extern void KeX_redraw_numbers(int pos);
extern void KeX_redraw_cursor();
extern void KeX_update_cursor(int x, int y);
extern void KeX_message(const char *str, int x, int y);
extern void KeX_input(const char *caption, char *str, int x, int y);

#endif