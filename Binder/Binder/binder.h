#ifndef BINDER_H
#define BINDER_H

#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>

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
	std::vector<char*> nameRecord;
	std::vector<unsigned long> positionRecord;
	std::vector<unsigned long> sizeRecord;

	// Private methods.
	bool checkExeFileNameFormat(char*);
	bool checkIfFileExist(char*);
	bool isSetHostFile();
	bool isSetDestinationFile();
	void clearFile(std::map<char*, FILE*>&);
	unsigned long writeFile(FILE*, FILE*);
	unsigned long writeBindRecord(FILE*);
	void setBindRecord(char*, unsigned long, unsigned long);
};

#endif