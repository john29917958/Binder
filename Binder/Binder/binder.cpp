#define _CRT_SECURE_NO_WARNINGS

#include "binder.h"

// Public methods.

Binder::Binder() {
	
}

bool Binder::setHostFileName(char *fileName) {
	FILE *tempFile = nullptr;
	char *name = nullptr;

	if (checkExeFileNameFormat(fileName) && !checkIfFileExist(fileName) && (tempFile = fopen(fileName, "rb"))) {
		name = new char[strlen(fileName) + 1];
		strcpy(name, fileName);
		hostFile[name] = tempFile;

		return true;
	}
	else {
		delete tempFile;
		delete name;
		return false;
	}
}

bool Binder::addFile(char* fileName) {
	FILE *tempFile = nullptr;
	char *name = nullptr;

	if (isSetHostFile() && isSetDestinationFile() && !checkIfFileExist(fileName) &&
		(tempFile = fopen(fileName, "rb"))) {
		name = new char[strlen(fileName) + 1];
		strcpy(name, fileName);
		fileList[name] = tempFile;

		return true;
	}
	else {
		delete tempFile;
		delete name;
		return false;
	}
}

bool Binder::setDestinationFileName(char* fileName) {
	FILE *tempFile = nullptr;
	char *name = nullptr;

	if (checkExeFileNameFormat(fileName) && !checkIfFileExist(fileName) && (tempFile = fopen(fileName, "wb+"))) {
		name = new char[strlen(fileName) + 1];
		strcpy(name, fileName);
		destinationFile[name] = tempFile;

		return true;
	}
	else {
		delete tempFile;
		delete name;
		return false;
	}
}

bool Binder::bind(char *appName) {
	char *currentFileName = nullptr;
	FILE *hFile = nullptr;
	FILE *binderFile = nullptr;
	// List of all binded files, the count of all binded file
	// is the file count in fileList and one host file and
	// this program.
	FILE *dstFile = destinationFile.begin()->second;
	FILE *currentFile = nullptr;

	if (isSetHostFile() && isSetDestinationFile()) {
		hFile = hostFile.begin()->second;
		binderFile = fopen(appName, "rb");

		/*
		Copy compiled myBinder to the destination host file, setting
		the entry point to myBinder's int main() function to perform
		an extract operation when user double clicks on the created
		host file.
		*/
		writeFile(appName, binderFile, dstFile);
		
		// Writing host file into new host file.
		writeFile(hostFile.begin()->first, hFile, dstFile);

		// Writing all files to be binded to the new host file.
		for (std::map<char*, FILE*>::iterator it = fileList.begin(); it != fileList.end(); ++it) {
			currentFileName = it->first;
			currentFile = it->second;

			writeFile(currentFileName, currentFile, dstFile);
		}
		
		return true;
	}
	else {
		return false;
	}
}

Binder::~Binder() {
	clearFile(hostFile);
	clearFile(destinationFile);
	clearFile(fileList);
}

// Private methods

bool Binder::checkExeFileNameFormat(char *fileName) {
	int length = std::strlen(fileName);

	if (length > 4 &&
		fileName[length - 4] == '.' &&
		fileName[length - 3] == 'e' &&
		fileName[length - 2] == 'x' &&
		fileName[length - 1] == 'e') {
		return true;
	}
	else {
		return false;
	}
}

bool Binder::checkIfFileExist(char *fileName) {
	if (hostFile.find(fileName) == hostFile.end() &&
		destinationFile.find(fileName) == destinationFile.end() &&
		fileList.find(fileName) == fileList.end()) {
		return false;
	}
	else {
		return true;
	}
}

bool Binder::isSetHostFile() {
	return !hostFile.empty();
}

bool Binder::isSetDestinationFile() {
	return !destinationFile.empty();
}
/*
char* Binder::readFileInBinary(FILE* f) {
	printf("======================Reading file======================\n");
	unsigned long size = 0;
	char *buffer = nullptr;

	// Positioning to the end of file.
	fseek(f, 0, SEEK_END);
	size = ftell(f);

	// Reset the position to the beginning of file.
	rewind(f);
	buffer = (char*)malloc(size);
	fread(buffer, size, 1, f);

	std::cout << "Size: " << size << std::endl;
	std::cout << "Buffer: " << std::endl;
	for (int i = 0; i < size; i++) {
		std::cout << buffer[i];
	}

	printf("========================================================\n");
	return buffer;
}
*/
void Binder::clearFile(std::map<char*, FILE*> &m) {
	if (!m.empty()) {
		for (std::map<char*, FILE*>::iterator it = m.begin(); it != m.end(); ++it) {
			fclose(it->second);
		}
		m.clear();
	}
}

void Binder::writeFile(char *fileName, FILE* srcFile, FILE* targetFile) {
	unsigned long size = 0;
	char *buffer = nullptr;
	char *seperator = "*****";

	// Positioning to the end of file.
	fseek(srcFile, 0, SEEK_END);
	size = ftell(srcFile);

	// Reset the position to the beginning of file.
	rewind(srcFile);
	buffer = (char*)malloc(size);
	fread(buffer, size, 1, srcFile);

	fwrite(buffer, size, 1, targetFile);
	fwrite(seperator, strlen(seperator), 1, targetFile);
	fwrite(fileName, strlen(fileName), 1, targetFile);
	fwrite(seperator, strlen(seperator), 1, targetFile);
}