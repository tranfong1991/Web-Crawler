/*
Name: Phong Tran
Class: CSCE 463-500
*/

#pragma once
#include <ctime>
#include <unordered_set>
#include <string>

using namespace std;

//contain helper functions
class Utils{
public:
	// return duration in miliseconds(ms)
	static	double duration(int start, int end)
	{
		return (end - start) * 1000 / (double)CLOCKS_PER_SEC;
	}

	//check if the header file start with HTTP/
	static bool isValidHeader(char* buf)
	{
		char* http = strstr(buf, "HTTP/");
		return http != NULL && http == buf;
	}

	static bool isValidPort(char* port)
	{
		if (!isValidNum(port))
			return false;

		//check if the port number is within range
		int num = atoi(port);
		if (num <= 0 || num > 65535)
			return false;

		return true;
	}

	static bool isValidNum(char* num)
	{
		//check if there is any non-digit in the port number
		for (int i = 0; i < strlen(num); i++){
			if (!isdigit(num[i]))
				return false;
		}
		return true;
	}

	static char* getStatusCode(char* buf)
	{
		char* status = new char[4];
		for (int i = 0; i < 3; i++)
			status[i] = buf[i + 9];
		status[3] = '\0';

		return status;
	}

	//insert and check if the host/ip is unique
	static bool isUnique(unordered_set<string>* mySet, char* str)
	{
		string s = string(str);
		int prevSize = mySet->size();
		mySet->insert(s);

		if (prevSize < mySet->size())
			return true;
		return false;
	}

	//copy chars from source to destination
	static void copy(char* dest, char* source, int num)
	{
		for (int i = 0; i < num; i++)
			dest[i] = source[i];
	}
};