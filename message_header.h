#ifndef _MESSAGE_HEAD_H
#define _MESSAGE_HEAD_H


#include <inttypes.h> 


struct message_header {
    enum MESSAGE_HEADER_TYPE{TP_MESSAGE=0x01, TP_ACK=0x02, TP_FIN=0x04, TP_HEARTBEAT=0x08,
                            TP_FAILED=0x10, TP_ERROR=0x20, TP_SUCCESS=0x40, TP_VERIFY=0x80};

    uint32_t content_size_;
    uint32_t type_;
};
#define limit_CONTENT_LENGTH 4096



#endif