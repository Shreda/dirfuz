
#include "dirfuz.h"

const std::string dirfuz::helpo("-h");
const std::string dirfuz::wordlisto("-w");
const std::string dirfuz::urlo("-u");

static size_t write_cb(char* data, size_t n, size_t l, void* userp)
{
    /* take care of the data here, ignored in this example */
    (void)data;
    (void)userp;
    return n * l;
}

void dirfuz::PrintUsage()
{
    std::cerr << "Usage:" << "\n" <<
                "OPTIONS" << "\n" << 
                "\t-h\tPrint usage.\n" <<
                "\t-u\tURL to fuzz.\n" <<
                "\t-w\tWordlist to use.\n";
}

void dirfuz::AddTransfer(std::string url)
{
    CURL* eh = curl_easy_init();
    curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(eh, CURLOPT_URL, url.c_str());
    curl_multi_add_handle(this->curlm, eh);
}

/*
* Processes the queue
*/
void dirfuz::ProcessQueue()
{
    curl_global_init(CURL_GLOBAL_ALL);
    this->curlm = curl_multi_init();
    curl_multi_setopt(this->curlm, CURLMOPT_MAXCONNECTS, (long)10);
    for (this->transfers = 0; this->transfers < 10; this->transfers++)
    {
        if (!this->urlQueue.Empty())
        {
            this->AddTransfer(this->urlQueue.PopFront());
        }
        else
        {
            break;
        }
    }

    do {
        curl_multi_perform(this->curlm, &(this->stillAlive));
        while ((this->msg = curl_multi_info_read(this->curlm, &(this->msgsLeft))))
        {
            if (this->msg->msg == CURLMSG_DONE)
            {
                char* url;
                CURL* e = this->msg->easy_handle;
                long responseCode;

                curl_easy_getinfo(this->msg->easy_handle, CURLINFO_RESPONSE_CODE, &responseCode);
                curl_easy_getinfo(this->msg->easy_handle, CURLINFO_EFFECTIVE_URL, &url);

                if (responseCode == 200) {
                    std::cout << responseCode << " - " << url << std::endl;
                }
                //std::cerr << "R: " << msg->data.result << " - " << this->msg->data.result << " " << curl_easy_strerror(this->msg->data.result) << " " << url << std::endl;
                curl_multi_remove_handle(this->curlm, e);
                curl_easy_cleanup(e);
            }
            else 
            {
                std::cerr << "Error" << std::endl;
            }

            if (!this->urlQueue.Empty())
            {
                this->AddTransfer(this->urlQueue.PopFront());
            }
        }
        if (this->stillAlive)
        {
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
void dirfuz::BuildQueue()
{
    std::ifstream wl;
    wl.open(this->wordlist, std::ios::in);
    if (wl.is_open()) 
    {
        while (!wl.eof())
        {
            char temp[1000];
            wl.getline(temp, 1000);
            std::string path = std::move(temp);
            this->urlQueue.PushBack(this->url + path);
        }
    }
    else 
    {
        std::cerr << "Could not open wordlist.";
    }
    wl.close();
}

/*
* Parses the command line arguments provided by the user
* and sets application variables such as URL and wordlist
* @param argc - number of arguments
* @param argv - pointer to a list of character pointers.
*         contains each arg passed on the command line
*         seperated by whitspace.
*/
void dirfuz::ParseArgs(int argc, char* argv[])
{
    if (argc <= 1) {
        dirfuz::PrintUsage();
        exit(0);
    }

    for (int i = 0; i < argc; ++i)
    {
        std::string arg = argv[i];
        // help
        if (arg.compare(helpo) == 0)
        {
            dirfuz::PrintUsage();
        }

        // url
        else if (arg.compare(urlo) == 0)
        {
            //check there is still commandline to process.
            if (i + 1 < argc)
            {
                //move to next item in commandline
                i++;
                this -> url = argv[i];
                std::cout << this -> url << std::endl;
            }
            else
            {
                std::cerr << "No URL provided.";
            }
        }

        // wordlist
        else if (arg.compare(wordlisto) == 0)
        {
            if (i + 1 < argc)
            {
                i++;
                this -> wordlist = argv[i];
                std::cout << this -> wordlist << std::endl;
            }
            else
            {
                std::cerr << "No wordlist provided." << std::endl;
            }
        }
    }
}

int dirfuz::Run() {
    this->BuildQueue();
    this->ProcessQueue();
    return 0;
}

int main(int argc, char* argv[])
{
    dirfuz p;
    p.ParseArgs(argc, argv);
    p.Run();
}
