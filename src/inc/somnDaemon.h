#pragma once

#include "common.h"

class SomnDaemon
{
private:
    CURLM *cm;
    std::vector<CURL*> curl;
    CURLMsg *msg;
    std::vector<std::string> urls;
    bool is_renewed = 0;
public:
    SomnDaemon();
    void refresh_cm_poll(const std::vector<std::string> &url_list);
    int procces_cm(std::vector<Log> &logs);
    ~SomnDaemon();
};