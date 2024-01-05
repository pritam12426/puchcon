#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

int terminal_col_size(void) {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
}

void printNTime(const char _what, const unsigned _time, bool _new_line) {
	for (int i = 1; i <= _time; i++) {
		putchar((int) _what);
	}

	if (_new_line) putchar((int) '\n');
}
