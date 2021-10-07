#include "message.h"




message::message():header_(), header_index_(0), content_pr_(nullptr),
            content_index_(0), check_state_(CHECK_STATE_HEADER) {
    // DBG("construct message.");
    // print();
}

message::~message() {
    DBGOUT("destory message");
}

void message::print() {
    DBG("message::print");
    DBG("message=> {header_:{{%d,%d},index=%d},content={%s,index=%d},check_sate={%d}}\n", header_.content_size_, header_.type_, header_index_, content_pr_ ? content_pr_: "null", content_index_, check_state_);
}

void message::reset_content() {
    delete(content_pr_);
    content_pr_ = nullptr;
    content_index_ = 0;
    check_state_ = CHECK_STATE_HEADER;
}

message& message::operator=(message &&other) {
    DBGIN(" message::operator=");
    this->header_ = other.header_;

    this->header_index_ = other.header_index_;

    this->content_pr_ = other.content_pr_;

    other.content_pr_ = nullptr;

    this->content_index_ = other.content_index_;

    this->check_state_ = other.check_state_;

    DBGOUT(" message::operator=");
}