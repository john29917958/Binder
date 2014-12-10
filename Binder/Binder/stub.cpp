#define _CRT_SECURE_NO_WARNINGS

#include "stub.h"
#include <Windows.h>

Stub::Stub() {
	fileName = nullptr;
	file = nullptr;
}

bool Stub::setFileName(char *fName) {
	if (checkExeFileNameFormat(fName) && (file = fopen(fName, "rb"))) {
		fileName = new char[strlen(fName) + 1];
		fileName[strlen(fName)] = '\0';
		strcpy(fileName, fName);

		return true;
	}
	else {
		fclose(file);
		file = nullptr;

		return false;
	}
}

bool Stub::isSetFileName() {
	if (fileName == nullptr) {
		return false;
	}
	else {
		return true;
	}
}

bool Stub::extractAndExecute() {
	if (isSetFileName()) {
		char *buffer = nullptr;
		std::vector<char*> fileNameList;
		unsigned long recordPosition = 0;
		unsigned long size = 0;
		char *currentFileName = nullptr;
		unsigned long currentFilePosition = 0;
		unsigned long currentFileSize = 0;
		FILE *currentFile = nullptr;

		// Get host file size in bytes.
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		rewind(file);

		// Load host file data into memory in bytes.
		buffer = new char[size + 1];
		buffer[size] = '\0';
		fread(buffer, size, 1, file);

		// Get the position of record which stores the information of binded files.
		recordPosition = readRecordPosition(buffer, size);
		// Now read all records.
		readRecord(buffer, recordPosition);
		
		// Create all binded files and execute it.
		while (!nameRecord.empty()) {
			currentFileName = nameRecord.back();
			currentFilePosition = positionRecord.back();
			currentFileSize = sizeRecord.back();
			
			if (createFile(currentFileName, buffer + currentFilePosition, currentFileSize)) {
				ShellExecute(NULL, "open", currentFileName, NULL, NULL, 1);
			}

			nameRecord.pop_back();
			positionRecord.pop_back();
			sizeRecord.pop_back();
		}	

		return true;
	}
	else {
		return false;
	}
}

Stub::~Stub() {
	fclose(file);
	delete[] fileName;
	fileName = nullptr;
	file = nullptr;
}

bool Stub::checkExeFileNameFormat(char *fileName) {
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

bool Stub::checkSeperator(char* ptr) {
	int seperatorSize = 5;

	for (int i = 0; i < seperatorSize; i++) {
		if (ptr[i] != '*') {
			return false;
		}
	}

	return true;
}

unsigned long Stub::readRecordPosition(char *buffer, unsigned long size) {
	unsigned long position = size - 1;
	unsigned long result = 0;

	while (buffer[position] != '!') {
		position -= 1;
	}

	position += 1;

	result = atol(buffer + position);

	return result;
}

void Stub::readRecord(char *buffer, unsigned long recordPosition) {
	unsigned long position = recordPosition;
	char *currentData = nullptr;
	
	while (buffer[position] != '!') {
		// Reading file name.
		currentData = readSingleRecordData(buffer, position);
		nameRecord.push_back(currentData);
		position += strlen(currentData) + 1;

		// Reading position of data of current file in bytes.
		currentData = readSingleRecordData(buffer, position);
		positionRecord.push_back(atol(currentData));
		position += strlen(currentData) + 1;

		// Reading the size of current file in bytes.
		currentData = readSingleRecordData(buffer, position);
		sizeRecord.push_back(atol(currentData));
		position += strlen(currentData) + 1;
	}
}

char *Stub::readSingleRecordData(char* buffer, unsigned long position) {
	char *ptr = buffer + position;
	char *result = nullptr;
	int length = strlen(ptr);

	result = new char[length + 1];
	result[length] = '\0';
	strcpy(result, ptr);

	return result;
}

bool Stub::createFile(char* fileName, char* data, unsigned long size) {
	FILE *f = nullptr;
	
	if (f = fopen(fileName, "wb+")) {
		fwrite(data, size, 1, f);
		fclose(f);
		
		return true;
	}
	else {
		return false;
	}
}