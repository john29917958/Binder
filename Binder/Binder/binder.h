#ifndef BINDER_H
#define BINDER_H

#include <stdio.h>
#include <iostream>
#include <map>
#include <Windows.h>

class Binder {
public:
	Binder();
	bool setHostFileName(char*);
	bool addFile(char*);
	bool setDestinationFileName(char*);
	bool bind(char*);
	~Binder();
private:
	// Private attributes.
	std::map<char*, FILE*> hostFile;
	std::map<char*, FILE*> fileList;
	std::map<char*, FILE*> destinationFile;

	// Private methods.
	bool checkExeFileNameFormat(char*);
	bool checkIfFileExist(char*);
	bool isSetHostFile();
	bool isSetDestinationFile();
	void clearFile(std::map<char*, FILE*>&);
	void writeFile(char*, FILE*, FILE*);
};

#endif