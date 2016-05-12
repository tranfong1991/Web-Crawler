/*
Name: Phong Tran
Class: CSCE 463-500
*/

#pragma once
#include "HTMLParserBase.h"
#include "Socket.h"
#include "URLParser.h"
#include "URLEntry.h"
#include "Utils.h"

//responsible for connecting, doing dns, loading, verifying, and printing header
class Crawler
{
	Socket socket;
	URLParser parser;
	HTMLParserBase htmlParser;
	URLEntry* entry = NULL;
	char* url = NULL;
	char* httpCode = NULL;
	bool displayMsg = true;
public:
	Crawler();
	Crawler(char* url);
	~Crawler();

	void switchDisplayMsg(bool b){ displayMsg = b; socket.displayMsg = b; }
	void setUrl(char* url){ this->url = url;}
	char* getUrl() const { return url; }
	char* getHttpCode() const { return httpCode; }
	URLEntry* getURLEntry() const { return entry; }
	int getResponseSize() const { return socket.getBytes(); }

	void parseUrl();
	int parsePage();
	bool dns();
	bool connect(const string& method);
	bool load(int maxBytes, int totalTimeOut);
	bool verify(const string& statusCode);
	void printHeader();
};

