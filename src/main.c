#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "./lib/user_function.h"

/* 
* You need nerd font to render icons in terminal and as well as in your code editor.
* For more info visit this side "https://www.nerdfonts.com/"
*/

#define ICON_COPIED     ""
#define ICON_UPDATED    ""
#define ICON_TYPE_DIR   "󱧴"
#define ICON_NO_CHANGE  ""
#define ICON_NOT_FOUND  "󰩋"
#define ICON_OPEN_ERROR "󰀦"


#define TERMINAL_SIZE terminal_col_size()
#define STATUS_MARGIN 17


unsigned long findChecksum(const char *_fullPath);
void updateFile(const char *_source, const char *_target);
void checkPath(const char *_fileDir, const char *_fileName);
bool pathExist(char *_fullPath, unsigned long _string_len);
void printLog(const char *_fullPath, char _status);


typedef struct {
	char *fileName;
	char *systemDirectory;
	char *gitDir;
	char *newName;
} Filepath;


int main(void) {
	printf("|-%-*s|-%s", STATUS_MARGIN + 2, "Status", "Files");
	printNTime('-', TERMINAL_SIZE - 29, false);
	printf("|\n");

	Filepath pathData[] = {
		#include "./pathData.txt"
	};

	unsigned long len = (sizeof(pathData) / sizeof(Filepath));
	for (int i = 0; i < len; i++) {
		;
	}

	printNTime('-', TERMINAL_SIZE, true);
	return 0;
}


void checkPath(const char *_fileDir, const char *_fileName) {
	unsigned long len_dir = strlen(_fileDir);
	unsigned long len_file = strlen(_fileName);

	char fullPath[(len_dir + len_file)];
	sprintf(fullPath, "%s%s", _fileDir, _fileName);

	if (pathExist(fullPath, (len_dir + len_file - 1))) {
		;
	}

	return ;
}


void updateFile(const char *_source, const char *_target) {
	FILE *in;
	FILE *out;

	in = fopen(_source, "r");
	if (in == NULL) {
		unsigned long  len = strlen(_source);
		printLog(_source, 'O');
		return ;
	}

	in = fopen(_target, "w");
	if (out == NULL) {
		unsigned long  len = strlen(_source);
		printLog(_source, 'O');
		return ;
	}

	int ch;
	while ((ch = getc(in)) != EOF) {
		putc(ch, out);
	}
}


bool pathExist(char *_fullPath, unsigned long _string_len) {
	struct stat buffer;
	if (stat(_fullPath, &buffer) == 0) {
		if (S_ISREG(buffer.st_mode)) {
			return true;
		}

		else {
			printLog(_fullPath, 'O');
			return false;
		}
	}

	else {
		printLog(_fullPath, 'O');
		return false;
	}
}


void printLog(const char *_fullPath, char _status) {
	/* _status
	 * char 'C' `copied`     == "File copied to you git repo"
	 * char 'N' `no change`  == "Nothing is change in you local file"
	 * char 'D' `OPEN ERROR` == "Past path is a directory, not a file"
	 * char 'A' `alert`      == "File not found in you local directory"
	 * char 'O' `OPEN ERROR` == "File can not be open for reading or writing"
	 * char 'U' `updated`    == "File updated with you local file and git repo"
	*/

	unsigned short colorCode = 37;
	unsigned long _string_len = strlen(_fullPath);

	if (_status == 'C') {
		colorCode = 36;
		printf("\033[1;%hum%s  %-*s\033[0m", colorCode, ICON_COPIED, (STATUS_MARGIN), "COPIED");
	}

	else if (_status == 'O') {
		colorCode = 33;
		printf("\033[1;%hum%s  %-*s\033[0m", colorCode, ICON_OPEN_ERROR, (STATUS_MARGIN),"OPEN ERROR");
	}

	else if (_status == 'D') {
		colorCode = 91;
		printf("\033[1;%hum%s  %-*s\033[0m", colorCode, ICON_TYPE_DIR, (STATUS_MARGIN), "DIR ERRRO");
	}

	else if (_status == 'U') {
		colorCode = 32;
		printf("\033[1;%hum%s  %-*s\033[0m", colorCode, ICON_UPDATED, (STATUS_MARGIN), "UPDATED");
	}

	else if (_status == 'A') {
		colorCode = 101;
		printf("\033[1;%hum%s  %-*s\033[0m", 31, ICON_NOT_FOUND, (STATUS_MARGIN), "FILE NOT FOUND");
	}

	else {
		printf("\033[1;%hum%s  %-*s\033[0m", colorCode, ICON_NO_CHANGE, (STATUS_MARGIN), "UP TO DATE");
	}

	printf("\033[1;35m%s\033[0m", " | ");

	unsigned short address = 0;

	if (_string_len > (TERMINAL_SIZE - STATUS_MARGIN)) {
		address = STATUS_MARGIN + 19;
	}

	printf("\033[0;%hum%-*s\033[0m", colorCode, (int)((TERMINAL_SIZE - STATUS_MARGIN - 8)), &_fullPath[address]);
	printf("\033[1;35m%s\033[0m\n", " |");
}


unsigned long findChecksum(const char *_fullPath) {
	FILE *file;
	if (file == NULL) {
		printLog(_fullPath, 'O');
		return 0;
	}

	unsigned long sum = 0;
	int ch;

	while ((ch = getc(file) != EOF)) {
		sum += ch;
	}

	return sum;
}

