#include <chrono>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h> 
#include <sys/syslog.h>
#include <thread>
#include <curl/curl.h>
#include <string>

volatile sig_atomic_t stop_flag = 0;

void signal_handler(int signal) {
    stop_flag = 1;
}

size_t writeCallback(char *ptr, size_t size, size_t nmemb, std::string *userdata)
{
    size_t total = size * nmemb;
    userdata->append(ptr, total);
    return total;
}

int main() {
    // Устанавливаем обработчики сигналов
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    
    
    openlog("somn", LOG_PID | LOG_CONS, LOG_LOCAL0);

    curl_global_init(CURL_GLOBAL_ALL);

    CURL *cr;
    CURLcode cr_code;

    cr = curl_easy_init();
    if(!cr)
        std::exit(EXIT_FAILURE);
    curl_easy_setopt(cr,CURLOPT_URL,"https://google.com");
    curl_easy_setopt(cr, CURLOPT_WRITEFUNCTION, writeCallback);
    std::string buffer;
    curl_easy_setopt(cr, CURLOPT_WRITEDATA, &buffer);
    
    while (!stop_flag) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        cr_code = curl_easy_perform(cr);
        if(cr_code == CURLE_OK){
            syslog(LOG_INFO,"%s",buffer.substr(0,200).c_str());
        }
    }
    
    closelog();
    
    return EXIT_SUCCESS;
}