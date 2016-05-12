/*
Name: Phong Tran
Class: CSCE 463-500
*/

#include "MultiThreadedCrawler.h"

static int elapsedTime = 0;
static int urlsExtracted = 0;
static int threadsRunning = 0;
static int bytesDownloaded = 0;
static int pagesDownloaded = 0;
static int linksCount = 0;
static int successfulDNSCount = 0;
static int successfulRobotsCheckCount = 0;
static int successfulUrlCrawledCount = 0;

static FileReader* reader;
static queue<char*> urlQueue;
static unordered_set<string> hostSet;
static unordered_set<string> ipSet;
static map<char, int> codeMap;

bool MultiThreadedCrawler::connectLoadVerify(Crawler& crawler, const string& method, const string& statusCode)
{
	if (!crawler.connect(method))
		return false;

	if (!crawler.load(0, 0))
		return false;

	if (!crawler.verify(statusCode))
		return false;

	return true;
}

//deposit thread
DWORD WINAPI MultiThreadedCrawler::depositRun(LPVOID pParam)
{
	Parameters* p = (Parameters*)pParam;

	//deposit urls into queue
	while (reader->hasNext()){
		WaitForSingleObject(p->mutex, INFINITE);
		urlQueue.push(reader->getNext());
		ReleaseMutex(p->mutex);
		ReleaseSemaphore(p->semaphore, 1, NULL);
	}

	SetEvent(p->eventReaderFinished);

	return 0;
}

//stat print thread
DWORD WINAPI MultiThreadedCrawler::statRun(LPVOID pParam)
{
	Parameters* p = (Parameters*)pParam;
	int prevPagesDownloaded = pagesDownloaded;
	int prevBytesDownloaded = bytesDownloaded;

	elapsedTime = 2;

	while (WaitForSingleObject(p->eventStatFinished, 2000) == WAIT_TIMEOUT){
		int pd = pagesDownloaded - prevPagesDownloaded;
		int bd = bytesDownloaded - prevBytesDownloaded;

		printf("[%3d] %4d Q %6d E %7d H %6d D %6d I %5d R %5d C %5d ",
			elapsedTime, threadsRunning, urlQueue.size(),
			urlsExtracted, hostSet.size(), successfulDNSCount,
			ipSet.size(), successfulRobotsCheckCount,
			successfulUrlCrawledCount);
		if (linksCount > 999)
			printf("L %4dK\n", linksCount / 1000);
		else printf("L %4d\n", linksCount);
		printf("	*** crawling %.1f pps @ %.1f Mbps\n", pd / 2.0, (bd * 8) / 2000000.0);

		prevPagesDownloaded = pagesDownloaded;
		prevBytesDownloaded = bytesDownloaded;
		elapsedTime += 2;
	}

	return 0;
}

