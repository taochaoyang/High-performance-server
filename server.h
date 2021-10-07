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
	
	~server();

	static server *get_ptr();

	static void init_ptr();

	static void init_ptr(int port);

	void run();

	void make_nonblock(int fd);

	void test();

private:
	void init_pipe_and_epoll();

	void init_signal();

	static void *call_back(void *);

	void work_verify_token();

	void work_accept();

	void work_reactor();

	void signal_handler(int signal);
	
	void work_signal_emitter();

	void send_ack(const int ack, const int client_fd);

	enum MESSAGE_STATE{MESSAGE_COMPLETE, MESSAGE_ERROR, MESSAGE_OPEN};
	bool read_message_head(const int client_fd);

	bool read_message_content(const int client_fd);

	void heartbeat();

protected:
	server(int port);
	
	server();


	void init_server(int port);

private:
    static std::shared_ptr<server> server_ptr_;

	user *pr_users_;

	int server_listen_fd_;
	
	int limit_conn_waiting_num_;

	int limit_online_user_;

	int limit_sockfd_;

	int max_sockfd_;

	sigset_t signal_mask_;

    pthread_t login_tid;
	int read_pipe3_1;
	int write_pipe1;

	pthread_t verify_token_tid;
	int verify_token_epollfd;
	int read_pipe1;
	int write_pipe2;	
	int read_pipe3_2;

	pthread_t reactor_tid;
	int reactor_epollfd;
	int read_pipe2;
	int read_pipe3_3;

	pthread_t signal_handler_tid;
	// signal_hanlder exchange verify_token_tid and reactor_tid's heartbeat.
	int write_pipe3_1;
	int write_pipe3_2;
	int write_pipe3_3;
	//enum PIPE_MESSAGE_TYPE{PIPE_MESSAGE_SIGINT, PIPE_MESSAGE_SIGQUIT, PIPE_MESSAGE_SIGALRM};

	pthread_pool<user> threadpool_;


};



#endif
