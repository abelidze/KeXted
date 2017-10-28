#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <curses.h>
#include <kex.h>

static pair precursor, cursor;
static WINDOW *wTitle, *wMenu, *wRows, *wBody;
static GUIMenu *GUIMain;
static LineListT *lines;
static int quit = 0, rWidth, bWidth, bHeight, tHeight, mHeight;

chtype getcolor(chtype color) {
	chtype attr = color & A_ATTR;				
	return COLOR_PAIR(color & A_CHARTEXT) | attr;
}

void setcolor(WINDOW *win, chtype color) {
	wattrset(win, getcolor(color));
}

void fillcolor(WINDOW *win, chtype color, int x, int y) {
	chtype symbol = mvwinch(win, y, x);

	setcolor(win, color);
	mvwaddch(win, y, x, symbol & A_CHARTEXT);
	setcolor(win, symbol & A_ATTRIBUTES);
}

void KeX_colors() {
	init_pair(TITLECOLOR & ~A_ATTR, COLOR_YELLOW, COLOR_BLACK);
	init_pair(MENUCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_BLACK);
	init_pair(BODYCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_RED);
	init_pair(CURSORCOLOR & ~A_ATTR, COLOR_BLACK, COLOR_YELLOW);
	init_pair(MESSAGECOLOR & ~A_ATTR, COLOR_RED, COLOR_WHITE);
}

void KeX_init(int w, int h, int th, int mh, int rw, char *caption, GUIMenu *gui) {
	setlocale(LC_ALL, "Russian");

	TABSIZE = 1;
	initscr();
	// nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	resize_term(h, w);
	start_color();
	halfdelay(1);
	noecho();
	raw();
	nonl();
	mouse_set(ALL_MOUSE_EVENTS);

	clear();
	refresh();

	wTitle = subwin(stdscr, th, w, 0, 0),
	wMenu = subwin(stdscr, mh, h, th, 0),
	wRows = subwin(stdscr, h - th - mh, rw, th + mh, 0),
	wBody = subwin(stdscr, h - th - mh, w - rw, th + mh, rw);

	KeX_colors();

	wbkgd(wTitle, getcolor(TITLECOLOR));
	wbkgd(wMenu,  getcolor(MENUCOLOR));
	wbkgd(wRows,  getcolor(MENUCOLOR) & ~A_BOLD);
	wbkgd(wBody,  getcolor(BODYCOLOR) & ~A_BOLD);

	// Title
	tHeight = th;
	mvwaddstr(wTitle, 0, w/2 - 6, caption);

	// Menu
	mHeight = mh;
	int l = 0;
	for(GUIButton *bt = gui->element; bt->func; l += bt->len, ++bt)
		mvwaddstr(wMenu, 0, l, bt->name);
	gui->size = l;

	// Body
	bWidth = w - rw;
	bHeight = h - th - mh - 1;
	scrollok(wBody, 1);
	fillcolor(wBody, CURSORCOLOR, 0, 0);

	//Refresh ALL
	wrefresh(wTitle);
	wrefresh(wMenu);
	wrefresh(wRows);
	wrefresh(wBody);

	touchwin(wBody);
	curs_set(0);

	GUIMain = gui;
	lines = KeX_init_lines(w - rw);

	// Left Numbers
	rWidth = rw;
	KeX_redraw_numbers(1);
}

void KeX_exit() {
	quit = 1;
}

void KeX_clear_lines(int refresh) {
	if(!lines) return;

	lines->current = lines->head = lines->first;
	lines->current->text[0] = lines->current->enter = lines->current->len = 0;

	while(lines->current->next)
		KeX_deleteline(lines, lines->current->next);

	lines->count = 0;
	lines->headnum = 1;
	lines->headline = 1;
	lines->curline = 1;

	KeX_update_cursor(0, 0);
	if(refresh)
		KeX_redraw();
}

LineListT* KeX_init_lines(char width) {
	LineListT* list = (LineListT *) malloc(sizeof(LineListT));
	list->count = 0;
	list->headnum = 1;
	list->headline = 1;
	list->curline = 1;
	list->width = width;
	list->head = list->first = list->current = KeX_newline(list, NULL, NULL);
	return list;
}

LineListT* KeX_get_lines() {
	return lines;
}

LineT* KeX_newline(LineListT *list, LineT *prev, LineT *next) {
	if(!list) return NULL;

	LineT *line = (LineT *) malloc(sizeof(LineT));
	line->text = (char *) malloc(list->width + 1);
	line->prev = prev;
	line->next = next;
	line->text[0] = line->enter = line->len = 0;
	if(prev)
		prev->next = line;
	if(next)
		next->prev = line;
	list->count++;
	return line;
}

