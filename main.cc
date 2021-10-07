/*************************************************************************
	> File Name: main.cpp
	> Author: tcy
	> Mail: 1281042827@qq.com
	> Created Time: Tue Sep 21 11:29:41 2021
 ************************************************************************/


#include "server.h"
#include "common/common.h"
#include <glog/logging.h>
#include <iostream>
#include <getopt.h>
#include <memory>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>

const int six = 666;

void run(int port) {
    sigset_t signal_mask_;
    sigemptyset(&signal_mask_);
    sigaddset(&signal_mask_ , SIGINT);
    sigaddset(&signal_mask_ , SIGQUIT);
    sigaddset(&signal_mask_ , SIGALRM);
    if(pthread_sigmask(SIG_BLOCK, &signal_mask_ , nullptr) != 0) {
        perror("error in pthread_sigmask");
        exit(errno);
    }

    DBG("run\n");

    if (port < 0) {
        server::init_ptr();
    } else {
        server::init_ptr(port);
    }
    server::get_ptr()->run();
}

int main(int argc, char **argv) {
    //google::InitGoogleLogging("HelenXR_glog_program");
    //LOG(INFO) << "google log first info level message_header!";
    DBG("argc: %d\n", argc);
    int port = -1;
    int opt;
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            default:
                std::cerr << "Usage : " << argv[0] << "-p port!" << std::endl;
                exit(errno);
        }
    }

    run(port);
}