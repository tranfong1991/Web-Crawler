/*
Name: Phong Tran
Class: CSCE 463-500
*/

#pragma once
#include "Crawler.h"

//part 1
class SinglePageCrawler
{
	Crawler crawler;
	char* url;
public:
	SinglePageCrawler(char* url);
	~SinglePageCrawler();

	void crawl();
};

