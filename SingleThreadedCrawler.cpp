/*
Name: Phong Tran
Class: CSCE 463-500
*/

#include "SingleThreadedCrawler.h"

SingleThreadedCrawler::SingleThreadedCrawler(_TCHAR* fn) :reader(fn)
{
}

SingleThreadedCrawler::~SingleThreadedCrawler()
{
}

bool SingleThreadedCrawler::connectLoadVerify(const string& method, const string& statusCode, int maxBytes, int totalTimeOut)
{
	int start, end;

	//connect to host
	if (method == "GET")
		printf("	*Connecting on page... ");
	else
		printf("	 Connecting on robots... ");

	start = clock();
	if (!crawler.connect(method))
		return false;
	end = clock();
	printf("done in %.0f ms\n", Utils::duration(start, end));

	//load page
	printf("	 Loading... ");
	start = clock();
	if (!crawler.load(maxBytes, totalTimeOut))
		return false;
	end = clock();
	printf("done in %.0f ms with %d bytes\n", Utils::duration(start, end), crawler.getResponseSize());

	//verify page
	printf("	 Verifying header... ");
	if (!crawler.verify(statusCode))
		return false;

	return true;
}

void SingleThreadedCrawler::crawl()
{
	int start, end;
	char* url = NULL;

	printf("Main thread: ");
	printf("read file with size %d\n", reader.getFileSize());

	while (reader.hasNext()){
		url = reader.getNext();
		crawler.setUrl(url);

		printf("URL: %s\n", url);

		printf("	 Parsing URL... ");
		crawler.parseUrl();
		URLEntry* urlEntry = crawler.getURLEntry();
		printf("host %s, port %s\n", urlEntry->hostName, urlEntry->portNumber);

		//check host uniqueness
		printf("	 Checking host uniqueness... ");
		if (!Utils::isUnique(&hostSet, urlEntry->hostName)){
			printf("failed\n");
			continue;
		}
		else printf("passed\n");

		//do DNS
		printf("	 Doing DNS... ");
		start = clock();
		if (!crawler.dns())
			continue;
		end = clock();
		printf("done in %.0f ms, found %s\n", Utils::duration(start, end), urlEntry->ip);

		//check IP uniqueness
		printf("	 Checking IP uniqueness... ");
		if (!Utils::isUnique(&ipSet, urlEntry->ip)){
			printf("failed\n");
			continue;
		}
		else printf("passed\n");

		//connect to host, load page, and verify header
		if (!connectLoadVerify("HEAD", "4xx", MAX_ROBOTS_SIZE, 10))
			continue;

		if (!connectLoadVerify("GET", "2xx", MAX_PAGE_SIZE, 10))
			continue;

		//get number of links
		printf("	+Parsing page... ");
		start = clock();
		int linksCount = crawler.parsePage();
		end = clock();
		printf("done in %.0f ms with %d links\n", Utils::duration(start, end), linksCount);
	}
}
