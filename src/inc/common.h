#pragma once

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
#include <vector>

#define SYSL_HEAD "%s:%d: "
#define SYSL_ARG __FUNCTION__, __LINE__
#define MAX_CURL_NUM 1000

struct Log {
    uint bind_id;
    long status_code;
    double response_time;
    long redirs;
    bool ssl_verify;
};