LineT* KeX_deleteline(LineListT *list, LineT *line) {
	if(!line || !list) return NULL;

	LineT *temp = line->prev;
	if(line->prev)
		line->prev->next = line->next;
	if(line->next)
		line->next->prev = line->prev;
	free(line->text);
	free(line);

	list->count--;
	return temp;
}

LineT* KeX_nextline(LineT *line) {
	if(!line) return NULL;

	if(line->next)
		line = line->next;
	return line;
}

LineT* KeX_prevline(LineT *line) {
	if(!line) return NULL;

	if(line->prev)
		line = line->prev;
	return line;
}

LineT* KeX_shiftline(LineT *line, int i) {
	if(!line) return NULL;
	
	for(; i > 0 && line->next; --i)
		line = line->next;
	for(; i < 0 && line->prev; ++i)
		line = line->prev;
	return line;
}

void KeX_updateline(LineT *line, int pos) {
	if(!line) return;

	mvwinnstr(wBody, pos, 0, line->text, line->len);
	line->text[line->len] = 0;
}

void KeX_redrawline(LineT *line, int pos) {
	if(!line) return;

	mvwaddstr(wBody, pos, 0, line->text);
}

void KeX_redraw() {
	if(!lines) return;

	wclear(wBody);
	setcolor(wBody, BODYCOLOR);
	LineT *line = lines->head;
	for(int i = 0; i < bHeight && line; ++i, line = line->next)
		mvwaddstr(wBody, i, 0, line->text);

	wrefresh(wBody);
}

void KeX_redraw_numbers(int pos) {
	if(!lines) return;

	touchwin(wRows);
	wclear(wRows);
	char num[32] = "";
	LineT *line = lines->head;
	for(int i = 0; i <= bHeight && line; ++i, line = line->next) {
		if(line == lines->head || (line->prev && line->prev->enter)) {
			itoa(pos++, num, 10);
			mvwaddstr(wRows, i, 0, num);
		}
	}
	wrefresh(wRows);
	touchwin(wBody);
}

void KeX_redraw_cursor() {
	fillcolor(wBody, BODYCOLOR, precursor.x, precursor.y);
	fillcolor(wBody, CURSORCOLOR, cursor.x, cursor.y);
	setcolor(wBody, BODYCOLOR);
}

void KeX_update_cursor(int x, int y) {
	precursor = cursor;
	cursor.x = x;
	cursor.y = y;
}

