
#include "server.h"
#include "config.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/epoll.h>
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
    
    limit_online_user_ = atoi(config["limit_online_user_"].c_str());
    limit_sockfd_ = limit_online_user_ + 3;
    pr_users_ = new user[limit_sockfd_];
    DBG("limit_sockfd_: %d ", limit_sockfd_);

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
        if (listen(server_listen_fd_, limit_conn_waiting_num_) < 0) {
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

void server::heartbeat() {

}

void server::send_ack(const int ack, int client_fd) {
    if (send(client_fd, &ack, sizeof(ack), 0) < 0) {
        perror("send");
        exit(1);
    }
}

void server::do_verify_token() {
    verify_token_epollfd = epoll_create(1);
    const int &epollfd = verify_token_epollfd;

    // add the read_pipe1 to the epoll
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = read_pipe1;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, read_pipe1, &ev) < 0) {
        perror("eoill_ctr");
        exit(errno);
    }

    struct epoll_event events[limit_sockfd_];
    for(;;) {
        DBGIN(" epoll_wait");
        const int nfds = epoll_wait(epollfd, events, limit_sockfd_, -1);
        DBGOUT(" epoll_wait");
        if (nfds == -1) {
            perror("epoll_wait");
            exit(errno);
        }
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == read_pipe1) {
                DBGIN(" read_pipe1");
                int new_client_fd;
                while (read(read_pipe1, &new_client_fd,
                            sizeof(new_client_fd)) > 0) {
                    DBG("new_client_fd = %d", new_client_fd);
                    ev.data.fd = new_client_fd;
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD,
                                    new_client_fd, &ev) < 0) {
                        perror("epoll_ctr");
                        exit(errno);
                    }
                }
                DBGOUT(" read_pipe1");
            } else {
                DBGIN(" verify user");
                const int new_client_fd = events[i].data.fd;
                epoll_ctl(epollfd, EPOLL_CTL_DEL, new_client_fd, NULL);                

                // verify user with token_tmp
                const string token = config::get_config_ptr()->config_["token"];
                char token_tmp[token.size() + 1] = {};
                const int re = recv(new_client_fd, token_tmp, token.size(), 0);
                DBG("token: %s, token_tmp: %s len= %zu", token.c_str(), token_tmp, token.length());
                if (re <= 0) {
                    close(new_client_fd);
                    std::cout << "New sockfd ["<< new_client_fd << "] connection error!" << std::endl;
                    continue;
                }
                if (token_tmp != token) {
                    std::cout << "New sockfd ["<< new_client_fd << "] failed the verification!" << std::endl;
                    send_ack(0, new_client_fd);
                    close(new_client_fd);
                    continue;
                }
                DBG("else");
                if (write(write_pipe2, &new_client_fd, sizeof(new_client_fd)) < 0) {
                    send_ack(0, new_client_fd);
                    perror("write");
                    exit(1);
                }
                send_ack(1, new_client_fd);

                std::cout << "New sockfd ["<< new_client_fd << "] passed the verification!" << std::endl;
                
                DBGOUT(" verify user");
            }
        }
    }
}

// 1. Communicat with do_verify_token() by pipe.see: https://www.javaroad.cn/questions/47889
//    do_login() would add one client_sockfd to pipe once one client_sockfd is created.
void server::do_login() {
    while(1) {
        DBGIN(" do_login");
        struct sockaddr_in client_address;
        socklen_t len = sizeof(client_address);
        int new_client_fd = accept(server_listen_fd_, (struct sockaddr *)&client_address, &len);
        printf("New sockfd %d try to connect.\n", new_client_fd);

        if (new_client_fd < 0) {
            perror("accept");
            exit(errno);
        }
        if (new_client_fd >= limit_sockfd_) {
            printf("new_client_fd[%d] over limit_sockfd_[%d]\n", new_client_fd, limit_sockfd_);
            close(new_client_fd);
            continue;
        }
        max_sockfd_ = max(max_sockfd_, new_client_fd);
        // pr_users_[new_client_fd] = move(user(new_client_fd, client_address));
        pr_users_[new_client_fd].sockfd_ = new_client_fd;
        pr_users_[new_client_fd].addr_ = client_address;

        // 三个线程为什么可以做到同步？就是通过这种机制。三个线程do_login,verify_token,do_reactor取名为a,b,c
        // a,b,c中的sockfd几乎永远没有一个重叠的，因为他们之间的sockfd传递的逻辑是a->b->c，且只要传递之前，一定先从本地移除此sockfd。(当然线程a并没有移除sockfd，因为他根本没有监听此sockfd)
        if (write(write_pipe1, &new_client_fd, sizeof(new_client_fd)) < 0) {
            perror("write");
            exit(1);
        }
    }
}

void server::test() {
    for (int ii = 7, flag = 0; ii < 20; ii++ ) {
        if (pr_users_[ii].sockfd_ != -1) {
            flag = ii;
        }
        if (flag) {
            DBG("sockfd = %d", ii);
            pr_users_[ii].print();
        }
    }
}

bool server::read_message_head(const int client_fd) {
    DBGIN(" read_message_head");
    user &ref_user = pr_users_[client_fd];
    auto &message = ref_user.message_;
    auto &next_user = pr_users_[client_fd + 1];
    const int header_length = sizeof(message.header_);
    DBG("header_length=%d", header_length);
    int n_len = recv(client_fd, &message.header_ + message.header_index_,
                     header_length - message.header_index_, 0);
    if (n_len <= 0) {
        return false;
    }
    DBG("header_index_=%d, n_len = %d",message.header_index_, n_len);
    message.header_index_ += n_len;
    // DBG("sockfd[%d] message:header={{content_size_=%d, type_=%d}, index=%d}",
    //     client_fd,
    //     message.header_.content_size_, message.header_.type_, message.header_index_);
    if (message.header_index_ < sizeof(message.header_)) {
        DBG("still need read_message_head.");
    } else {
        DBG("change to read_message_content.");
        message.header_index_ = 0;
        message.check_state_ = message::CHECK_STATE_CONTENT;
    }
    return true;
}

