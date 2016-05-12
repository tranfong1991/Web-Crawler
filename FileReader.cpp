/*
Name: Phong Tran
Class: CSCE 463-500
*/

#include "FileReader.h"

FileReader::FileReader(_TCHAR* fileName)
{
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		throw FileReaderException(GetLastError());
	}

	LARGE_INTEGER largeInt;
	BOOL bRet = GetFileSizeEx(hFile, &largeInt);
	if (bRet == 0)
		throw FileReaderException(GetLastError());

	fileSize = (DWORD)largeInt.QuadPart;

	DWORD bytesRead;
	fileBuf = new char[fileSize];

	bRet = ReadFile(hFile, fileBuf, fileSize, &bytesRead, NULL);
	if (bRet == 0 || bytesRead != fileSize)
		throw FileReaderException(GetLastError());

	head = fileBuf;
	tail = fileBuf;

	CloseHandle(hFile);
}

FileReader::~FileReader()
{
	if (fileBuf != NULL)
		delete[] fileBuf;
}

bool FileReader::hasNext()
{
	return tail - fileBuf + 2 < fileSize;
}

char* FileReader::getNext()
{
	if (!hasNext())
		return NULL;

	tail = strchr(head, '\r\n');
	*tail = 0;
	
	char* h = head;
	head = tail + 2;

	return h;
}