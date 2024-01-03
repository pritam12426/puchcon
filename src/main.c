#include <stdio.h>
#include <stdlib.h>
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


#define STATUS_MARGIN 17
#define TERMINAL_SIZE terminal_col_size()


#define SOURCE_GIT_DIRECTORY getenv("COMP_SOURCE_DIR")


void makeDir(char *_child_dir);
bool pathExist(char *_fullPath);
unsigned long findChecksum(const char *_fullPath);
void printLog(const char *_fullPath, char _status);
void updateFile(const char *_source, const char *_target);
void checkPath(const char *_fileDir, const char *_fileName);
int checkCompEnv(void);


typedef struct {
	char *fileName;
	char *systemDirectory;
	char *gitDir;
	char *newName;
} Filepath;


int main(void) {
	printf("|-\033[1;35m%-*s\033[0m|-\033[1;34m%s\033[0m ", STATUS_MARGIN + 2, "STATUS", "FILES");
	printNTime('-', TERMINAL_SIZE - 30, false);
	printf("|\n");

	Filepath pathData[] = {
		#include "./pathData.txt"
	};

	unsigned long len = (sizeof(pathData) / sizeof(Filepath));

	unsigned long local_path_len = 0;
	unsigned long git_path_len = 0;

	for (int i = 0; i < len; i++) {
		local_path_len += strlen(pathData[i].systemDirectory);
		local_path_len += strlen(pathData[i].fileName);

		char full_local_path[local_path_len + 1];
		snprintf(full_local_path, sizeof(full_local_path), "%s%s", pathData[i].systemDirectory, pathData[i].fileName);

		if ( ! pathExist(full_local_path)) {
			printLog(full_local_path, 'A');
			continue;
		}
		
		git_path_len += strlen(pathData[i].gitDir);
		git_path_len += strlen(pathData[i].newName);

		char git_full_path[git_path_len + 1];
		snprintf(git_full_path, sizeof(git_full_path), "%s%s", pathData[i].gitDir, pathData[i].newName);

		if (pathExist(git_full_path)) {
			if (findChecksum(git_full_path) != findChecksum(full_local_path)) {
				updateFile(full_local_path, git_full_path);
				printLog(git_full_path, 'U');
			}

			else printLog(git_full_path, 'N');
		}

		else {
			//  TODO: add a function making directory in side git dir;
		}
	}

	putchar('|');
	printNTime('-', TERMINAL_SIZE - 2, false);
	printf("|\n");
	return 0;
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

	out = fopen(_target, "w");
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


bool pathExist(char *_fullPath) {
	struct stat buffer;
	if (stat(_fullPath, &buffer) == 0) {
		if (S_ISREG(buffer.st_mode)) {
			return true;
		}

		else {
			printLog(_fullPath, 'D');
			return false;
		}
	}

	else {
		return false;
	}

	return false;
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

	switch (_status) {
		case 'C':
			colorCode = 36;
			printf("\033[1;%hum%s  %-*s\033[0m", colorCode, ICON_COPIED, (STATUS_MARGIN), "COPIED");
			break;
		case 'O':
			colorCode = 33;
			printf("\033[1;%hum%s  %-*s\033[0m", colorCode, ICON_OPEN_ERROR, (STATUS_MARGIN),"OPEN ERROR");
			break;
		case 'D':
			colorCode = 91;
			printf("\033[1;%hum%s  %-*s\033[0m", colorCode, ICON_TYPE_DIR, (STATUS_MARGIN), "DIR ERRRO");
			break;
		case 'U':
			colorCode = 101;
			printf("\033[1;%hum%s  %-*s\033[0m", 31, ICON_NOT_FOUND, (STATUS_MARGIN), "FILE NOT FOUND");
			break;
		case 'A':
			colorCode = 101;
			printf("\033[1;%hum%s  %-*s\033[0m", 31, ICON_NOT_FOUND, (STATUS_MARGIN), "FILE NOT FOUND");
			break;
		default:
			printf("\033[1;%hum%s  %-*s\033[0m", colorCode, ICON_NO_CHANGE, (STATUS_MARGIN), "UP TO DATE");
			break;
	}

	printf("\033[1;35m%s\033[0m", " | ");

	unsigned short address = 0;

	if (_string_len > (TERMINAL_SIZE - STATUS_MARGIN)) {
		address = STATUS_MARGIN + 19;
	}

	printf("\033[0;%hum%-*s\033[0m", colorCode, (int)((TERMINAL_SIZE - STATUS_MARGIN - 8)), &_fullPath[address]);
	printf("\033[1;35m%s\033[0m\n", " |");
	fflush(stdout);
}


unsigned long findChecksum(const char *_fullPath) {
	FILE *file;

	file = fopen(_fullPath, "rb");

	if (file == NULL) {
		fclose(file);
		printLog(_fullPath, 'O');
		return 0;
	}

	unsigned long sum = 0;
	int ch;

	while ((ch = getc(file) != EOF)) {
		sum += ch;
	}

	fclose(file);

	return sum;
}

void makeDir(char *_child_dir) {
	chdir(SOURCE_GIT_DIRECTORY);
	char *ch = strtok(_child_dir, "/");
	while ((ch = strtok(NULL, "/")) != NULL) {
		mkdir(ch, 0755);
		chdir(ch);
	}
}


int checkCompEnv(void) {
	if (SOURCE_GIT_DIRECTORY == NULL) {
		// TODO: add error message.
	}

	struct stat stat_buffer;

	int status = stat(SOURCE_GIT_DIRECTORY, &stat_buffer);

	if (status == 0) {
		if (S_ISDIR(stat_buffer.st_mode)) {
			return 0;
		}
	}

	return 1;
}
