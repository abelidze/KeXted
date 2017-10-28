/*************************/
/* |\  /|   /\   | |\  | */
/* | \/ |  /__\  | | \ | */
/* |    | /    \ | |  \| */
/*************************/
#include <kex.h>
#include <utility.h>

GUIMenu guiMain =
{
	0,
	{
		{ " New ",   new_file   , 5},
		{ " Open ",  open_file  , 6},
		{ " Save ",  save_file  , 6},
		{ " Build ", build_file , 7},
		{ " Exit ",  KeX_exit   , 6},
		{ "", NULL, 0 }
	}
};
// _Static_assert(sizeof(guiMain) == 8, "Check");

int main(int argc, char *argv[]) {
	int i = 1, opened = 0, build = 0;
	while(i < argc) {
		char *par = argv[i];
		if(*par == '-') {
			par++;
			switch(*par) {
				case 'b':
					build = 1;
				break;

				case 'h':
					printf(
"Usage: %s [options] [input]\n\
List of options:\n\
  -b		Open and build file\n\
  -v		Print program version\n\
  -h		Show this help\n",
  *argv
);
					return 0;

				case 'v':
					printf(KEX_CAPTION);
					return 0;

				default:
					printf("usage: %s [options] [input]\n", *argv);
					return 0;
			}
		}
		else if(!opened) {
			strcpy(file_name, argv[i]);
			opened = 1;
		}
		++i;
	}

	KeX_init(WIDTH, HEIGHT, TITLEH, MENUH, ROWSW, KEX_CAPTION, &guiMain); // All defines are placed in "utility.h"

	if(opened) {
		load_file();
		if(build)
			build_file();
	}

	KeX_loop();	

	return 0;
}