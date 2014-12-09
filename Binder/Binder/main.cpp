#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include "dirent.h"
#include "binder.h"
#include <vector>
#include <stdio.h>
#include <string>
#include <fstream>
#include <cstdlib>
using namespace std;

char *dirConcat(char*, char*);

int main(int argc, char *argv[]) {
	// Execute the new generated host file.
	if (argc == 1) {
		printf("executed");
	} // Execute binder to bind files.
	else {
		char *appName = nullptr;
		char *srcFolder = nullptr;
		char *dstFolder = nullptr;
		char *hostFileName = nullptr;
		char *srcHostFileName = nullptr;
		char *dstHostFileName = nullptr;
		char *currentFileName = nullptr;
		Binder binder;

		appName = new char[strlen(argv[0]) + 5];
		strcpy(appName, argv[0]);
		strcat(appName, ".exe");

		srcFolder = argv[1];
		dstFolder = argv[2];
		hostFileName = argv[3];

		srcHostFileName = dirConcat(srcFolder, hostFileName);
		dstHostFileName = dirConcat(dstFolder, hostFileName);

		// Setting host file name in source directory to Binder.
		if (!binder.setHostFileName(srcHostFileName)) {
			printf("Binder could not open source host file \"%s\".\n", srcHostFileName);
			binder.~Binder();
			return 0;
		}
		else {
			printf("Added host file \"%s\" to Binder.\n", srcHostFileName);
		}

		// Setting host file name in destination directory to Binder.
		if (!binder.setDestinationFileName(dstHostFileName)) {
			printf("Binder could not create host file \"%s\".\n", dstHostFileName);
			binder.~Binder();
			return 0;
		}
		else {
			printf("Created host file \"%s\".\n", dstHostFileName);
		}

		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir(srcFolder)) != NULL) {
			printf("Reading directory \"%s\"......\n", srcFolder);

			// print all the files and directories within directory
			// Also add all files in source directory to Binder.
			while ((ent = readdir(dir)) != NULL) {
				currentFileName = dirConcat(srcFolder, ent->d_name);
				// Filter out "." and ".."
				if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..") && strcmp(currentFileName, srcHostFileName)) {
					printf("Detected: %s\n", ent->d_name);
					if (!binder.addFile(currentFileName)) {
						printf("Binder could not open \"%s\".\n", currentFileName);
						binder.~Binder();
						return 0;
					}
					else {
						printf("Added \"%s\" to Binder.\n", currentFileName);
					}
				}
			}
			closedir(dir);
		}
		else {
			// could not open directory
			printf("Could not open directory \"%s\"\n", srcFolder);
			binder.~Binder();

			return 0;
		}
		
		if (binder.bind(appName)) {
			printf("Binding finished.");
		}
		else {
			printf("Binding failed.");
			binder.~Binder();
			return 0;
		}

		//binder.~Binder();
	}

	return 0;
}

char *dirConcat(char *srcFirst, char *srcSecond) {
	int lenFirst = strlen(srcFirst);
	int lenSecond = strlen(srcSecond);

	char *dst = new char[lenFirst + lenSecond + 2];
	strcpy(dst, srcFirst);
	strcat(dst, "\\");
	strcat(dst, srcSecond);

	return dst;
}