#ifndef STUB_H
#define STUB_H

#include <stdio.h>
#include <iostream>
#include <vector>

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

	// Private methods
	bool checkExeFileNameFormat(char*);
	bool checkSeperator(char*);
	char *searchSeperator(char*);
};

#endif