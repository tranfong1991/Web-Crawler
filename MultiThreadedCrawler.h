/*
Name: Phong Tran
Class: CSCE 463-500
*/

#pragma once
#include "Crawler.h"
#include "FileReader.h"
#include <map>
#include <queue>
#include <unordered_set>

//this class is passed to all threads, acts as shared memory
class Parameters {
public:
	HANDLE mutex;
	HANDLE semaphore;
	HANDLE eventReaderFinished;
	HANDLE eventStatFinished;
};

//part 3
class MultiThreadedCrawler
{
	static bool connectLoadVerify(Crawler& crawler, const string& method, const string& statusCode);
	static DWORD WINAPI depositRun(LPVOID pParam);
	static DWORD WINAPI statRun(LPVOID pParam);
	static DWORD WINAPI crawlRun(LPVOID pParam);
	static void printFinalStats();
public:
	static void crawl(int threadsCount, _TCHAR* fileName);
};

