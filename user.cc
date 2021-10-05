#include "user.h"
#include "common.h"
#include <arpa/inet.h>


user::user():sockfd_(-1),name_("noname"), 
            heartbeat_count_(3), addr_(),
            message_() {
    DBG("construct user: sockfd[%d].", sockfd_);
    print();
}

user::user(int sockfd, struct sockaddr_in addr):sockfd_(sockfd), name_("noname"), 
                                                heartbeat_count_(3), addr_(addr),
                                                message_() {
    DBG("construct user: sockfd[%d].", sockfd_);
    print();
}

user::~user() {
    DBGIN("destory user");
    print();
    DBGOUT("destory user");
}

void user::print() {
    DBG("user::print(sockfd{%d})", sockfd_);
    //DBG("name: %p, %s", name_.c_str(), name_.c_str() ? name_.c_str() : "null");
    DBG("user=> name{%s} sockfd{%d} heartbeat{%d} addr{%s}", name_.c_str() ? name_.c_str() : "null", sockfd_, heartbeat_count_, inet_ntoa(addr_.sin_addr));
    // DBG("(noname)user=> %d %d %s\n", sockfd_, heartbeat_count_, inet_ntoa(addr_.sin_addr));
    // DBG("name:%s", name_.c_str() ? name_.c_str() : "null");
    message_.print();
}

user& user::operator=(user &&other) {    
    DBGIN(" user::operator=");
    other.print();
    this->print();

    this->message_ = move(other.message_);

    DBG("other.name: %s", other.name_.c_str());
    DBG("this->name: %s", this->name_.c_str());

    // must be deep copy!!!!!
    this->name_ = other.name_;
    DBGIN("in mid: user::operator=");

    this->sockfd_ = other.sockfd_;

    this->heartbeat_count_ = other.heartbeat_count_;

    this->addr_ = other.addr_;



    DBGOUT(" user::operator=");
}