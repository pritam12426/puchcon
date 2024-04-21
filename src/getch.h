#include <stdio.h>
#include <unistd.h>
#include <termios.h>

int getch() {
	struct termios old_termios, new_termios;

	// Get current terminal settings
	tcgetattr(STDIN_FILENO, &old_termios);
	new_termios = old_termios;

	// Disable canonical mode (line buffering) and echo
	new_termios.c_lflag &= ~(ICANON | ECHO);

	// Apply new settings
	tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

	// Read a single character
	char ch;
	read(STDIN_FILENO, &ch, 1);

	// Restore original terminal settings
	tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);

	return ch;
}