void KeX_loop() {
	GUIButton *lastSelected = NULL;
	int x, y, len = 0;
	char buffer[1024];
	chtype ch = ERR;

	while(!quit) {
		if((ch = wgetch(wBody)) == ERR) continue;

		precursor = cursor;

		switch(ch) {
			case KEY_MOUSE:
				request_mouse_pos();
				if(BUTTON_STATUS(1) == BUTTON_RELEASED || BUTTON_STATUS(1) == BUTTON_CLICKED) {
					// mvwprintw(wBody, 0, 4, "\n(%d; %d)\n", MOUSE_X_POS, MOUSE_Y_POS);
					KeX_updateline(lines->current, cursor.y);
					if(MOUSE_Y_POS < tHeight + mHeight) {
						wmouse_position(wMenu, &y, &x);
						if(x < GUIMain->size) {
							int l = 0;
							GUIButton *newSelect = NULL;
							for(GUIButton *bt = GUIMain->element; bt->func; l += bt->len, ++bt) {
								if(lastSelected == bt)
									mvwaddstr(wMenu, 0, l, bt->name);
								if(x > l && x < l + bt->len) {
									setcolor(wMenu, MSELCOLOR);
									mvwaddstr(wMenu, 0, l, bt->name);
									setcolor(wMenu, MENUCOLOR);
									bt->func();
									newSelect = bt;

									KeX_redraw_numbers(lines->headnum);
								}
							}
							lastSelected = newSelect;
							wrefresh(wMenu);
						}
					}
					else if(MOUSE_X_POS >= rWidth){
						wmouse_position(wBody, &y, &x);
						cursor.y = (y < lines->count) ? y : lines->count - 1;
						lines->current = KeX_shiftline(lines->current, cursor.y - (lines->curline - lines->headline));
						cursor.x = (lines->current->len > x) ? x : lines->current->len;
						lines->curline = lines->headline + cursor.y;
					}
				}
			break;

			case ALT_Q:
			case KEY_ESC:
				KeX_exit();
			break;

			case CTRL('D'):
				len = lines->current->len;
				mvwinnstr(wBody, cursor.y, 0, buffer, len);

				lines->current->enter = 1;
				lines->current = KeX_newline(lines, lines->current, lines->current->next);
				lines->current->enter = 1;
				lines->current->len = len;

				if(cursor.y < bHeight) cursor.y++;
				else {
					if(lines->head->enter)
						lines->headnum++;
					lines->head = KeX_nextline(lines->head);
					wscrl(wBody, 1);
				}

				lines->curline++;
				mvwinsertln(wBody, cursor.y, 0);
				mvwinsstr(wBody, cursor.y, 0, buffer);

				mvwaddch(wBody, cursor.y - 1, len, 10);
				mvwinsch(wBody, cursor.y - 1, 0, 'F');
				wrefresh(wBody);
				mvwdelch(wBody, cursor.y - 1, 0);
				KeX_updateline(lines->current, cursor.y - 1);
				lines->current = KeX_nextline(lines->current);

				KeX_redraw_numbers(lines->headnum);
			break;

			case ALT_D:
				if(cursor.y > 0) {
					mvwdeleteln(wBody, cursor.y, 0);
					lines->current = KeX_deleteline(lines, lines->current);
					cursor.x = lines->current->len, cursor.y--, lines->curline--;
				}

				KeX_redraw_numbers(lines->headnum);
			break;

			case 10:
			case 13:
			case KEY_ENTER:
				len = lines->current->len - cursor.x;
				lines->current->enter = 1;
				KeX_newline(lines, lines->current, lines->current->next);

				if(cursor.y < bHeight) cursor.y++;
				else {
					if(lines->head->enter)
						lines->headnum++;
					lines->head = KeX_nextline(lines->head);
					wscrl(wBody, 1);
				}

				lines->curline++;
				mvwinsertln(wBody, cursor.y, 0);

				if(len > 0) {
					mvwinnstr(wBody, cursor.y - 1, cursor.x, buffer, len);
					mvwinsstr(wBody, cursor.y, 0, buffer);
					lines->current->len -= len;
					KeX_nextline(lines->current)->len = len;
				}
				KeX_nextline(lines->current)->enter = 1;
				
				mvwaddch(wBody, cursor.y - 1, cursor.x, 10);
				mvwinsch(wBody, cursor.y - 1, 0, 'F');
				wrefresh(wBody);
				mvwdelch(wBody, cursor.y - 1, 0);
				KeX_updateline(lines->current, cursor.y - 1);
				lines->current = KeX_nextline(lines->current);
				cursor.x = 0;

				KeX_redraw_numbers(lines->headnum);
			break;

			case '\b':
			case KEY_BACKSPACE:
				if(cursor.x > 0) {
					cursor.x--;
					lines->current->len--;
					mvwdelch(wBody, cursor.y, cursor.x);
				}
				else if(lines->curline > 1) {
					if(cursor.y == 0 && lines->head->prev) {
						if(lines->head->enter)
							lines->headnum--;
						lines->head = KeX_prevline(lines->head);
						lines->headline--;
						wscrl(wBody, -1);
						KeX_redrawline(lines->head, 0);
						precursor.y++;
					}
					else cursor.y--;

					len = lines->current->len;
					if(lines->current->len > 0) {
						len = bWidth - KeX_prevline(lines->current)->len;
						mvwinnstr(wBody, cursor.y + 1, 0, buffer, len);
						mvwinsstr(wBody, cursor.y, bWidth - len, buffer);
						mvwinnstr(wBody, cursor.y + 1, len, buffer, lines->current->len - len);
					}

					mvwdeleteln(wBody, cursor.y + 1, 0);
					if(len < lines->current->len) {
						lines->current->enter = 0;
						mvwinsstr(wBody, cursor.y, bWidth - len, buffer);
						lines->current = KeX_prevline(lines->current);
					}
					else {
						len = lines->current->len;
						lines->current = KeX_deleteline(lines, lines->current);
					}

					cursor.x = lines->current->len, lines->current->len += len, lines->curline--;
					KeX_updateline(lines->current, cursor.y);
					if(lines->current->next == NULL)
						lines->current->enter = 0;

					KeX_redraw_numbers(lines->headnum);
				}
			break;

			case KEY_DC:
				if(cursor.x < lines->current->len) {
					lines->current->len--;
					mvwdelch(wBody, cursor.y, cursor.x);
				}
			break;

			case KEY_HOME:
				cursor.x = 0;
			break;

			case KEY_END:
				cursor.x = lines->current->len;
			break;

			case KEY_UP:
				if(lines->curline > 1) {
					KeX_updateline(lines->current, cursor.y);
					lines->current = KeX_prevline(lines->current);
					lines->curline--;

					if(cursor.y == 0 && lines->head->prev) {
						if(lines->head->enter)
							lines->headnum--;
						lines->head = KeX_prevline(lines->head);
						lines->headline--;
						wscrl(wBody, -1);
						KeX_redrawline(lines->head, 0);
						KeX_redraw_numbers(lines->headnum);
						precursor.y++;
					}
					else cursor.y--;

					if(cursor.x+1 > lines->current->len)
						cursor.x = lines->current->len;
				}
			break;

			case KEY_DOWN:
				if(lines->curline < lines->count) {
					KeX_updateline(lines->current, cursor.y);
					lines->current = KeX_nextline(lines->current);
					lines->curline++;

					if(cursor.y < bHeight) cursor.y++;
					else {
						if(lines->head->enter)
							lines->headnum++;
						lines->head = KeX_nextline(lines->head);
						lines->headline++;
						wscrl(wBody, 1);
						KeX_redrawline(lines->current, cursor.y);
						KeX_redraw_numbers(lines->headnum);
						precursor.y--;

					}

					if(cursor.x+1 > lines->current->len)
						cursor.x = lines->current->len;
				}
			break;

			case KEY_LEFT:
				cursor.x = (cursor.x > 0) ? cursor.x-1 : 0;
			break;

			case KEY_RIGHT:
				cursor.x = (cursor.x < lines->current->len) ? cursor.x+1 : lines->current->len;
			break;

			default:
				if(lines->current->len < bWidth - 1) {
					mvwinsch(wBody, cursor.y, cursor.x, ch);
					cursor.x++;
					lines->current->len++;
				}
		}
		KeX_redraw_cursor();

		wrefresh(wBody);
	}

	endwin();
	system("cls");
}

