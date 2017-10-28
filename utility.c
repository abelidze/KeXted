#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kex.h>
#include <utility.h>

static char brain_commands[BRAINCOMMANDS] = {'<', '>', '+', '-', '.', ',', '[', ']'};

char file_name[BUFFERSIZE] = "";

void new_file() {
	KeX_clear_lines(1);
}

void open_file() {
	KeX_input("Enter yout testing file:", file_name, WIDTH/2, HEIGHT/2);
	load_file();
}

void load_file() {
	FILE *file = fopen(file_name, "r");
	if(!file) {
		KeX_message("Can't open the file.", WIDTH/2, HEIGHT/2);
		return;
	}

	KeX_clear_lines(0);
	LineListT *list = KeX_get_lines();
	LineT *line = list->first;
	for(int i = 0; !feof(file); ++i) {
		fgets(line->text, WIDTH-ROWSW, file);
		line->len = strlen(line->text);
		if(line->text[line->len-1] == 10) {
			line->text[--line->len] = 0;
			line->enter = 1;
		}
		line = KeX_newline(list, line, NULL);
	}
	list->curline = 1;
	KeX_update_cursor(0, 0);
	KeX_redraw_numbers(1);
	KeX_redraw();
	fclose(file);
}

void save_file() {
	FILE *file = fopen(file_name, "w");
	if(!file) {
		KeX_message("Can't save the file.", WIDTH/2, HEIGHT/2);
		return;
	}

	LineT *line = KeX_get_lines()->first;
	for(int i = 0; line; line = line->next, ++i) {
		fprintf(file, "%s", line->text);
		if(line->enter)
			putc('\n', file);
	}
	fclose(file);

	KeX_message("Saved!", WIDTH/2, HEIGHT/2);
}

void build_file() {
	int c_len = 0,
		buf = 0,
		st = 0,
		stack[STACKSIZE];

	char bf[BRAINSIZE],
		 buffer[BUFFERSIZE],
		 commands[BRAINSIZE];

	KeX_message("Press any key to continue...", WIDTH/2, HEIGHT/2);

	for(LineT *line = KeX_get_lines()->first; line; line = line->next)
		for(int i = 0; line->text[i] != 0 && i < line->len; ++i)
			for(int c = 0; c < BRAINCOMMANDS; ++c)
				if(line->text[i] == brain_commands[c])
					commands[c_len++] = line->text[i];
	commands[c_len] = 0;

	memset(bf, 0, BRAINSIZE);
	memset(buffer, 0, sizeof(buffer));
	memset(stack, 0, sizeof(stack));

	int stop = 0, i = 0, c = 0;
	while(!stop) {
		switch(commands[c++]) {
			case 0: stop = 1; break;
			case '<': i = (i - 1 + BRAINSIZE) % BRAINSIZE; break;
			case '>': i = (i + 1) % BRAINSIZE; break;
			case '+': ++bf[i]; break;
			case '-': --bf[i]; break;
			case ',': bf[i] = 0; break;
			case '.': (buf < BUFFERSIZE) ? (buffer[buf++] = bf[i]) : (stop = 2); break;

			case '[':
				if(st >= STACKSIZE) { stop = 3; break; }

				if(bf[i] != 0) stack[st++] = c;
				else while(c < c_len && commands[c] != ']') c++;
			break;

			case ']':
				if(bf[i] != 0) c = stack[st-1];
				else st--;

				if(st < 0) stop = 4;
			break;
		}
	}
	if(st > 0) stop = 4;
	else buffer[buf] = 0;

	switch(stop) {
		case 1: KeX_message(buffer, WIDTH/2, HEIGHT/2); break;
		case 2: KeX_message("ERROR: Buffer Overflow", WIDTH/2, HEIGHT/2); break;
		case 3: KeX_message("ERROR: Stack Overflow", WIDTH/2, HEIGHT/2); break;
		case 4: KeX_message("ERROR: Unbalanced '['-']'", WIDTH/2, HEIGHT/2); break;

		default: KeX_message("ERROR: Unhandled", WIDTH/2, HEIGHT/2);
	}
}