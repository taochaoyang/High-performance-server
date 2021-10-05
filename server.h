/*************************************************************************
	> File Name: server.h
	> Author: tcy
	> Mail: 1281042827@qq.com
	> Created Time: Tue Sep 21 11:29:41 2021
 ************************************************************************/

#ifndef _SERVER_H
#define _SERVER_H

#include "common.h"
#include <memory>
#include <map>
#include "user.h"
#include <pthread.h>
#include "pthread_poll.h"
#include <set>
#include "message_header.h"


// Simple interest model.


class server {

public:
    // typedef void (server::*type_func)(void);
	using type_func = void (server::*)(void);
    struct call_back_arg_struc{
        server *pr_obj;
        server::type_func func;
    };

	static server *get_ptr();

	static void init_ptr();

	static void init_ptr(int port);

	void start_listen_conn();

	void do_verify_token();

	void do_login();

	void do_reactor();

	~server() = default;

	void make_nonblock(int fd);

	void test();
private:
	void init_pipe();

	static void *call_back(void *);

	void heartbeat();

	void send_ack(const int ack, const int client_fd);

	enum MESSAGE_STATE{MESSAGE_COMPLETE, MESSAGE_ERROR, MESSAGE_OPEN};
	bool read_message_head(int client_fd);

	bool read_message_content(int client_fd);

protected:
	server(int port);
	
	server();

	void init_server(int port);

private:
    static std::shared_ptr<server> server_ptr_;

	user *pr_users_;

	int server_listen_fd_;
	
	int max_conn_waiting_num_;

	int max_online_user_;

	int max_sockfd_;

    pthread_t login_tid;
	int read_pipe1;
	int write_pipe1;
	pthread_t verify_token_tid;
	int read_pipe2;
	int write_pipe2;
	pthread_t reactor_tid;

	pthread_pool<message_header> threadpool_;


};



#endif
