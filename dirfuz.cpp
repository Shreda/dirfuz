
#include "dirfuz.h"

static size_t write_cb(char* data, size_t n, size_t l, void* userp) {
    /* take care of the data here, ignored in this example */
    (void)data;
    (void)userp;
    return n * l;
}

void dirfuz::AddTransfer(std::string url) {
    CURL* eh = curl_easy_init();
    curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(eh, CURLOPT_URL, url.c_str());
    curl_multi_add_handle(this->curlm, eh);
}

/*
* Processes the queue
*/
void dirfuz::ProcessQueue() {
    curl_global_init(CURL_GLOBAL_ALL);
    this->curlm = curl_multi_init();
    curl_multi_setopt(this->curlm, CURLMOPT_MAXCONNECTS, (long)this->maxParallel);

    for (this->transfers = 0; this->transfers < this->maxParallel; this->transfers++) {
        if (!this->urlQueue.Empty()) {
            this->AddTransfer(this->urlQueue.PopFront());
        }
        else {
            break;
        }
    }

    do {
        curl_multi_perform(this->curlm, &(this->stillAlive));
        while ((this->msg = curl_multi_info_read(this->curlm, &(this->msgsLeft)))) {
            if (this->msg->msg == CURLMSG_DONE) {
                char* url;
                CURL* e = this->msg->easy_handle;

                this->ProcessResponse(e);

                curl_multi_remove_handle(this->curlm, e);
                curl_easy_cleanup(e);
            }
            else {
                std::cerr << "Error" << std::endl;
            }

            if (!this->urlQueue.Empty()) {
                this->AddTransfer(this->urlQueue.PopFront());
            }
        }
        if (this->stillAlive) {
            curl_multi_wait(this->curlm, NULL, 0, 1000, NULL);
        }
    } while (this->stillAlive || !this->urlQueue.Empty());
    curl_multi_cleanup(this->curlm);
    curl_global_cleanup();
}

/*
* Uses the base URL and wordlist to generate a queue of
* URLs to be requested.
*/
void dirfuz::BuildQueue() {
    std::ifstream wl;
    wl.open(this->wordlist, std::ios::in);
    if (wl.is_open()) {
        while (!wl.eof()) {
            char temp[1000];
            wl.getline(temp, 1000);
            std::string path = std::move(temp);
            this->urlQueue.PushBack(this->url + path);
        }
    }
    else {
        std::cerr << "Could not open wordlist.";
    }
    wl.close();
}

void dirfuz::ProcessResponse(CURL* e) {
    long responseCode;
    char* url;

    curl_easy_getinfo(e, CURLINFO_RESPONSE_CODE, &responseCode);
    curl_easy_getinfo(e, CURLINFO_EFFECTIVE_URL, &url);

    if (responseCode == 200) {
        std::cout << responseCode << " - " << url << std::endl;
    }

    else if (responseCode == 301 || responseCode == 302 || responseCode == 303) {
        char* redirect_url;
        curl_easy_getinfo(e, CURLINFO_REDIRECT_URL, &redirect_url);
        std::cout << responseCode << " -> " << redirect_url << std::endl;
    }
}

void dirfuz::ParseOptions(cxxopts::Options* options, int argc, char* argv[]) {
    cxxopts::ParseResult result;
    options->allow_unrecognised_options();
    try {
        result = options->parse(argc, argv);
    }
    catch (...) {
        std::cerr << "Error parsing commandline arguments. Check yourself, before you wreck yourself." << std::endl;
        exit(1);
    }

    if (result.count("help")) {
        std::cout << options -> help() << std::endl;
        exit(0);
    }
    
    else if ((result.count("url")) && (result.count("wordlist"))) {
        this->url = result["url"].as<std::string>();
        this->wordlist = result["wordlist"].as<std::string>();
        this->maxParallel = result["maxparallel"].as<unsigned int>();
    }

    else {
        std::cout << options->help() << std::endl;
        exit(0);
    }
}

int dirfuz::Run() {
    this->BuildQueue();
    this->ProcessQueue();
    return 0;
}

int main(int argc, char* argv[]) {
    dirfuz p(argc, argv);
    int code = p.Run();
    return code;
}