//crawl thread
DWORD WINAPI MultiThreadedCrawler::crawlRun(LPVOID pParam)
{
	Crawler crawler;
	Parameters* p = (Parameters*)pParam;
	HANDLE arr[] = { p->semaphore, p->eventReaderFinished };

	WaitForSingleObject(p->mutex, INFINITE);
	threadsRunning++;
	ReleaseMutex(p->mutex);

	//turn off message display
	crawler.switchDisplayMsg(false);

	while (WaitForMultipleObjects(2, arr, false, INFINITE) == WAIT_OBJECT_0){
		//extract url from queue
		WaitForSingleObject(p->mutex, INFINITE);
		char* url = urlQueue.front();
		urlQueue.pop();
		urlsExtracted++;
		ReleaseMutex(p->mutex);

		crawler.setUrl(url);
		crawler.parseUrl();
		URLEntry* entry = crawler.getURLEntry();

		//check host uniqueness
		WaitForSingleObject(p->mutex, INFINITE);
		if (!Utils::isUnique(&hostSet, entry->hostName)){
			ReleaseMutex(p->mutex);
			continue;
		}
		ReleaseMutex(p->mutex);

		//do dns
		if (!crawler.dns())
			continue;

		//increment if dns succeeds
		WaitForSingleObject(p->mutex, INFINITE);
		successfulDNSCount++;
		ReleaseMutex(p->mutex);

		//check ip uniqueness
		WaitForSingleObject(p->mutex, INFINITE);
		if (!Utils::isUnique(&ipSet, entry->ip)){
			ReleaseMutex(p->mutex);
			continue;
		}
		ReleaseMutex(p->mutex);

		//do robots
		if (!connectLoadVerify(crawler, "HEAD", "4xx"))
			continue;

		//increment if robots is valid (4xx)
		WaitForSingleObject(p->mutex, INFINITE);
		successfulRobotsCheckCount++;
		ReleaseMutex(p->mutex);

		//do page
		if (!crawler.connect("GET"))
			continue;

		if (!crawler.load(0, 0))
			continue;

		//increment http code counts
		WaitForSingleObject(p->mutex, INFINITE);
		char code = crawler.getHttpCode()[0];
		if (code == '2' || code == '3' || code == '4' || code == '5'){
			codeMap[code]++;
			successfulUrlCrawledCount++;
			bytesDownloaded += crawler.getResponseSize();
			pagesDownloaded++;
		}
		else codeMap['o']++;
		ReleaseMutex(p->mutex);

		if (!crawler.verify("2xx"))
			continue;

		//increment if the page is valid (2xx)
		WaitForSingleObject(p->mutex, INFINITE);
		linksCount += crawler.parsePage();
		ReleaseMutex(p->mutex);
	}

	//decrement the number of running threads
	WaitForSingleObject(p->mutex, INFINITE);
	if (--threadsRunning == 0)
		SetEvent(p->eventStatFinished);
	ReleaseMutex(p->mutex);

	return 0;
}

void MultiThreadedCrawler::printFinalStats()
{
	printf("Extracted %d URLs @ %.0f/s\n", urlsExtracted, urlsExtracted / (double)elapsedTime);
	printf("Looked up %d DNS names @ %.0f/s\n", successfulDNSCount, successfulDNSCount / (double)elapsedTime);
	printf("Downloaded %d robots @ %.0f/s\n", ipSet.size(), ipSet.size() / (double)elapsedTime);
	printf("Crawled %d pages @ %.0f/s (%.2f MB)\n", successfulUrlCrawledCount, successfulUrlCrawledCount / (double)elapsedTime, bytesDownloaded / 1000000.0);
	printf("Parsed %d links @ %.0f/s\n", linksCount, linksCount / (double)elapsedTime);
	printf("HTTP codes: 2xx = %d, 3xx = %d, 4xx = %d, 5xx = %d, other = %d\n", codeMap['2'], codeMap['3'], codeMap['4'], codeMap['5'], codeMap['o']);
}

void MultiThreadedCrawler::crawl(int threadsCount, _TCHAR* fileName)
{
	Parameters p;
	HANDLE* threads = new HANDLE[threadsCount + 2];

	//read file
	printf("Main thread: ");
	reader = new FileReader(fileName);
	printf("read file with size %d\n", reader->getFileSize());

	//create a mutex for accessing critical sections (including printf); initial state = not locked
	p.mutex = CreateMutex(NULL, 0, NULL);
	//create a semaphore that counts the number of active threads; initial value = 0, max = 1000000
	p.semaphore = CreateSemaphore(NULL, 0, 1000010, NULL);
	//create a reader finish event; manual reset, initial state = not signaled
	p.eventReaderFinished = CreateEvent(NULL, true, false, NULL);
	//create a quit event; manual reset, initial state = not signaled
	p.eventStatFinished = CreateEvent(NULL, true, false, NULL);

	threads[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)depositRun, &p, 0, NULL);
	threads[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)statRun, &p, 0, NULL);

	for (int i = 0; i < threadsCount; i++)
		threads[i + 2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)crawlRun, &p, 0, NULL);

	for (int i = 0; i < threadsCount + 2; i++){
		WaitForSingleObject(threads[i], INFINITE);
		CloseHandle(threads[i]);
	}

	printf("\n\n");
	printFinalStats();

	delete reader;
	delete[] threads;
}
