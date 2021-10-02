/*************************************************************************
	> File Name: server.h
	> Author: tcy
	> Mail: 1281042827@qq.com
	> Created Time: Tue Sep 21 11:29:41 2021
 ************************************************************************/

#ifndef _USER_H
#define _USER_H

#include "common.h"
#include <string>

struct user{
public:
    user();

    ~user() = default;

    friend class server;

private:
    std::string name_;

    int sockfd_;

    int headtbeat_count_;
};


#endif