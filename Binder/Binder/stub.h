#ifndef STUB_H
#define STUB_H

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

class Stub {
public:
	Stub();
	bool setFileName(char*);
	bool isSetFileName();
	bool extractAndExecute();
	~Stub();
private:
	// Private attributes
	char *fileName;
	FILE *file;
	std::vector<char*> nameRecord;
	std::vector<unsigned long> positionRecord;
	std::vector<unsigned long> sizeRecord;

	// Private methods
	bool checkExeFileNameFormat(char*);
	bool checkSeperator(char*);
	unsigned long readRecordPosition(char*, unsigned long);
	void readRecord(char*, unsigned long);
	char *readSingleRecordData(char*, unsigned long);
	bool createFile(char*, char*, unsigned long);
};

#endif