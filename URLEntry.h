/*
Name: Phong Tran
Class: CSCE 463-500
*/

#pragma once
#include<string>

using namespace std;

//struct for storing basic url properties
struct URLEntry
{
	char* hostName = NULL;
	char* portNumber = NULL;
	char* request = NULL;
	char* ip = NULL;

	URLEntry(){
	}

	~URLEntry(){
		if (hostName != NULL)
			delete[] hostName;
		if (portNumber != NULL)
			delete[] portNumber;
		if (request != NULL)
			delete[] request;
		if (ip != NULL)
			delete[] ip;
	}
};