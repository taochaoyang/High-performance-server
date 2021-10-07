#ifndef _MESSAGE_HEAD_H
#define _MESSAGE_HEAD_H


#include <inttypes.h> 

#define TP_MESSAGE 0x01
#define TP_ACK 0x02
#define TP_FIN 0x04
#define TP_HEARTBEAT 0x08
struct message_header {
    uint32_t content_size_;
    uint32_t type_;
};
#define limit_CONTENT_LENGTH 4096



#endif