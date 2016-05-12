/*
Name: Phong Tran
Class: CSCE 463-500
Acknowledgement: HTMLParserTest
*/

#pragma once
#include "stdafx.h"

//exception for file error
struct FileReaderException{
	int errorCode;

	FileReaderException(int code) :errorCode(code){}
};

//take care of opening and reading file line by line
class FileReader
{
	char* fileBuf = NULL;
	int fileSize;
	char* head = NULL;
	char* tail = NULL;
public:
	FileReader(_TCHAR* fileName);
	~FileReader();

	int getFileSize() const { return fileSize; }

	bool hasNext();
	char* getNext();
};

