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
#include <netinet/in.h>
#include "message_header.h"
#include "message.h"

struct user{

public:
    user();

    user(int sockfd, struct sockaddr_in addr);
    
    user& operator=(user &&other);

    ~user();

    void print();

    friend class server;

private:
    std::string name_;

    int sockfd_;

    int heartbeat_count_;

    struct sockaddr_in addr_;

    message message_;

};


#endif