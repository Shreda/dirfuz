#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <curl/curl.h>
#include "queue.h"

class dirfuz
{
    public:
        // curl context
        CURLM* curlm;
        CURLMsg* msg;
        unsigned int transfers;
        int msgsLeft;
        int stillAlive;

        // queue of urls to process
        tsqueue<std::string> urlQueue;

        // options
        static const std::string helpo;
        static const std::string urlo;
        static const std::string wordlisto;

        std::string url;
        std::string wordlist;

        static void PrintUsage();
        void ParseArgs(int argc, char* argv[]);
        int Run();
        void ProcessResponse(CURL* e);
        void BuildQueue();
        void ProcessQueue();
        void AddTransfer(std::string url);

};

