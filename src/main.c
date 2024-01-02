#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "./lib/user_function.c"

/* 
* You need nerd font to render icons in terminal and as well as in your code editor.
* For more info visit this side "https://www.nerdfonts.com/"
*/

#define ICON_NO_CHANGE ""
#define ICON_UPDATED   ""
#define ICON_ALERT     "󰩋"
#define ICON_ALERT     ""
#define ICON_COPIED    ""


bool pathExist(const char *_fullPath);
void printLog(const char *_fullPath, char _status);
void updateFile(const char *_source, const char *_target);
void checkPath(const char *_fileDir, const char *_fileName);


typedef struct {
	char *fileName;
	char *systemDirectory;
	char *gitDir;
	char *newName;
} Filepath;


int main(void) {
	Filepath pathData[] = {
		#include "./pathData.txt"
	};

	unsigned long len = (sizeof(pathData) / sizeof(pathData[0]));

	for (int i = 0; i < len; i++) {
		;
	}

	return 0;
}


void checkPath(const char *_fileDir, const char *_fileName) {
	unsigned long len_dir = strlen(_fileDir);
	unsigned long len_file = strlen(_fileName);

	char fullPath[(len_dir + len_file)];
	sprintf(fullPath, "%s%s", _fileDir, _fileName);

	if (pathExist(fullPath)) {
		;
	}

	return ;
}

void updateFile(const char *_source, const char *_target) {
	FILE *in;
	FILE *out;

	in = fopen(_source, "r");
	if (in == NULL) {
		/* TODO: write a fuction for printing log */
		return ;
	}

	in = fopen(_target, "w");
	if (out == NULL) {
		/* TODO: write a fuction for printing log */
		return ;
	}

	int ch;
	while ((ch = getc(in)) != EOF) {
		putc(ch, out);
	}

	/* TODO: write a fuction for printing log */
}


bool pathExist(const char *_fullPath) {
	struct stat buffer;
	if (stat(_fullPath, &buffer) == 0) return true;
	return false;
}

void printLog(const char *_fullPath, char _status) {
	;
}
