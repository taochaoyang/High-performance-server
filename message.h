#ifndef _MESSAGE_H
#define _MESSAGE_H



#include "message_header.h"
#include "common.h"

struct message {
public:
    enum CHECK_STATE{CHECK_STATE_HEADER, CHECK_STATE_CONTENT};

public:
    message();

    ~message();

    message& operator=(message &&other);

    void print();

friend class server;

private:
    message_header header_;

    int header_index_;

    char *content_pr_;

    int content_index_;

    CHECK_STATE check_state_;

};




#endif