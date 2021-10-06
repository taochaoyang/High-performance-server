#ifndef _MESSAGE_HEAD_H
#define _MESSAGE_HEAD_H


#include <inttypes.h> 

#define TP_MESSAGE 1
#define TP_ACK 2
#define TP_FIN 4
struct message_header {
    uint32_t content_size_;
    uint32_t type_;
};
#define limit_CONTENT_LENGTH 4096



#endif