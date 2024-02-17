#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>
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
static char *SOURCE_GIT_DIRECTORY = NULL;


int  pushRepo(void);
int  gitCommit(void);
int  checkCompEnv(void);
bool unChangeFiles(void);
void makeDir(char *_child_dir);
char getCharWithoutEnter(void);
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
	SOURCE_GIT_DIRECTORY = getenv("COMP_GIT_SOURCE_DIR");

	if (checkCompEnv() != 0) return 1;

	printf("\033[1;33m%s\033[0m: \033[1;31m%s\033[0m \033[1;37m%s\033[0m\n" , "Local Git Repo", "󰊢", SOURCE_GIT_DIRECTORY);
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

		git_path_len += strlen(SOURCE_GIT_DIRECTORY);
		git_path_len += strlen(pathData[i].gitDir);
		git_path_len += strlen(pathData[i].newName);

		char git_full_path[git_path_len + 1];
		snprintf(git_full_path, sizeof(git_full_path), "%s%s%s", SOURCE_GIT_DIRECTORY, pathData[i].gitDir, pathData[i].newName);

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

	chdir(SOURCE_GIT_DIRECTORY);

	unChangeFiles();

	if (gitCommit() != 0) {
		printf("Error while commit the code\n");
		return 1;
	}

	if (pushRepo() != 0) {
		return 1;
	}

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
	 * char 'C' `COPIED`     => "File copied to you git repo"
	 * char 'N' `NO CHANGE`  => "Nothing is change in you local file"
	 * char 'D' `OPEN ERROR` => "Past path is a directory, not a file"
	 * char 'A' `ALERT`      => "File not found in you local directory"
	 * char 'O' `OPEN ERROR` => "File can not be open for reading or writing"
	 * char 'U' `UPDATED`    => "File updated with you local file and git repo"
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
	usleep(10000);
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

	chdir(SOURCE_GIT_DIRECTORY);
	char *ch = strtok(child_dir, "/");

	while (ch != NULL) {
		mkdir(ch, 0755);
		chdir(ch);

		ch = strtok(NULL, "/");
	}

	chdir(pwd);
}


int checkCompEnv(void) {
	if (SOURCE_GIT_DIRECTORY == NULL) {
		const char *env_not_fond = {
			#include "./error_mess/not_found_env.txt"
		};

		printf("%s", env_not_fond);
		return 1;
	}

	struct stat stat_buffer;

	int status = stat(SOURCE_GIT_DIRECTORY, &stat_buffer);

	if (status == 0) {
		if (S_ISDIR(stat_buffer.st_mode)) {
			char *cwd = getenv("PWD");
			chdir(SOURCE_GIT_DIRECTORY);

			if (system("git rev-parse 2>> /dev/null") != 0) {
				printf("\t \033[1;31m\033[0m  \033[1;33mENVIRONMENT VARIABLE\033[0m  \033[1;37mCOMP_GIT_SOURCE_DIR\033[0m `\033[1;101m%s\033[0m` is not a git repository!\n", SOURCE_GIT_DIRECTORY);
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
			printf(" { \033[1;101m%s\033[0m } ", SOURCE_GIT_DIRECTORY);
			printf("%s\n", env_not_dir);
			return 1;
		}
	}

	return 1;
}


bool unChangeFiles(void) {
	printf("\nFOUNDED CHANGES IN\n");
	FILE *fp;
	int ch = 0;

	fp = popen("git status --porcelain", "r");

	unsigned line = 1;
	printf("%3d\033[1;35m: \033[0m", line++);
	bool commit = false;

	for (int i = 0; (ch = getc(fp)) != -1; i++) {
		if (i < 3) {
			continue;
		}

		if (ch == '\n') {
			putchar('\n');
			printf("%3d\033[1;35m: \033[0m", line++);
			i = -1;
			continue;
		}

		putchar(ch);

		commit = true;
	};

	pclose(fp);
	printf("\b\b\b\b\b");
	return commit;
}


int pushRepo() {
	FILE *fp = popen("git status | grep 'Your'", "r");
	if (fp == NULL) {
		perror("Error opening pipe");
		return 1;
	}

	char buffer[31];
	int isPushed = false;

	if ((fgets(buffer, sizeof(buffer), fp) != NULL) && (strncasecmp(buffer, "Your branch is up to date with", 30)) == 0) isPushed = true;

	pclose(fp);

	if ( ! isPushed) {
		printf("Repo is not push :: Hit [y/n] to push: ");

		if (getCharWithoutEnter() == 'y') {
			printf("\nTrying to push...\n");
			if (system("git push origin main -f > /dev/null 2>&1") == 0) {
				printf("Pushed Successful.\n");
			}
			else {
				printf("Some thing wrong happened while pushing\n");
				return 1;
			}
		}
		else {
			putchar('\n');
		}
	}

	return 0;
}


char getCharWithoutEnter(void) {
	char ch;
	struct termios oldt, newt;

	tcgetattr(STDIN_FILENO, &oldt);

	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}


int gitCommit(void) {
	FILE *fp = popen("git status --short", "r");

	if (fp == NULL) {
		// perror("Error opening pipe");
		return 1;
	}

	int ch;

	if ((ch = getc(fp)) == EOF) {
		pclose(fp);
		return 1;
	}
	else {
		pclose(fp);
	}

	printf("Uncommit files found :: Hit [y/n] to push: ");

	if (getCharWithoutEnter() != 'y') {
		putchar('\n');
		return 1;
	}
	else {
		putchar('\n');
	}

	char commit_command[(15 + 100 + 14 + 5)];
	system("git add --all > /dev/null 2>&1");

	char commit[100];

	printf("Enter the commit for the change >> ");
	scanf("%[^\n]%*c", commit);

	strcat(commit_command, commit);
	sprintf(commit_command, "git commit -m '%s' %s", commit, " > /dev/null 2>&1");
	printf("%s\n", commit_command);

	if (system(commit_command) == 0) {
		return 0;
	}
	else {
		return 1;
	}
}
