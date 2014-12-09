#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include "dirent.h"
#include <vector>
#include <stdio.h>
#include <string>
#include <fstream>
#include <cstdlib>
using namespace std;

char **getFileListInDir(char*);

int main(int argc, char *argv[]) {
	char *appName = nullptr;
	char *srcFolder = nullptr;
	char *dstFolder = nullptr;
	char *hostFileName = nullptr;

	// Execute the new generated host file.
	if (argc == 1) {
		char **test = new char*[4];
		test[0] = "First";
		test[1] = "Second";
		test[2] = "Third";
		test[3] = "0";

		int count = 0;

		while (test[count] != "0")
		{
			printf("%s\n", test[count]);
			count += 1;
		}

		cin.get();
	} // Execute binder to bind files.
	else {
		appName = argv[0];
		srcFolder = argv[1];
		dstFolder = argv[2];
		hostFileName = argv[1];

		char **fileNames = getFileListInDir(srcFolder);
		int count = 0;
		if (fileNames) {
			while (fileNames[count] != nullptr) {
				printf("%s\n", fileNames[count]);
				count += 1;
				cin.get();
			}
		}
		else {
			printf("Directory cannot be opened.");
		}
		cin.get();
	}

	return 0;
}

char **getFileListInDir(char *dirName) {
	DIR *dir;
	struct dirent *ent;
	int fileCount = 0;
	vector<char*> fileNameListVector;
	char **fileNameList = nullptr;

	if ((dir = opendir(dirName)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			fileNameListVector.push_back(ent->d_name);
			//printf("%s\n", ent->d_name);
		}
		closedir(dir);

		fileCount = fileNameListVector.size();
		fileNameList = new char*[fileCount + 1];
		for (int i = 0; i < fileCount; i++) {
			printf("Peak: %s", fileNameListVector.back());
			fileNameList[i] = fileNameListVector.back();
			fileNameListVector.pop_back();
		}
		fileNameList[fileCount + 1] = nullptr;

		fileNameListVector.clear();
		return fileNameList;
	}
	else {
		/* could not open directory */
		return nullptr;
	}
}