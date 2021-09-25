
#include "server.h"
#include "common/config.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>

std::shared_ptr<server> server::server_ptr_;

server::server(int port) {
    // 1.socket(ipv4, 流，填0就行)：创建socket的tcp套接字
    // 2.addr.sin_family、addr.sin_port、addr.sin_addr.s_addr：设置好他们的值
    // 3.bind(sockfd，addr地址，addr大小)：绑定到服务器socket
    // 4.listen(sockfd, 最大等待连接数):设为监听套接字。
    // backlog argument defines the maximum length to which the queue of pending
    // connections for sockfd may grow.nt sockfd;
    struct sockaddr_in addr;
    int sockfd;

    try
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        addr.sin_family = AF_INET;//ipv4
        addr.sin_port = htons(port);//端口号
        addr.sin_addr.s_addr = inet_addr("0.0.0.0");//敞开大门，随意连接
        bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
        listen(sockfd, 20) < 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << "server constructor: " << e.what() << '\n';
        exit(1);
    }

    server_listen_fd = sockfd;    
}

server::server() {
    int port;
    struct sockaddr_in addr;
    int sockfd;

    if (config::get_config_ptr() == nullptr) {
        std::cerr << "config_ptr_ is nullptr." << std::endl;
        exit(1);
    }

    const auto config = config::get_config_ptr()->config_;
    if (config.count("port") == 0) {
        std::cerr << "Not found port in config.txt." << std::endl;
        exit(1);
    } else {
        std::cout << "Get port from config.txt." << std::endl;
        port = atoi(config.at("port").c_str());
    }

    try
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        addr.sin_family = AF_INET;//ipv4
        addr.sin_port = htons(port);//端口号
        addr.sin_addr.s_addr = inet_addr("0.0.0.0");//敞开大门，随意连接
        bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
        listen(sockfd, 20) < 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << "server constructor: " << e.what() << '\n';
        exit(1);
    }

    server_listen_fd = sockfd;    
}

std::shared_ptr<server>  server::get_ptr() {
    if (server_ptr_ == nullptr) {
        server_ptr_.reset(new server());
    }
    return server_ptr_;
}

void server::init_ptr(int port) {
    server_ptr_.reset(new server(port));
}

void server::init_ptr() {
    server_ptr_.reset(new server());
}

void server::keep_listen() {
    while (1) {
        int new_fd;
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        if ((new_fd = accept(server_listen_fd, (struct sockaddr *)&client, &len)) < 0) {
            perror("accept");
            exit(1);
        }
        while (1) {
            while(1) {           
                printf("一个男孩说：");//会进入缓冲区，直到遇到回车
                char buff[1024] = {0};
                size_t rsize = recv(new_fd, buff, sizeof(buff), 0);
                if (rsize <= 0) {
                    close(new_fd);
                    break;
                }printf("\n");
                send(new_fd, "ACK", 4, 0);
                printf("<Recv %ld> : %s\n", rsize, buff);
            }
            printf("<Bye> : A boy left!\n");
        }
    }
}

