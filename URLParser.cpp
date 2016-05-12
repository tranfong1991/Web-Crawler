/*
Name: Phong Tran
Class: CSCE 463-500
*/

#include "URLParser.h"

using namespace std;

URLEntry* URLParser::parse(char* url)
{
	URLEntry* urlEntry = new URLEntry();

	//check for valid scheme
	char* scheme = strstr(url, "http://");
	if (scheme == NULL || scheme != url)
		throw InvalidSchemeException();

	//move the url pointer to the beginning of the host name
	url += 7;

	//find and remove fragment
	char* pound = strchr(url, '#');
	if (pound != NULL)
		*pound = '\0';

	//find the first special character (:, ?, /), starting from after http://
	char* ptr = url;
	while (*ptr != '/' && *ptr != ':' && *ptr != '?' && *ptr != '\0')
		ptr++;

	//get host name
	urlEntry->hostName = new char[ptr - url + 1];
	Utils::copy(urlEntry->hostName, url, ptr - url);
	urlEntry->hostName[ptr - url] = '\0';

	//get port number
	if (*ptr == ':'){
		char* temp = ptr + 1;
		while (*ptr != '/' && *ptr != '?' && *ptr != '\0')
			ptr++;

		//when no port is specified
		if (ptr - temp == 0)
			throw InvalidPortException();

		urlEntry->portNumber = new char[ptr - temp + 1];
		Utils::copy(urlEntry->portNumber, temp, ptr - temp);
		urlEntry->portNumber[ptr - temp] = '\0';

		if (!Utils::isValidPort(urlEntry->portNumber))
			throw InvalidPortException();
	}
	//default port number
	else {
		urlEntry->portNumber = new char[3];
		Utils::copy(urlEntry->portNumber, "80\0", 3);
	}

	//get request
	if (*ptr == '/'){
		urlEntry->request = new char[strlen(ptr) + 1];
		Utils::copy(urlEntry->request, ptr, strlen(ptr) + 1);
	}
	else if (*ptr == '?'){
		urlEntry->request = new char[strlen(ptr) + 2];
		urlEntry->request[0] = '/';
		Utils::copy(urlEntry->request + 1, ptr, strlen(ptr) + 1);
	}
	else if (*ptr == '\0'){
		urlEntry->request = new char[2];
		urlEntry->request[0] = '/';
		urlEntry->request[1] = '\0';
	}

	return urlEntry;
}
