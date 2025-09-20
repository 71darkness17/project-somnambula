#include "inc/somnDaemon.h"

SomnDaemon::SomnDaemon()
{
    // for(int i = 0; i<MAX_CURL_NUM; ++i){
    //     curl[i] = curl_easy_init();
    //     curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    //     curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,1L);
    //     curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,2L);
    // }

}
SomnDaemon::~SomnDaemon()
{

    // for(int i = 0; i<MAX_CURL_NUM; ++i){
    //     curl_multi_remove_handle(cm,curl[i]);
    //     curl_easy_cleanup(curl[i]);
    // }
    // curl_multi_cleanup(cm);
}
void SomnDaemon::refresh_cm_poll(const std::vector<std::string> &url_list)
{
    cm = curl_multi_init();
    curl.clear();
    urls = url_list;
    curl.resize(urls.size());
    for(int i = 0;i<urls.size();++i){
        curl[i] = curl_easy_init();
        curl_easy_setopt(curl[i], CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl[i],CURLOPT_SSL_VERIFYPEER,1L);
        curl_easy_setopt(curl[i],CURLOPT_SSL_VERIFYHOST,2L);
        curl_easy_setopt(curl[i], CURLOPT_URL, urls[i].c_str());

        curl_multi_add_handle(cm,curl[i]);

    }
}
int SomnDaemon::procces_cm(std::vector<Log> &logs){
    logs.clear();
    Log log = {0,0,0,0,0};
    int running_hadles = 0;
    do {
        curl_multi_perform(cm,&running_hadles);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }while(running_hadles);
    do {
      int msgq = 0;
      msg = curl_multi_info_read(cm, &msgq);
      if(msg && (msg->msg == CURLMSG_DONE)) {
        CURL *e = msg->easy_handle;

        curl_easy_getinfo(e,CURLINFO_RESPONSE_CODE,&(log.status_code));
        curl_easy_getinfo(e,CURLINFO_TOTAL_TIME,&(log.response_time));
        curl_easy_getinfo(e,CURLINFO_REDIRECT_COUNT,&(log.redirs));
        int ssl;
        curl_easy_getinfo(e,CURLINFO_SSL_VERIFYRESULT,&(ssl));

        log.ssl_verify = (ssl == 0);

        curl_multi_remove_handle(cm, e);

        curl_easy_cleanup(e);

      }
      logs.push_back(log);
      log = {0,0,0,0,0};
    } while(msg);
    curl_multi_cleanup(cm);
    return 0;
}