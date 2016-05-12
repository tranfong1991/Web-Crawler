/*
Name: Phong Tran
Class: CSCE 463-500
*/

#pragma once
#include "Crawler.h"
#include "FileReader.h"
#include <unordered_set>

#define MAX_PAGE_SIZE 2000000
#define MAX_ROBOTS_SIZE 16000

//part 2
class SingleThreadedCrawler
{
	Crawler crawler;
	FileReader reader;
	unordered_set<string> hostSet;
	unordered_set<string> ipSet;

	bool connectLoadVerify(const string& method, const string& statusCode, int maxBytes, int totalTimeOut);
public:
	SingleThreadedCrawler(_TCHAR* fileName);
	~SingleThreadedCrawler();

	void crawl();
};