bool server::read_message_content(const int client_fd) {
    DBGIN(" read_message_content");
    test();

    user &ref_user = pr_users_[client_fd];
    auto &message = ref_user.message_;

    const int content_size_ = message.header_.content_size_;
    if (message.content_index_ == 0) {
        if (content_size_ > limit_CONTENT_LENGTH) {
            const char *buff = "content_size_ over limit_CONTENT_LENGTH!Close the connection.";
            send(client_fd, buff, sizeof(buff), 0);
            message.reset_content();
            return false;
        }
        if (message.content_pr_) {
            delete(message.content_pr_);
        }
        message.content_pr_ = new char[content_size_ + 1];
        DBG("new message.content,and the length = %d", content_size_ + 1);
    }

    int n_len = recv(client_fd, message.content_pr_ + message.content_index_,
                    content_size_ - message.content_index_, 0);
    if (n_len <= 0) {
        message.reset_content();
        return false;
    }
    DBG("sock[%d] says: %s", client_fd, message.content_pr_ + message.content_index_);

    message.content_index_ += n_len;
    if (message.content_index_ < content_size_) {
        DBG("still need read_message_content.");
    } else {        
        DBG("change to read_message_head.");
        message.reset_content();
    }
    test();
    return true;
}

void server::do_reactor() {
    reactor_epollfd = epoll_create(1);
    const int &epollfd = reactor_epollfd;

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = read_pipe2;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, read_pipe2, &ev) < 0) {
        perror("eoill_ctr");
        exit(errno);
    }

    struct epoll_event events[limit_sockfd_];
    for (;;) {
        DBGIN(" epoll_wait");
        const int nfds = epoll_wait(epollfd, events, limit_sockfd_, -1);
        DBGOUT(" epoll_wait");
        if (nfds == -1) {
            perror("epoll_wait");
            exit(errno);
        }
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == read_pipe2) {
                DBGIN(" read_pipe2");
                int client_fd;
                while (read(read_pipe2, &client_fd,
                            sizeof(client_fd)) > 0) {
                    DBG("client_fd = %d", client_fd);
                    ev.data.fd = client_fd;
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD,
                                    client_fd, &ev) < 0) {
                        perror("epoll_ctr");
                        exit(errno);
                    }
                    const user &mid_user= pr_users_[client_fd];
                    DBG("sockfd[%d] ip[%s]", mid_user.sockfd_, inet_ntoa(mid_user.addr_.sin_addr));
                }
                DBGOUT(" read_pipe2");
            } else {
                DBGIN(" read message");
                const int client_fd = events[i].data.fd;
                const user &mid_user = pr_users_[client_fd];
                
                if (mid_user.message_.check_state_ == message::CHECK_STATE_HEADER) {
                    DBG("CHECK_STATE_HEADER");
                    if (read_message_head(client_fd) == false) {
                        printf("sockfd[%d] closed connection.\n", client_fd);

                        epoll_ctl(reactor_epollfd, EPOLL_CTL_DEL, client_fd, nullptr);
                        close(client_fd);
                    }
                } else if (mid_user.message_.check_state_ == message::CHECK_STATE_CONTENT) {
                    DBG("CHECK_STATE_CONTENT");
                    if (read_message_content(client_fd) == false) {
                        printf("sockfd[%d] closed connection.\n", client_fd);
                        epoll_ctl(reactor_epollfd, EPOLL_CTL_DEL, client_fd, nullptr);
                        close(client_fd);
                    }
                }

                DBGOUT(" read message_");
            }
        }
    }
}

void server::make_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        perror("fcntl");
        exit(errno);
    }
    if (fcntl(fd, F_SETFL, O_NONBLOCK | flags) != 0) {
        perror("fcntl");
        exit(errno);
    }
}

void server::init_pipe() {
    int pipefds1[2] = {};
    if (pipe(pipefds1) < 0) {
        perror("pipe");
        exit(1);
    }
    read_pipe1 = pipefds1[0];
    write_pipe1 = pipefds1[1];
    int pipefds2[2] = {};
    if (pipe(pipefds2) < 0) {
        perror("pipe");
        exit(1);
    }    read_pipe2 = pipefds2[0];
    write_pipe2 = pipefds2[1];   

    make_nonblock(read_pipe1);
    make_nonblock(write_pipe1);
    make_nonblock(read_pipe2);
    make_nonblock(write_pipe2);
}

void *server::call_back(void *arg) {
    server::call_back_arg_struc *p_node = (server::call_back_arg_struc*)arg;
    auto pr_obj = p_node->pr_obj;
    auto func = p_node->func;
    (pr_obj->*func)();
    delete p_node;
}

void server::start_listen_conn() {
    init_pipe();

    // main reactor
    call_back_arg_struc *login_tid_struc = new call_back_arg_struc{this, &server::do_login};

    // sub reactor
    call_back_arg_struc *verify_token_tid_struc = new call_back_arg_struc{this, &server::do_verify_token};
    call_back_arg_struc *reactor_struc = new call_back_arg_struc{this, &server::do_reactor};


    pthread_create(&login_tid, nullptr, call_back, login_tid_struc);

    pthread_create(&verify_token_tid, nullptr, call_back, verify_token_tid_struc);
    pthread_create(&reactor_tid, nullptr, call_back, reactor_struc);


    pthread_join(login_tid, nullptr);
    pthread_join(verify_token_tid, nullptr);
    pthread_join(reactor_tid, nullptr);
}