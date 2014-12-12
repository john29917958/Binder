/*
* binder.cpp - a class for Microsoft Visual Studio
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
* $Id: binder.h,v 1.0 2014/12/12 12:00 Taoyuan Taiwan $
*/
#define _CRT_SECURE_NO_WARNINGS

#include "binder.h"

/* Public methods.*/

/* @constructor */
Binder::Binder() {

}

/**
 * Set up the host file.
 *
 * @param {char*} fileName the name of host file.
 * @return {boolean} if host file is set successfully, return true,
 * else return false.
 */
bool Binder::setHostFile(char *fileName) {
	FILE *tempFile = nullptr;
	char *name = nullptr;

	if (checkExeFileNameFormat(fileName) && !checkIfFileUsed(fileName) && (tempFile = fopen(fileName, "rb"))) {
		name = new char[strlen(fileName) + 1];
		name[strlen(fileName)] = '\0';
		strcpy(name, fileName);

		// Close and clear hostFile if host file opened.
		clearFileInfo(hostFile);

		hostFile[name] = tempFile;

		return true;
	}
	else {
		return false;
	}
}

/**
 * Add files to be bound into the host file.
 *
 * @param {char*} fileName the name of the file to be added.
 * @return {bool} if the file is added successfully, return true,
 * else return false.
 */
bool Binder::addFile(char* fileName) {
	FILE *tempFile = nullptr;
	char *name = nullptr;

	if (isSetHostFile() && isSetDestinationFile() && !checkIfFileUsed(fileName) &&
		(tempFile = fopen(fileName, "rb"))) {
		name = new char[strlen(fileName) + 1];
		name[strlen(fileName)] = '\0';
		strcpy(name, fileName);
		fileList[name] = tempFile;

		return true;
	}
	else {
		return false;
	}
}

/**
 * Set up the file name which will be used by the newly created host file.
 *
 * @param {char*} fileName the name of destination file.
 * @return {boolean} return true if the file name of destination file is set,
 * else, return false.
 */
bool Binder::setDestinationFileName(char* fileName) {
	FILE *tempFile = nullptr;
	char *name = nullptr;

	if (checkExeFileNameFormat(fileName) && !checkIfFileUsed(fileName) && (tempFile = fopen(fileName, "wb+"))) {
		name = new char[strlen(fileName) + 1];
		name[strlen(fileName)] = '\0';
		strcpy(name, fileName);

		// Close and clear destinationFile if destination file opened, and remove created destination file.
		if (!destinationFile.empty()) {
			remove(destinationFile.begin()->first);
			clearFileInfo(destinationFile);
		}

		destinationFile[name] = tempFile;

		return true;
	}
	else {
		return false;
	}
}

/**
 * Bind all added files into destination host file.
 * This method will copy the file specified by the parameter "appName"
 * to the empty destination host file at first, thus, the entry point
 * of newly created host file will be set to the file "appName". So when
 * user double click or use terminal to execute the newly created host file,
 * the "appName" will be executed.
 *
 * @param {char*} appName the name of file to be set as the entry point of the
 * newly created host file.
 * @return {bool} if binding is succeeded, return true, else, return false.
 */
bool Binder::bind(char *appName) {
	char *currentFileName = nullptr;
	FILE *currentFile = nullptr;
	FILE *dstFile = destinationFile.begin()->second;
	unsigned long size = 0;
	unsigned long offset = 0;

	if (isSetHostFile() && isSetDestinationFile() && (currentFile = fopen(appName, "rb"))) {
		/*
		Copy compiled executable Binder file to the destination host file,
		setting	the entry point to myBinder's int main() function to perform
		an extract operation when user double clicks on the created	host file.
		*/
		currentFileName = appName;

		size = writeFile(currentFile, dstFile);
		offset += size;

		// Writing host file into new host file.
		//currentFileName = hostFile.begin()->first;
		currentFileName = "HostFile.exe";
		currentFile = hostFile.begin()->second;

		size = writeFile(currentFile, dstFile);
		addBindRecord(currentFileName, offset, size);
		offset += size;

		// Writing all files to be binded to the new host file.
		for (std::map<char*, FILE*>::iterator it = fileList.begin(); it != fileList.end(); ++it) {
			currentFileName = it->first;
			currentFile = it->second;

			size = writeFile(currentFile, dstFile);
			addBindRecord(currentFileName, offset, size);
			offset += size;
		}

		// Writing the binding record to the end of the created host file.
		offset = writeBindRecord(dstFile);
		std::string bindRecordPosition = std::to_string(offset);

		// Writing the position of the binding record to the end of the created host file.
		fwrite(bindRecordPosition.c_str(), bindRecordPosition.length(), 1, dstFile);
		
		return true;
	}
	else {
		return false;
	}
}

/**
 * Close all opened files.
 * @destructor
 */
