#include "inc/somnd.h"
#include <sys/syslog.h>



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

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    
    
    openlog("somn", LOG_PID | LOG_CONS, LOG_LOCAL0);
    
    curl_global_init(CURL_GLOBAL_ALL);
    
    SomnDaemon somnd;

    std::vector<std::string> urls = {"https://google.com", "https://github.com","https://amazon.com","https://vk.com"};
    std::vector<Log> logs;
    int res = 0;

    while (!stop_flag) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        logs.clear();

        somnd.refresh_cm_poll(urls);

        res = somnd.procces_cm(logs);

        for(int i = 0;i < std::min(logs.size(),urls.size());++i){
            if(logs[i].status_code >= 400 || logs[i].status_code < 200){
                //notify
            }
            syslog(LOG_INFO,"%s: status: %ld, ssl verified: %s, redirs: %ld, time: %f",urls[i].c_str(),logs[i].status_code,(logs[i].ssl_verify ? "True" : "False"),logs[i].redirs,logs[i].response_time);
        }
        //dump logs
    }
    
    closelog();
    
    return EXIT_SUCCESS;
}