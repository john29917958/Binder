#define _CRT_SECURE_NO_WARNINGS

#include "binder.h"

// Public methods.

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

	if (checkExeFileNameFormat(fileName) && !checkIfFileExist(fileName) && (tempFile = fopen(fileName, "rb"))) {
		name = new char[strlen(fileName) + 1];
		name[strlen(fileName)] = '\0';
		strcpy(name, fileName);

		// Close and clear hostFile if host file opened.
		clearFile(hostFile);

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

	if (isSetHostFile() && isSetDestinationFile() && !checkIfFileExist(fileName) &&
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

	if (checkExeFileNameFormat(fileName) && !checkIfFileExist(fileName) && (tempFile = fopen(fileName, "wb+"))) {
		name = new char[strlen(fileName) + 1];
		name[strlen(fileName)] = '\0';
		strcpy(name, fileName);

		// Close and clear destinationFile if destination file opened, and remove created destination file.
		if (!destinationFile.empty()) {
			remove(destinationFile.begin()->first);
			clearFile(destinationFile);
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
		setBindRecord(currentFileName, offset, size);
		offset += size;

		// Writing all files to be binded to the new host file.
		for (std::map<char*, FILE*>::iterator it = fileList.begin(); it != fileList.end(); ++it) {
			currentFileName = it->first;
			currentFile = it->second;

			size = writeFile(currentFile, dstFile);
			setBindRecord(currentFileName, offset, size);
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

void Binder::clearFile(std::map<char*, FILE*> &m) {
	if (!m.empty()) {
		for (std::map<char*, FILE*>::iterator it = m.begin(); it != m.end(); ++it) {
			delete[] it->first;
			fclose(it->second);
			it->second = nullptr;
		}
		m.clear();
	}
}

unsigned long Binder::writeFile(FILE* srcFile, FILE* targetFile) {
	unsigned long size = 0;
	char *buffer = nullptr;

	// Positioning to the end of file.
	fseek(srcFile, 0, SEEK_END);
	size = ftell(srcFile);

	// Reset the position to the beginning of file.
	rewind(srcFile);
	buffer = (char*)malloc(size);
	fread(buffer, size, 1, srcFile);

	fwrite(buffer, size, 1, targetFile);

	return size;
}

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

void Binder::setBindRecord(char* fileName, unsigned long position, unsigned long size) {
	char *currentFileName = new char[strlen(fileName) + 1];
	currentFileName[strlen(fileName)] = '\0';
	strcpy(currentFileName, fileName);

	nameRecord.push_back(currentFileName);
	positionRecord.push_back(position);
	sizeRecord.push_back(size);
}