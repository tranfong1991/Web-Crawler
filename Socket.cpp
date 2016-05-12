/*
Name: Phong Tran
Class: CSCE 463-500
Acknowledgement: read() function from the homework paper, connect() function from 463-sample 
*/

#include "Socket.h"

using namespace std;

Socket::Socket()
{
	buf = new char[INITIAL_BUF_SIZE];
	buf[0] = '\0';	//to indicate this is an empty buffer
	allocatedSize = INITIAL_BUF_SIZE;
	curPos = 0;
}

Socket::Socket(URLEntry* entry){
	buf = new char[INITIAL_BUF_SIZE];
	buf[0] = '\0';	//to indicate this is an empty buffer
	allocatedSize = INITIAL_BUF_SIZE;
	curPos = 0;
	urlEntry = entry;
}

Socket::~Socket()
{
	closesocket(sock);
	if (buf != NULL)
		delete[] buf;
}

//double the size of buffer
void Socket::resizeBuffer()
{
	allocatedSize *= 2;
	buf = (char*)realloc(buf, allocatedSize);
}

void Socket::resetBuffer()
{
	char * newBuffer = new char[INITIAL_BUF_SIZE];

	newBuffer[0] = '\0';
	allocatedSize = INITIAL_BUF_SIZE;
	curPos = 0;

	delete[] buf;
	buf = newBuffer;
}

bool Socket::connectToHost()
{
	struct sockaddr_in server;
	DWORD ip = inet_addr(urlEntry->ip);

	server.sin_addr.S_un.S_addr = ip;
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(urlEntry->portNumber));

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET){
		if(displayMsg)
			printf("failed with %d\n", WSAGetLastError());
		closesocket(sock);
		return false;
	}

	if (connect(sock, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) == SOCKET_ERROR){
		if (displayMsg)
			printf("failed with %d\n", WSAGetLastError());
		closesocket(sock);
		return false;
	}

	return true;
}

//send GET request for page statistics
bool Socket::sendHttpRequest()
{
	char format[] = "GET %s HTTP/1.0\r\nUser-agent: tamuCrawler/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n";

	//sendSize is the total length of the format, the host name, and the request plus 1 for the null-terminate character and less 2 for each %s
	int sendSize = strlen(urlEntry->hostName) + strlen(urlEntry->request) + strlen(format) - 3;
	char* buffer = new char[sendSize];
	int n = sprintf(buffer, format, urlEntry->request, urlEntry->hostName);

	if (send(sock, buffer, n, 0) != n){
		if (displayMsg)
			printf("failed with %d\n", WSAGetLastError());
		closesocket(sock);
		delete[] buffer;
		return false;
	}

	delete[] buffer;
	return true;
}

//send HEAD request for robots
bool Socket::sendRobotsRequest()
{
	char format[] = "HEAD /robots.txt HTTP/1.0\r\nUser-agent: tamuCrawler/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n";

	int sendSize = strlen(urlEntry->hostName) + strlen(format) - 1;
	char* buffer = new char[sendSize];
	int n = sprintf(buffer, format, urlEntry->hostName);

	if (send(sock, buffer, n, 0) != n){
		if (displayMsg)
			printf("failed with %d\n", WSAGetLastError());
		closesocket(sock);
		delete[] buffer;
		return false;
	}

	delete[] buffer;
	return true;
}

/*
download response to buffer until a specified limit is reach or total timeout is reached
limit = 0 means no limit
totalTimeOut = 0 means no timeout
*/
bool Socket::read(int maxBytes, int totalTimeOut)
{
	if (strlen(buf) != 0)
		resetBuffer();

	FD_SET fd;
	struct timeval timeout;	//wait for 10 seconds
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	int start, end, secondsElapsed = 0;

	while (true){
		int ret;
		FD_ZERO(&fd);
		FD_SET(sock, &fd);

		if (totalTimeOut != 0)
			start = clock();

		if ((ret = select(0, &fd, 0, 0, &timeout)) > 0){
			int bytes = recv(sock, buf + curPos, allocatedSize - curPos, 0);
			if (bytes == SOCKET_ERROR){
				if (displayMsg)
					printf("failed with %d on recv\n", WSAGetLastError());
				break;
			}

			curPos += bytes;

			if (maxBytes != 0 && curPos + 1 > maxBytes){
				if (displayMsg)
					printf("failed with exceeding max\n");
				break;
			}

			//when no more byte to read, append null-terminated character at the end
			if (bytes == 0){
				buf[curPos] = '\0';
				closesocket(sock);
				return true;
			}

			if (allocatedSize - curPos < THRESHOLD)
				resizeBuffer();

			if (totalTimeOut != 0){
				end = clock();
				secondsElapsed += (end - start) / (double)CLOCKS_PER_SEC;

				if (secondsElapsed >= totalTimeOut){
					if (displayMsg)
						printf("failed with slow download\n");
					break;
				}
			}
		}
		else if (ret == 0){
			if (displayMsg)
				printf("failed with timeout\n");
			break;
		}
		else {
			if (displayMsg)
				printf("failed with %d\n", WSAGetLastError());
			break;
		}
	}
	closesocket(sock);
	return false;
}
