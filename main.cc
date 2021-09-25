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


void run(int port) {
    std::cout << "run" << std::endl;

    if (port < 0) {
        server::init_ptr();
    } else {
        server::init_ptr(port);
    }
    server::get_ptr()->keep_listen();

}

int main(int argc, char **argv) {
    //google::InitGoogleLogging("HelenXR_glog_program");
    //LOG(INFO) << "google log first info level message!";

    int port = -1;
    int opt;
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        std::cout << "111";
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            default:
                std::cerr << "Usage : " << argv[0] << "-p port!" << std::endl;
                exit(1);
        }
    }

    run(port);
}