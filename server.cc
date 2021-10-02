
#include "server.h"
#include "config.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>


std::shared_ptr<server> server::server_ptr_;

// 1.socket(ipv4, 流，填0就行)：创建socket的tcp套接字
// 2.addr.sin_family、addr.sin_port、addr.sin_addr.s_addr：设置好他们的值
// 3.bind(sockfd，addr地址，addr大小)：绑定到服务器socket
// 4.listen(sockfd, 最大等待连接数):设为监听套接字。
// backlog argument defines the maximum length to which the queue of pending
// connections for sockfd may grow.nt sockfd;
server::server(int port) {
    init_server(port);
}

server::server() {
    if (config::get_config_ptr() == nullptr) {
        std::cerr << "config_ptr_ is nullptr." << std::endl;
        exit(errno);
    }
    auto &config = config::get_config_ptr()->config_;
    const int port = atoi(config["port"].c_str());
    init_server(port);
}

void server::init_server(int port) {
    if (config::get_config_ptr() == nullptr) {
        std::cerr << "config_ptr_ is nullptr." << std::endl;
        exit(errno);
    }
    auto &config = config::get_config_ptr()->config_;
    
    max_online_user = atoi(config["max_online_user"].c_str());
    pthread_rwlock_init(&rwlock, NULL);

    try
    {
        server_listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;//ipv4
        addr.sin_port = htons(port);//端口号
        addr.sin_addr.s_addr = inet_addr("0.0.0.0");//敞开大门，随意连接
        if (bind(server_listen_fd_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("bind");
            exit(errno);
        }
        if (listen(server_listen_fd_, max_conn_waiting_num_) < 0) {
            perror("listen");
            exit(errno);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "server constructor: " << e.what() << '\n';
        exit(errno);
    }
}

server *server::get_ptr() {
    if (server_ptr_ == nullptr) {
        server_ptr_.reset(new server());
    }
    return server_ptr_.get();
}

void server::init_ptr(int port) {
    server_ptr_.reset(new server(port));
}

void server::init_ptr() {
    server_ptr_.reset(new server());
}

void server::do_verify_token() {
    const int epollfd = epoll_create(1);

    // add the read_pipe to the epoll
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = read_pipe;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, read_pipe, &ev) < 0) {
        perror("eoill_ctr");
        exit(errno);
    }

    struct epoll_event events[max_online_user];
    while(1) {
        DBG("in epoll_wait");
        const int nfds = epoll_wait(epollfd, events, max_online_user, -1);
        DBG("out epoll_wait");
        if (nfds == -1) {
            perror("epoll_wait");
            exit(errno);
        }
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == read_pipe) {
                DBG("in read_pipe");
                int new_client_fd;
                while (read(read_pipe, &new_client_fd,
                            sizeof(new_client_fd)) > 0) {
                    DBG("new_client_fd = %d", new_client_fd);
                    ev.data.fd = new_client_fd;
                    DBG("in epoll_ctl");
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD,
                                    new_client_fd, &ev) < 0) {
                        perror("epoll_ctr");
                        exit(errno);
                    }
                    DBG("out epoll_ctl");
                }
                DBG("out read_pipe");
            } else {
                DBG("in verify user");
                // verify user with token_tmp
                char token_tmp[100];
                const int new_client_fd = events[i].data.fd;
                epoll_ctl(epollfd, EPOLL_CTL_DEL, new_client_fd, NULL);
                if ((recv(new_client_fd, token_tmp, sizeof(token_tmp), 0)) <= 0) {
                    close(new_client_fd);
                    std::cout << "New sockfd ["<< server_listen_fd_ << "] connection error!" << std::endl;
                } else if (strcmp(token_tmp,
                        config::get_config_ptr()->config_["token"].c_str()) <= 0) {
                    close(new_client_fd);
                    std::cout << "New sockfd ["<< server_listen_fd_ << "] failed the verification!" << std::endl;
                    continue;
                } else {

                    std::cout << "New sockfd ["<< server_listen_fd_ << "] passed the verification!" << std::endl;
                }
                DBG("out verify user");
            }
        }
    }
}

// 1. Communicat with do_verify_token() by pipe.see: https://www.javaroad.cn/questions/47889
//    do_login() would add one client_sockfd to pipe once one client_sockfd is created.
void server::do_login() {
    while(1) {
        DBG("in do_login");
        int new_client_fd = accept(server_listen_fd_, NULL, NULL);
        printf("New sockfd %d try to connect.\n", server_listen_fd_);

        if (new_client_fd < 0) {
            perror("accept");
            exit(errno);
        }

        write(write_pipe, &new_client_fd, sizeof(new_client_fd));

        // Use pthread_rwlock_wlock to protect the pthread safety.
        // pthread_rwlock_wrlock(&rwlock);
        // waiting_verify_token_sockfd_set_.insert(new_client_fd);
        // pthread_rwlock_unlock(&rwlock);


    }
}

void server::init_pipe() {
    int pipefds[2] = {};
    pipe(pipefds);
    read_pipe = pipefds[0];
    write_pipe = pipefds[1];

    // make read-end non-blocking
    if (fcntl(read_pipe, F_SETFL, O_NONBLOCK) != 0) {
        perror("fcntl");
        exit(errno);
    }
}

void *server::call_back(void *arg) {
    server::call_back_arg_struc *p_node = (server::call_back_arg_struc*)arg;
    auto obj = p_node->obj;
    auto func = p_node->func;
    (obj.*func)();
    delete p_node;
}

void server::start_listen_conn() {
    init_pipe();

    call_back_arg_struc *login_tid_struc = new call_back_arg_struc{*this, &server::do_login};
    call_back_arg_struc *verify_token_tid_struc = new call_back_arg_struc{*this, &server::do_verify_token};

    pthread_create(&login_tid, nullptr, call_back, login_tid_struc);
    pthread_create(&verify_token_tid, nullptr, call_back, verify_token_tid_struc);

    pthread_join(login_tid, nullptr);
    pthread_join(verify_token_tid, nullptr);
}