#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>


bool pathExist(const char *_fullPath);
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


bool pathExist(const char *_fullPath) {
	struct stat buffer;
	if (stat(_fullPath, &buffer) == 0) return true;
	return false;
}