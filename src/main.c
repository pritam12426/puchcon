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

static int terminal_size = 0;
static char *source_git_directory = NULL;


int checkCompEnv(void);
void makeDir(char *_child_dir);
bool pathExist(char *_fullPath);
void printLog(const char *_fullPath, char _status);
void updateFile(const char *_source, const char *_target);
bool fileCmp(const char *_fullPath, const char *_fullPath2);


typedef struct {
	char *fileName;
	char *systemDirectory;
	char *gitDir;
	char *newName;
} Filepath;


int main(void) {
	/* writing data into Global variable */
	terminal_size = terminal_col_size();
	source_git_directory = getenv("COMP_GIT_SOURCE_DIR");

	if (checkCompEnv() != 0) return 1;

	printf("\033[1;33m%s\033[0m: \033[1;31m%s\033[0m \033[1;37m%s\033[0m\n" , "Local Git Repo", "󰊢", source_git_directory);
	printf("|-\033[1;35m%-*s\033[0m|-\033[1;34m%s\033[0m ", STATUS_MARGIN + 2, "STATUS", "FILES");
	printNTime('-', terminal_size - 30, false);
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

		git_path_len += strlen(source_git_directory);
		git_path_len += strlen(pathData[i].gitDir);
		git_path_len += strlen(pathData[i].newName);

		char git_full_path[git_path_len + 1];
		snprintf(git_full_path, sizeof(git_full_path), "%s%s%s", source_git_directory, pathData[i].gitDir, pathData[i].newName);

		if (pathExist(git_full_path)) {
			if ( ! fileCmp(full_local_path, git_full_path)) {
				updateFile(full_local_path, git_full_path);
				printLog(full_local_path, 'U');
			}

			else printLog(full_local_path, 'N');
		}

		else {
			makeDir(pathData[i].gitDir);
			updateFile(full_local_path, git_full_path);
			printLog(full_local_path, 'C');
		}
	}

	putchar('|');
	printNTime('-', terminal_size - 2, false);
	printf("|\n");
	return 0;
}


void updateFile(const char *_source, const char *_target) {
	FILE *in;
	FILE *out;

	in = fopen(_source, "r");
	if (in == NULL) {
		printLog(_source, 'O');
		return ;
	}

	out = fopen(_target, "w");
	if (out == NULL) {
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
			colorCode = 32;
			printf("\033[1;%hum%s  %-*s\033[0m", colorCode, ICON_UPDATED, (STATUS_MARGIN), "UPDATED");
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

	unsigned short address = 0, margin = 8;

	if (_string_len > (terminal_size - 23)) {
		printf("\033[0;%dm%s\033[0m", colorCode, "...");
		address = (_string_len - (terminal_size - 23)) + 5;
		margin += 3;
	}

	printf("\033[0;%hum%-*s\033[0m", colorCode, (int)((terminal_size - STATUS_MARGIN - margin)), &_fullPath[address]);
	printf("\033[1;35m%s\033[0m\n", " |");
	fflush(stdout);
	// usleep(100000);
}


bool fileCmp(const char *_fullPath, const char *_fullPath2) {
	FILE *file_local;

	file_local = fopen(_fullPath, "r");


	if (file_local == NULL) {
		fclose(file_local);
		printLog(_fullPath, 'O');
		return false;
	}

	FILE *file_git;

	file_git = fopen(_fullPath2, "r");

	if (file_git == NULL) {
		fclose(file_git);
		printLog(_fullPath2, 'O');
		return false;
	}

	int ch, ch2;

	while ((ch = getc(file_local)) != EOF) {
		ch2 = getc(file_git);
		if (ch != ch2) return false;
	}

	fclose(file_git);
	fclose(file_local);

	return true;
}


void makeDir(char *_child_dir) {
	unsigned long _len = strlen(_child_dir) + 1;
	char child_dir[_len];

	strcpy(child_dir, _child_dir);

	char *pwd = getenv("PWD");

	chdir(source_git_directory);
	char *ch = strtok(child_dir, "/");

	while (ch != NULL) {
		mkdir(ch, 0755);
		chdir(ch);

		ch = strtok(NULL, "/");
	}

	chdir(pwd);
}


int checkCompEnv(void) {
	if (source_git_directory == NULL) {
		const char *env_not_fond = {
			#include "./error_mess/not_found_env.txt"
		};

		printf("%s", env_not_fond);
		return 1;
	}

	struct stat stat_buffer;

	int status = stat(source_git_directory, &stat_buffer);

	if (status == 0) {
		if (S_ISDIR(stat_buffer.st_mode)) {
			char *cwd = getenv("PWD");
			chdir(source_git_directory);

			if (system("git rev-parse 2>> /dev/null") != 0) {
				printf("\t \033[1;31m\033[0m  \033[1;33mENVIRONMENT VARIABLE\033[0m  \033[1;37mCOMP_GIT_SOURCE_DIR\033[0m `\033[1;101m%s\033[0m` is not a git repository!\n", source_git_directory);
				chdir(cwd);
				return 1;
			}

			chdir(cwd);
			return 0;
		}

		else {
			const char *env_not_dir = {
				#include "./error_mess/env_not_dir.txt"
			};

			printf("\033[1;33mCOMP_GIT_SOURCE_DIR\033[0m `ENVIRONMENT VARIABLE`");
			printf(" { \033[1;101m%s\033[0m } ", source_git_directory);
			printf("%s\n", env_not_dir);
			return 1;
		}
	}

	return 1;
}
