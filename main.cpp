/*
Name: Phong Tran
Class: CSCE 463-500
Acknowledgement: 463-sample, HTMLParserTest, homework handout
*/

#pragma once
#include "stdafx.h"
#include <sstream>
#include "SinglePageCrawler.h"
#include "SingleThreadedCrawler.h"
#include "MultiThreadedCrawler.h"

int _tmain(int argc, _TCHAR *argv[])
{
	try{
		SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);

		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(2, 2);
		if (WSAStartup(wVersionRequested, &wsaData) != 0) {
			printf("WSAStartup error %d\n", WSAGetLastError());
			WSACleanup();
			return 0;
		}

		//check the arguments
		if (argc > 3 || argc < 2){
			printf("failed with too few or too many arguments\n");
			WSACleanup();
			return 0;
		}

		char* firstArg = new char[MAX_URL_LENGTH];
		wcstombs(firstArg, argv[1], wcslen(argv[1]) + 1);

		//part 1
		if (argc == 2){
			SinglePageCrawler spc = SinglePageCrawler(firstArg);
			spc.crawl();
		}
		//part 2
		else if (Utils::isValidNum(firstArg) && atoi(firstArg) == 1){
			SingleThreadedCrawler stc = SingleThreadedCrawler(argv[2]);
			stc.crawl();
		}
		//part 3
		else if (atoi(firstArg) > 1){
			MultiThreadedCrawler::crawl(atoi(firstArg), argv[2]);
		}

		printf("\n\n");
		WSACleanup();
		return 0;
	}
	catch (InvalidSchemeException){
		printf("failed with invalid scheme\n");
	}
	catch (InvalidPortException){
		printf("failed with invalid port\n");
	}
	catch (FileReaderException e){
		printf("failed with error %d\n", e.errorCode);
	}
	catch (...){
		printf("Unknown Exception!\n");
	}
}