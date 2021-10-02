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
#include <set>

// Simple interest model.


class server {

public:
    // typedef void (server::*type_do_login)(void);
	using type_do_login = void (server::*)(void);
    struct call_back_arg_struc{
        server &obj;
        server::type_do_login func;
    };

	static server *get_ptr();

	static void init_ptr();

	static void init_ptr(int port);

	void start_listen_conn();

	void do_verify_token();

	void do_login();

	void heart();

	~server() = default;

private:
	void init_pipe();

	static void *call_back(void *);
protected:
	server(int port);
	
	server();

	void init_server(int port);

private:
    static std::shared_ptr<server> server_ptr_;

	std::set<int> waiting_verify_token_sockfd_set_;

	// sockfd->user
	std::map<int, user> users_;

	int server_listen_fd_;
	
	int max_conn_waiting_num_;

	int max_online_user;

	pthread_rwlock_t rwlock;
	int read_pipe;
	int write_pipe;
    pthread_t login_tid;
	pthread_t verify_token_tid;
};

#endif
