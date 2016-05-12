/*
Name: Phong Tran
Class: CSCE 463-500
*/

#include "SinglePageCrawler.h"

SinglePageCrawler::SinglePageCrawler(char* u) :url(u), crawler(u)
{
}

SinglePageCrawler::~SinglePageCrawler()
{
}

void SinglePageCrawler::crawl()
{
	int start, end;

	printf("URL: %s\n", url);

	printf("	 Parsing URL... ");
	crawler.parseUrl();
	URLEntry* urlEntry = crawler.getURLEntry();
	printf("host %s, port %s, request %s\n", urlEntry->hostName, urlEntry->portNumber, urlEntry->request);

	//do DNS
	printf("	 Doing DNS... ");
	start = clock();
	if (!crawler.dns())
		return;
	end = clock();
	printf("done in %.0f ms, found %s\n", Utils::duration(start, end), urlEntry->ip);

	//connect to host
	printf("	*Connecting on page... ");
	start = clock();
	if (!crawler.connect("GET"))
		return;
	end = clock();
	printf("done in %.0f ms\n", Utils::duration(start, end));

	//load page
	printf("	 Loading... ");
	start = clock();
	if (!crawler.load(0, 0))
		return;
	end = clock();
	printf("done in %.0f ms with %d bytes\n", Utils::duration(start, end), crawler.getResponseSize());

	//verify page
	printf("	 Verifying header... ");
	if (!crawler.verify("2xx"))
		return;

	//get number of links
	printf("	+Parsing page... ");
	start = clock();
	int linksCount = crawler.parsePage();
	end = clock();
	printf("done in %.0f ms with %d links\n", Utils::duration(start, end), linksCount);

	//print header
	crawler.printHeader();
}
