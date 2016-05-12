/*
Name: Phong Tran
Class: CSCE 463-500
*/

#include "Crawler.h"

Crawler::Crawler()
{
}

Crawler::Crawler(char* u) :url(u)
{
}

Crawler::~Crawler()
{
}

void Crawler::parseUrl()
{
	if (entry != NULL)
		delete entry;
	entry = parser.parse(url);
}

int Crawler::parsePage()
{
	int nLinks;
	char* response = socket.getResponse();
	htmlParser.Parse(response, strlen(response), url, strlen(url), &nLinks);

	//in case of error
	if (nLinks < 0)
		nLinks = 0;

	return nLinks;
}

bool Crawler::dns()
{
	DWORD ip = inet_addr(entry->hostName);
	if (ip == INADDR_NONE){
		struct hostent* remote = gethostbyname(entry->hostName);

		if (remote == NULL){
			if (displayMsg)
				printf("failed with %d\n", WSAGetLastError());
			return false;
		}

		char* ipAddr = inet_ntoa(*((struct in_addr *) remote->h_addr));
		entry->ip = new char[strlen(ipAddr) + 1];
		Utils::copy(entry->ip, ipAddr, strlen(ipAddr) + 1);
	}
	else {
		entry->ip = new char[strlen(entry->hostName) + 1];
		Utils::copy(entry->ip, entry->hostName, strlen(entry->hostName) + 1);
	}

	return true;
}

bool Crawler::connect(const string& method)
{
	socket.setURLEntry(entry);

	//connect to host
	if (!socket.connectToHost())
		return false;

	//send request
	if (method == "GET"){
		if (!socket.sendHttpRequest())
			return false;
	}
	else if (method == "HEAD"){
		if (!socket.sendRobotsRequest())
			return false;
	}

	return true;
}

/*
maxBytes = 0 means no limits
totalTimeOut = 0 means no timeout
*/
bool Crawler::load(int maxBytes, int totalTimeOut)
{
	if (maxBytes < 0 || totalTimeOut < 0)
		return false;

	if (!socket.read(maxBytes, totalTimeOut))
		return false;

	char* response = socket.getResponse();
	if (!Utils::isValidHeader(response)){
		if(displayMsg)
			printf("failed with non-HTTP header\n");
		socket.closeSocket();
		return false;
	}

	if (httpCode != NULL)
		delete[] httpCode;
	httpCode = Utils::getStatusCode(response);

	return true;
}

bool Crawler::verify(const string& statusCode)
{
	if (displayMsg)
		printf("status code %s\n", httpCode);

	return httpCode[0] == statusCode[0];
}

void Crawler::printHeader()
{
	char* response = socket.getResponse();

	//print header
	printf("\n--------------------------------------\n");
	char* specialChar = strchr(response, '<');
	if (specialChar == NULL)
		printf("%s\n", response);
	else {
		*specialChar = 0;
		printf(response);
	}
}
