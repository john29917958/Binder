#define _CRT_SECURE_NO_WARNINGS

#include "stub.h"

Stub::Stub() {
	fileName = nullptr;
	file = nullptr;
}

bool Stub::setFileName(char *fName) {
	if (checkExeFileNameFormat(fName) && (file = fopen(fName, "rb"))) {
		fileName = new char[strlen(fName) + 1];
		strcpy(fileName, fName);

		return true;
	}
	else {
		delete file;
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
		unsigned long size = 0;

		fseek(file, 0, SEEK_END);
		size = ftell(file);
		rewind(file);

		fread(buffer, size, 0, file);

		return true;
	}
	else {
		return false;
	}
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

Stub::~Stub() {
	delete fileName;
	fclose(file);
}

bool Stub::searchSeperator() {

}