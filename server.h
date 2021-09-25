/*************************************************************************
	> File Name: server.h
	> Author: tcy
	> Mail: 1281042827@qq.com
	> Created Time: Tue Sep 21 11:29:41 2021
 ************************************************************************/

#ifndef _SERVER_H
#define _SERVER_H

#include <memory>

class server {

public:
	static std::shared_ptr<server> get_ptr();

	static void init_ptr();

	static void init_ptr(int port);

	void keep_listen();

protected:
	server(int port);
	
	server();

private:
    static std::shared_ptr<server> server_ptr_;

	int server_listen_fd;
};

#endif