void KeX_message(const char *str, int x, int y) {
	int w = 24,
		h = 7,
		i = 0,
		last = 0,
		len = 0;

	for(i = 0; str[i] != 0; ++i) {
		if(str[i] == '\n') {
			len = MAX(len , i - last);
			last = i + 1;
		}
	}
	len = MAX(len , i - last);
	w = MAX(w, 8 + len);

	WINDOW *wMessage = newwin(h, w, y - h / 2, x - w / 2),
	       *wBack    = newwin(h, w + 2, y - h / 2, x - w / 2 - 1);
	wbkgd(wMessage, getcolor(MESSAGECOLOR));
	wbkgd(wBack, getcolor(MESSAGECOLOR));
	mvwaddstr(wMessage, 3, 4, str);
	touchwin(wBack);
	wrefresh(wBack);
	touchwin(wMessage);
	wrefresh(wMessage);

	while(getch() == ERR) {}
	delwin(wMessage);
	delwin(wBack);

	touchwin(wBody);
	KeX_redraw();
}

void KeX_input(const char *caption, char *str, int x, int y) {
	int w = 64,
		h = 6,
		i = 0,
		last = 0,
		len = 0,
		s_len = strlen(str);

	for(i = 0; caption[i] != 0; ++i) {
		if(caption[i] == '\n') {
			len = MAX(len , i - last);
			last = i + 1;
		}
	}
	len = MAX(len , i - last);
	w = MAX(w, 10 + len);

	WINDOW *wInput = newwin(h, w - 2, y - h / 2, x - w / 2 + 1),
	       *wBack  = newwin(h, w, y - h / 2, x - w / 2);

	curs_set(1);
	wbkgd(wInput, getcolor(MESSAGECOLOR));
	wbkgd(wBack, getcolor(MESSAGECOLOR));
	mvwaddstr(wInput, 1, 1, caption);
	mvwaddstr(wInput, 3, 1, str);
	pair cur = {s_len + 1, 3};

	touchwin(wBack);
	wrefresh(wBack);
	touchwin(wInput);
	wrefresh(wInput);

	int ch, input = 1;
	while(input) {
		if((ch = wgetch(wInput)) == ERR) continue;

		switch(ch) {
			case 13:
			case '\n':
			case KEY_ESC:
				input = 0;
			break;

			case '\b':
			case KEY_BACKSPACE:
				if(cur.x > 1) {
					--cur.x, str[--s_len] = 0;
					wmove(wInput, cur.y, cur.x);
					wdelch(wInput);
				}
			break;

			default:
				if(cur.x < w - 3) {
					++cur.x, str[s_len++] = (char) ch;
					waddch(wInput, ch);
				}
		}

		wrefresh(wInput);
	}
	curs_set(0);
	delwin(wInput);
	delwin(wBack);

	touchwin(wBody);
	KeX_redraw();
}