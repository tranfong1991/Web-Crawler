/*
Name: Phong Tran
Class: CSCE 463-500
*/

#pragma once
#include<winsock.h>
#include<iostream>
#include<ctime>
#include"URLEntry.h"

#define INITIAL_BUF_SIZE 8000
#define THRESHOLD 20

//wrapper class for SOCKET, takes care of making http request, connecting to host, and receiving response
class Socket
{
	//allows Crawler to access private fields
	friend class Crawler;

	SOCKET sock;
	char* buf;
	int allocatedSize;
	int curPos;
	URLEntry* urlEntry;
	bool displayMsg = true;

	void resizeBuffer();
	void resetBuffer();
public:
	Socket();
	Socket(URLEntry* entry);
	~Socket();

	void setURLEntry(URLEntry* entry){ urlEntry = entry; }
	int getBytes() const { return strlen(buf); }
	char* getResponse() const { return buf; }

	bool connectToHost();
	void closeSocket(){ closesocket(sock); }
	bool sendHttpRequest();
	bool sendRobotsRequest();
	bool read(int maxBytes, int totalTimeOut);
};

