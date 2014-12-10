#define _CRT_SECURE_NO_WARNINGS

#include "stub.h"

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
	if (fileName == nullptr && file == nullptr) {
		return false;
	}
	else {
		return true;
	}
}

bool Stub::extractAndExecute() {
	if (isSetFileName()) {
		char *buffer = nullptr;
		char *bytePointer = nullptr;
		std::vector<char*> fileNameList;

		unsigned long size = 0;

		fseek(file, 0, SEEK_END);
		size = ftell(file);
		rewind(file);

		buffer = new char[size + 1];
		buffer[size] = '\0';
		fread(buffer, size, 0, file);
		bytePointer = buffer;

		bytePointer = searchSeperator(buffer);

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

char *Stub::searchSeperator(char *bufferPosition) {
	if (bufferPosition != nullptr) {
		for (unsigned long i = 0; bufferPosition[i] != '\0'; i++) {
			if (checkSeperator(bufferPosition + i)) {
				return bufferPosition + i + 7;
			}
		}

		return nullptr;
	}
	else {
		return nullptr;
	}
}