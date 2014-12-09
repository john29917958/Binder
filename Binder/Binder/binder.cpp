#define _CRT_SECURE_NO_WARNINGS

#include "binder.h"

// Public methods.

Binder::Binder() {

}

bool Binder::setHostFileName(char *fileName) {
	FILE *tempFile = nullptr;

	if (checkExeFileNameFormat(fileName) && checkIfFileExist(fileName) && (tempFile = fopen(fileName, "rb"))) {
		clearFile(hostFile);
		hostFile[fileName] = tempFile;

		return true;
	}
	else {
		delete tempFile;

		return false;
	}
}

bool Binder::addFile(char* fileName) {
	FILE *tempFile;

	if (isSetHostFile() && isSetDestinationFile() && checkIfFileExist(fileName) &&
		(tempFile = fopen(fileName, "rb"))) {
		fileList[fileName] = tempFile;

		return true;
	}
	else {
		return false;
	}
}

bool Binder::setDestinationFileName(char* fileName) {
	FILE *tempFile = nullptr;

	if (checkExeFileNameFormat(fileName) && checkIfFileExist(fileName) && (tempFile = fopen(fileName, "ab+"))) {
		clearFile(destinationFile);
		destinationFile[fileName] = tempFile;

		return true;
	}
	else {
		delete tempFile;

		return false;
	}
}

char **Binder::bind(char *argv0) {
	char *hostFileData = nullptr;
	char *fileListData = nullptr;
	char *binderData = nullptr;
	// List of all binded files, the count of all binded file
	// is the file count in fileList and one host file and
	// this program.
	char **fileNameListInOrder = new char*[fileList.size() + 2];
	int fileNameListInOrderCounter = 0;
	FILE *dstFile = destinationFile.begin()->second;

	if (isSetHostFile() && isSetDestinationFile()) {
		hostFileData = readFileInBinary(hostFile.begin()->second);
		binderData = readFileInBinary(fopen(argv0, "rb"));

		/*
		Copy compiled myBinder to the destination host file, setting
		the entry point to myBinder's int main() function to perform
		an extract operation when user double clicks on the created
		host file.
		*/
		writeFile(binderData, dstFile, true);
		fileNameListInOrder[0] = argv0;
		fileNameListInOrderCounter += 1;

		// Writing host file into new host file.
		writeFile(hostFileData, dstFile, false);
		fileNameListInOrder[1] = hostFile.begin()->first;
		fileNameListInOrderCounter += 1;

		// Writing all files to be binded to the new host file.
		for (std::map<char*, FILE*>::iterator it = fileList.begin(); it != fileList.end(); ++it) {
			writeFile(readFileInBinary(it->second), dstFile, false);
			fileNameListInOrderCounter += 1;
		}

		return fileNameListInOrder;
	}
	else {
		return nullptr;
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
		fileName[length - 3] == '.' &&
		fileName[length - 2] == 'e' &&
		fileName[length - 1] == 'x' &&
		fileName[length] == 'e') {
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
		return true;
	}
	else {
		return false;
	}
}

bool Binder::isSetHostFile() {
	return !hostFile.empty();
}

bool Binder::isSetDestinationFile() {
	return !destinationFile.empty();
}

char* Binder::readFileInBinary(FILE* f) {
	unsigned long size = 0;
	char *buffer = nullptr;

	// Positioning to the end of file.
	fseek(f, 0, SEEK_END);
	size = ftell(f);

	// Reset the position to the beginning of file.
	rewind(f);
	buffer = (char*)malloc(size);
	fread(buffer, size, 1, f);

	return buffer;
}

void Binder::clearFile(std::map<char*, FILE*> &m) {
	if (!m.empty()) {
		for (std::map<char*, FILE*>::iterator it = m.begin(); it != m.end(); ++it) {
			fclose(it->second);
			remove(it->first);
		}
		m.clear();
	}
}

void Binder::writeFile(char* buffer, FILE* f, bool isEntryPoint) {
	char *seperator = "*****";

	if (!isEntryPoint) {
		fwrite(seperator, strlen(seperator), 1, f);
	}

	fwrite(buffer, strlen(buffer), 1, f);
}