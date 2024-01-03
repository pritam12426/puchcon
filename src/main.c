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
#define STATUS_MARGIN 26


void updateFile(const char *_source, const char *_target);
void checkPath(const char *_fileDir, const char *_fileName);
bool pathExist(char *_fullPath, unsigned long _string_len);
void printLog(const char *_fullPath, char _status, unsigned long _string_len);


typedef struct {
	char *fileName;
	char *systemDirectory;
	char *gitDir;
	char *newName;
} Filepath;


int main(void) {
	printNTime('-', TERMINAL_SIZE, true);
	Filepath pathData[] = {
		#include "./pathData.txt"
	};

	unsigned long len = (sizeof(pathData) / sizeof(Filepath));
	int i = 0;
	// for (int i = 0; i < len; i++) {
		printLog(pathData[i].systemDirectory, 'C', 55);
		printLog(pathData[i].systemDirectory, 'A', 50);
		printLog(pathData[i].systemDirectory, 'U', 50);
		printLog(pathData[i].systemDirectory, 'N', 50);
		printLog(pathData[i].systemDirectory, 'D', 50);
		printLog(pathData[i].systemDirectory, 'O', 50);
	// }

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
		printLog(_source, 'O', len);
		return ;
	}

	in = fopen(_target, "w");
	if (out == NULL) {
		unsigned long  len = strlen(_source);
		printLog(_source, 'O', len);
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
			printLog(_fullPath, 'O', _string_len);
			return false;
		}
	}

	else {
		printLog(_fullPath, 'O', _string_len);
		return false;
	}
}

void printLog(const char *_fullPath, char _status, unsigned long _string_len) {
	/* _status
	 * char 'C' `copied`     == "File copied to you git repo"
	 * char 'N' `no change`  == "Nothing is change in you local file"
	 * char 'D' `OPEN ERROR` == "Past path is a directory, not a file"
	 * char 'A' `alert`      == "File not found in you local directory"
	 * char 'O' `OPEN ERROR` == "File can not be open for reading or writing"
	 * char 'U' `updated`    == "File updated with you local file and git repo"
	*/

	unsigned short colorCode = 37;

	if (_status == 'C') {
		colorCode = 36;
		printf("\033[1;%hum%s  %-*s\033[0m", colorCode, ICON_COPIED, (STATUS_MARGIN - 9), "COPIED");
	}

	else if (_status == 'O') {
		colorCode = 33;
		printf("\033[1;%hum%s  %-*s\033[0m", colorCode, ICON_OPEN_ERROR, (STATUS_MARGIN - 9),"OPEN ERROR");
	}

	else if (_status == 'D') {
		colorCode = 91;
		printf("\033[1;%hum%s  %-*s\033[0m", colorCode, ICON_TYPE_DIR, (STATUS_MARGIN - 9), "DIR ERRRO");
	}

	else if (_status == 'U') {
		colorCode = 32;
		printf("\033[1;%hum%s  %-*s\033[0m", colorCode, ICON_UPDATED, (STATUS_MARGIN - 9), "UPDATED");
	}

	else if (_status == 'A') {
		colorCode = 101;
		printf("\033[1;%hum%s  %-*s\033[0m", 31, ICON_NOT_FOUND, (STATUS_MARGIN - 9), "FILE NOT FOUND");
	}

	else {
		printf("\033[1;%hum%s  %-*s\033[0m", colorCode, ICON_NO_CHANGE, (STATUS_MARGIN - 9), "UP TO DATE");
	}

	printf("\033[1;35m%s\033[0m", " | ");
	printf("\033[0;%hum%-*s\033[0m", colorCode, (TERMINAL_SIZE - STATUS_MARGIN), _fullPath);
	printf("\033[1;35m%s\033[0m\n", " | ");
}
