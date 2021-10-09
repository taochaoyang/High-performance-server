#include "user.h"
#include "common.h"
#include <arpa/inet.h>

const int user::LIMIT_HEARTBEAT_COUNT_ = 4;

user::user():sockfd_(-1),name_("noname"), 
            heartbeat_count_(LIMIT_HEARTBEAT_COUNT_), addr_(),
            message_(), is_validated_(false) {
    // DBG("construct user: sockfd[%d].", sockfd_);
    // print();
}

user::user(int sockfd, struct sockaddr_in addr ):sockfd_(sockfd), name_("noname"), 
                                                heartbeat_count_(LIMIT_HEARTBEAT_COUNT_), addr_(addr),
                                                message_(), is_validated_(false) {
    DBG("construct user: sockfd[%d].", sockfd_);
    // print();
}

user::~user() {
    // DBGIN("destory user");
    // print();
    // DBGOUT("destory user");
}

void user::print() {
    DBG("user::print(sockfd{%d})", sockfd_);
    //DBG("name: %p, %s", name_.c_str(), name_.c_str() ? name_.c_str() : "null");
    DBG("user=> name{%s} sockfd{%d} heartbeat{%d} addr{%s}", name_, sockfd_, heartbeat_count_, inet_ntoa(addr_.sin_addr));
    // DBG("(noname)user=> %d %d %s", sockfd_, heartbeat_count_, inet_ntoa(addr_.sin_addr));
    // cout << "name=" << name_ << endl;
    // DBG("name:%s", name_.c_str() ? name_.c_str() : "null");
    message_.print();
}

user& user::operator=(user &&other) {    
    DBGIN(" user::operator=");
    other.print();
    this->print();

    this->message_ = move(other.message_);

    // DBG("other.name: %s", other.name_.c_str());
    // DBG("this->name: %s", this->name_.c_str());

    // must be deep copy!!!!!
    // this->name_ = other.name_;
    // this->name_ = other.name_;
    // other.name_.clear();
    // DBG("name:%s", name_.c_str() ? name_.c_str() : "null");

    this->sockfd_ = other.sockfd_;

    this->heartbeat_count_ = other.heartbeat_count_;

    this->addr_ = other.addr_;



    DBGOUT(" user::operator=");
}