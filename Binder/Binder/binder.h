/*
* binder.h - a class for Microsoft Visual Studio
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
#ifndef BINDER_H
#define BINDER_H

#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>

/**
 * Declaration of Binder class.
 * This class provide some useful API for binding different files into
 * a single executable host file.
 * This class only provides the service of binding files into a single one,
 * it doesn't provide the service of extracting and executing binded files.
 * @class Binder
 */
class Binder {
public:
	/* @constructor */
	Binder();

	/**
	* Set up the host file.
	*
	* @param {char*} fileName the name of host file.
	* @return {boolean} if host file is set successfully, return true,
	* else return false.
	*/
	bool setHostFile(char*);

	/**
	* Add files to be bound into the host file.
	*
	* @param {char*} fileName the name of the file to be added.
	* @return {bool} if the file is added successfully, return true,
	* else return false.
	*/
	bool addFile(char*);

	/**
	* Set up the file name which will be used by the newly created host file.
	*
	* @param {char*} fileName the name of destination file.
	* @return {boolean} return true if the file name of destination file is set,
	* else, return false.
	*/
	bool setDestinationFileName(char*);

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
	bool bind(char*);

	/**
	* Close all opened files.
	* @destructor
	*/
	~Binder();
private:
	/* Private attributes. */
	std::map<char*, FILE*> hostFile; /* information about host file */
	std::map<char*, FILE*> fileList; /* information about all files to be bound */
	std::map<char*, FILE*> destinationFile; /* information about destination host file */
	std::vector<char*> nameRecord; /* the record of name of all bounded files */
	std::vector<unsigned long> positionRecord; /* record of the begining position of each bounded file in newly generated host file */
	std::vector<unsigned long> sizeRecord; /* the record of file size of each bounded files */

	/* Private methods. */
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