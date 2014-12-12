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

	/**
	* Checking if the target file is an .exe file or not.
	*
	* @param {char*} fileName the name of file to be checked.
	* @return {bool} return true if the target file is an .exe
	* file, return false otherwise.
	*/
	bool checkExeFileNameFormat(char*);

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
	bool checkIfFileUsed(char*);

	/**
	* Check if the host file of this Binder is set.
	*
	* @return {bool} return true if host file set,
	* return false if host file has not been set.
	*/
	bool isSetHostFile();

	/**
	* Check if the destination host file name is set or not.
	*
	* @return {bool} return true if destination host file
	* is set, return false if destination host file has not
	* been set.
	*/
	bool isSetDestinationFile();

	/**
	* Clear out the certain file information in Binder.
	*
	* @param {std::map<char*, FILE*>&} m the file information to be
	* clean out.
	*/
	void clearFileInfo(std::map<char*, FILE*>&);

	/**
	* Write the source file into destination file.
	*
	* @param {FILE*} srcFile the source file.
	* @param {FILE*} dstFile the destination file.
	* @return {unsigned long} the size of bytes written into
	* destination file.
	*/
	unsigned long writeFile(FILE*, FILE*);

	/**
	* Write the binding record to the end of the given file.
	*
	* @param {FILE*} targetFile the file to be written.
	* @return {unsigned long} the beginning postition of this
	* record in given file.
	*/
	unsigned long writeBindRecord(FILE*);

	/**
	* Add a binding record of certain file to Binder.
	*
	* @param {char*} fileName the name of file which is written into the newly generated host file.
	* @param {unsigned long} position the beginning position of this file in host file.
	* @param {unsigned long} size the size of this file.
	*/
	void addBindRecord(char*, unsigned long, unsigned long);
};

#endif