/*
Name: Phong Tran
Class: CSCE 463-500
*/

#pragma once
#include <string>
#include "Utils.h"
#include "URLEntry.h"

using namespace std;

class InvalidSchemeException{};
class InvalidPortException{};

//parser for extracting host name/IP, port number, and request
class URLParser
{
public:
	URLParser(){};
	~URLParser(){};

	URLEntry* parse(char* url);
};