Binder::~Binder() {
	clearFileInfo(hostFile);
	clearFileInfo(destinationFile);
	clearFileInfo(fileList);
}

/* Private methods */

/**
 * Checking if the target file is an .exe file or not.
 *
 * @param {char*} fileName the name of file to be checked.
 * @return {bool} return true if the target file is an .exe
 * file, return false otherwise.
 */
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

/**
 * Check if the given file name is used by Binder.
 * In another word, check if the given file name matches
 * the host file name, destination file name, file list
 * file name or not.
 *
 * @param {char*} fileName the file name to be checked.
 * @return {bool} return true if the given file name is
 * used, return flase if the given file name is not in
 * use by Binder.
 */
bool Binder::checkIfFileUsed(char *fileName) {
	if (hostFile.find(fileName) == hostFile.end() &&
		destinationFile.find(fileName) == destinationFile.end() &&
		fileList.find(fileName) == fileList.end()) {
		return false;
	}
	else {
		return true;
	}
}

/**
 * Check if the host file of this Binder is set or not.
 *
 * @return {bool} return true if host file set, return
 * false if host file has not been set.
 */
bool Binder::isSetHostFile() {
	return !hostFile.empty();
}

/**
 * Check if the destination host file name is set or not.
 *
 * @return {bool} return true if destination host file
 * is set, return false if destination host file has not
 * been set.
 */
bool Binder::isSetDestinationFile() {
	return !destinationFile.empty();
}

/**
 * Clear out the certain file information in Binder.
 *
 * @param {std::map<char*, FILE*>&} m the file information to be
 * clean out.
 */
void Binder::clearFileInfo(std::map<char*, FILE*> &m) {
	if (!m.empty()) {
		for (std::map<char*, FILE*>::iterator it = m.begin(); it != m.end(); ++it) {
			delete[] it->first;
			fclose(it->second);
			it->second = nullptr;
		}
		m.clear();
	}
}

/**
 * Write the source file into destination file.
 *
 * @param {FILE*} srcFile the source file.
 * @param {FILE*} dstFile the destination file.
 * @return {unsigned long} the size of bytes written into
 * destination file.
 */
unsigned long Binder::writeFile(FILE* srcFile, FILE* dstFile) {
	unsigned long size = 0;
	char *buffer = nullptr;

	// Positioning to the end of file.
	fseek(srcFile, 0, SEEK_END);
	size = ftell(srcFile);

	// Reset the position to the beginning of file.
	rewind(srcFile);
	buffer = new char[size];

	fread(buffer, size, 1, srcFile);
	fwrite(buffer, size, 1, dstFile);

	delete[] buffer;

	return size;
}

/**
 * Write the binding record to the end of the given file.
 *
 * @param {FILE*} targetFile the file to be written.
 * @return {unsigned long} the beginning postition of this
 * record in given file.
 */
unsigned long Binder::writeBindRecord(FILE* targetFile) {
	if (targetFile != nullptr) {
		unsigned long currentPosition = ftell(targetFile);
		int fileNameSpliter = 0;
		std::string currentFileName;
		std::string currentFilePosition;
		std::string currentFileSize;

		while (!nameRecord.empty()) {
			currentFileName = nameRecord.back();
			fileNameSpliter = currentFileName.find_last_of("\\", std::string::npos);
			if (fileNameSpliter != std::string::npos) {
				currentFileName = currentFileName.substr(fileNameSpliter + 1, std::string::npos);
			}			

			currentFilePosition = std::to_string(positionRecord.back());
			currentFileSize = std::to_string(sizeRecord.back());

			// The order of information to be written is "file name", "position", "size".
			fwrite(currentFileName.c_str(), currentFileName.length(), 1, targetFile);
			fwrite("\0", 1, 1, targetFile);
			fwrite(currentFilePosition.c_str(), currentFilePosition.length(), 1, targetFile);
			fwrite("\0", 1, 1, targetFile);
			fwrite(currentFileSize.c_str(), currentFileSize.length(), 1, targetFile);
			fwrite("\0", 1, 1, targetFile);

			nameRecord.pop_back();
			positionRecord.pop_back();
			sizeRecord.pop_back();
		}

		fwrite("!", 1, 1, targetFile);

		return currentPosition;
	}
	else {
		return 0;
	}
}

/**
 * Add a binding record of certain file to Binder.
 *
 * @param {char*} fileName the name of file which is written into the newly generated host file.
 * @param {unsigned long} position the beginning position of this file in host file.
 * @param {unsigned long} size the size of this file.
 */
void Binder::addBindRecord(char* fileName, unsigned long position, unsigned long size) {
	char *currentFileName = new char[strlen(fileName) + 1];
	currentFileName[strlen(fileName)] = '\0';
	strcpy(currentFileName, fileName);

	nameRecord.push_back(currentFileName);
	positionRecord.push_back(position);
	sizeRecord.push_back(size);
}