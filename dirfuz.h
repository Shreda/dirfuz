#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <curl/curl.h>
#include "queue.h"
#include "cxxopts.h"

class dirfuz
{
    public:
        dirfuz(int argc, char* argv[]) {
            options = new cxxopts::Options("dirfuz", "dirfuz - Simple and fast directory brute forcing.");
            options->add_options()
                ("u,url", "Base URL, for example https://www.example.com", cxxopts::value<std::string>())
                ("w,wordlist", "Wordlist to perform the brute force with", cxxopts::value<std::string>())
                ("m,maxparralel", "Maximum number of parallel requests to send at one time", cxxopts::value<int>()->default_value("20"))
                ("h,help", "Print usage");
            this->ParseOptions(options, argc, argv);
        }

        // curl context
        CURLM* curlm;
        CURLMsg* msg;
        unsigned int transfers;
        int msgsLeft;
        int stillAlive;
        
        // queue of urls to process
        tsqueue<std::string> urlQueue;

        cxxopts::Options* options;
        std::string url;
        std::string wordlist;

        int Run();
        void ProcessResponse(CURL* e);
        void BuildQueue();
        void ProcessQueue();
        void AddTransfer(std::string url);
        void ParseOptions(cxxopts::Options*, int, char* []);

};

