
#include "server.h"
#include "config.h"

#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/epoll.h>
#include <fcntl.h>
#include <signal.h>
#include <set>
#include <sys/select.h>
#include <vector>

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

server::~server() {
    // for (int i = 0; i < max_sockfd_; i++) {
    //     // No matter is or not sockfd.
    //     close_sockfd(i);
    //     printf("close sockfd[%d]", i);
    // }
}

void server::thread_poll_work() {
    
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

void server::work_signal_emitter() {
    struct itimerval itimer;
    itimer.it_interval.tv_sec = 1;
    itimer.it_interval.tv_usec = 0;
    itimer.it_value.tv_sec = 1;
    itimer.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itimer, NULL);
    int signum;
    while(1) {
        if (sigwait(&signal_mask_, &signum) != 0) {
            perror("sigwait");
            exit(errno);
        }
        DBG("Receive one signal: %d", signum);
        write(write_pipe3_1, &signum, sizeof(signum));
        write(write_pipe3_2, &signum, sizeof(signum));
        write(write_pipe3_3, &signum, sizeof(signum));

        if (signum == SIGINT || signum == SIGQUIT) {
            DBG("signum = %d", signum);
            DBG("write_pipe3_* make me return");
            return;
        }
    }
}

void server::send_message(const int client_fd, const message_header::MESSAGE_HEADER_TYPE type, const int content_size, const char *pr_content) {
    message_header mid_message_header;
    mid_message_header.type_ = type;
    mid_message_header.content_size_ = content_size;

    if (send(client_fd, &mid_message_header, sizeof(mid_message_header), 0) < 0) {
        perror("send");
        exit(1);
    }
    if (send(client_fd, pr_content, content_size, 0) < 0) {
        perror("send");
        exit(1);
    }
}

void server::send_message(const int client_fd, const char *pr_content) {
    send_message(client_fd, message_header::TP_ACK, strlen(pr_content), pr_content);
}

void server::close_sockfd(const int client_fd, const message_header::MESSAGE_HEADER_TYPE type, const int content_size, const char *pr_content) {
    send_message(client_fd, type, content_size, pr_content);
    printf("close %d\n", client_fd);
    close(client_fd);
}

void server::work_verify_token() {
    const int &epollfd = verify_token_epollfd;
    set<int> ev_sockfds;
    struct epoll_event ev, events[limit_sockfd_];
    ev.events = EPOLLIN;
    for(;;) {
        DBGIN(" epoll_wait");
        const int nfds = epoll_wait(epollfd, events, limit_sockfd_, -1);
        DBGOUT(" epoll_wait");
        if (nfds == -1) {
            perror("epoll_wait");
            exit(errno);
        }

        bool read_pipe3_3_is_ready = false;
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
                    ev_sockfds.insert(new_client_fd);
                    pr_users_[new_client_fd].is_validated_ = false;
                }
                DBGOUT(" read_pipe1");
            } else if (events[i].data.fd == read_pipe3_3) {
                DBGIN(" read_pipe3_3");
                read_pipe3_3_is_ready = true;
            } else {
                DBGIN(" verify user");
                const int new_client_fd = events[i].data.fd;

                const user &mid_user = pr_users_[new_client_fd];
                
                if (mid_user.message_.check_state_ == message::CHECK_STATE_HEADER) {
                    DBG("CHECK_STATE_HEADER");
                    if (read_message_head(new_client_fd) == false) {
                        printf("sockfd[%d] closed connection.\n", new_client_fd);
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, new_client_fd, nullptr);
                        ev_sockfds.erase(new_client_fd);
                        close_sockfd(new_client_fd, message_header::TP_ERROR, strlen("CHECK_STATE_HEADER"), "CHECK_STATE_HEADER");
                    }
                } else if (mid_user.message_.check_state_ == message::CHECK_STATE_CONTENT) {

                    // read_message_content.
                    DBG("CHECK_STATE_CONTENT");
                    char *pr_content = nullptr;
                    int content_length = 0;
                    if (read_message_content(new_client_fd, pr_content, content_length) == false) {
                        printf("sockfd[%d] verified failed,closed connection.\n", new_client_fd);
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, new_client_fd, nullptr);
                        ev_sockfds.erase(new_client_fd);
                        close_sockfd(new_client_fd, message_header::TP_FAILED, strlen("verified failed"), "verified failed");
                    }
                    if (pr_content == nullptr || content_length == 0) {
                        continue;
                    }

                    // read_message_content completely.
                    DBG("content = %s, length = %d", pr_content, content_length);
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, new_client_fd, NULL);                
                    ev_sockfds.erase(new_client_fd);

                    // Verify user by comparing pr_content and token.
                    const string token = config::get_config_ptr()->config_["token"];
                    if (content_length <= 0) {
                        close_sockfd(new_client_fd, message_header::TP_ERROR);
                        std::cout << "New sockfd ["<< new_client_fd << "] connection error!" << std::endl;
                        continue;
                    }
                    if (pr_content != token) {
                        std::cout << "New sockfd ["<< new_client_fd << "] failed the verification!" << std::endl;
                        close_sockfd(new_client_fd, message_header::TP_FAILED, strlen("token not passed") ,"token not passed");
                        continue;
                    }

                    // verify pass
                    send_message(new_client_fd, message_header::TP_SUCCESS);
                    std::cout << "New sockfd ["<< new_client_fd << "] passed the verification!" << std::endl;
                    pr_users_[new_client_fd].is_validated_ = true;
                    pr_users_[new_client_fd].heartbeat_count_ = user::LIMIT_HEARTBEAT_COUNT_;                    
                    if (write(write_pipe2, &new_client_fd, sizeof(new_client_fd)) < 0) {
                        send_message(new_client_fd, message_header::TP_ERROR);
                        perror("write");
                        exit(1);
                    }
                    DBGOUT(" verify user");
                }
            }
        }
        if (read_pipe3_3_is_ready) {
            DBGIN(" read_pipe3_3");
            int signum;
            while (read(read_pipe3_3, &signum, sizeof(signum)) > 0) {
                DBG("signum = %d", signum);
                switch(signum) {
                    case SIGINT:
                    case SIGQUIT:
                        for (int sockfd : ev_sockfds) {              
                            close_sockfd(sockfd, message_header::TP_FIN, strlen("heartbeat"), "heartbeat");
                        }
                        DBG("write_pipe3_* make me return");
                        return;
                    case SIGALRM:
                        vector<int> deleted_sockfd;
                        for (int sockfd : ev_sockfds) {
                            if (pr_users_[sockfd].heartbeat_count_ <= 0) {
                                epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, NULL);                
                                deleted_sockfd.push_back(sockfd);
                                close_sockfd(sockfd, message_header::TP_FIN, strlen("heartbeat"), "heartbeat");
                                continue;
                            }
                            pr_users_[sockfd].heartbeat_count_--;
                            DBG("pr_users_[%d].heartbeat_count_ = %d", sockfd, pr_users_[sockfd].heartbeat_count_);
                            send_message(sockfd, message_header::TP_HEARTBEAT);
                        }
                        for (int sockfd : deleted_sockfd) {
                            ev_sockfds.erase(sockfd);
                        }
                        break;
                }
            }
            DBGOUT(" read_pipe3_3");
        }
    }
}

// 1. Communicat with work_verify_token() by pipe.see: https://www.javaroad.cn/questions/47889
//    work_accept() would add one client_sockfd to pipe once one client_sockfd is created.
void server::work_accept() {
    while(1) {
        DBGIN(" work_accept");
        struct sockaddr_in client_address;
        socklen_t len = sizeof(client_address);

        // make server_listen_fd_ nonblock to avoid blocking because we decide to use select.
        make_nonblock(server_listen_fd_);
        while(1) {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(server_listen_fd_ ,&readfds);
            FD_SET(read_pipe3_1, &readfds);

            int maxfd = max(server_listen_fd_, read_pipe3_1);
            if (select(maxfd + 1, &readfds, nullptr, nullptr, nullptr) == -1) {
                perror("select");
                exit(errno);
            }

            if (FD_ISSET(server_listen_fd_, &readfds)) {
                int new_client_fd = accept(server_listen_fd_, (struct sockaddr *)&client_address, &len);
                printf("New sockfd %d try to connect.\n", new_client_fd);

                if (new_client_fd < 0) {
                    perror("accept");
                    exit(errno);
                }
                if (new_client_fd >= limit_sockfd_) {
                    printf("new_client_fd[%d] over limit_sockfd_[%d]\n", new_client_fd, limit_sockfd_);
                    close_sockfd(new_client_fd, message_header::TP_ERROR);
                    continue;
                }
                max_sockfd_ = max(max_sockfd_, new_client_fd);
                pr_users_[new_client_fd] = move(user(new_client_fd, client_address));

                // 三个线程为什么可以做到同步读写pr_users 中的数据？就是通过下面这个设计。
                // 三个线程work_accept,verify_token,work_reactor取名为a,b,c
                // a,b,c中的sockfd永远没有一个重叠的，因为他们之间通过管道传递sockfd，即a->b->c，且只要传递之前，一定先从本地移除此sockfd。(当然线程a并没有移除sockfd，因为他并没有监听sockfd)
                // 如此就很显然的，每个sockfd同一时刻只会被一个线程服务。而 pr_users 是以sockfd为下标的，因此这种模式下的 pr_users 的读写 也是线程安全的
                if (write(write_pipe1, &new_client_fd, sizeof(new_client_fd)) < 0) {
                    send_message(new_client_fd, message_header::TP_ERROR);
                    perror("write");
                    exit(1);
                }
            }
            if (FD_ISSET(read_pipe3_1, &readfds)){
                int signum;
                while (read(read_pipe3_1, &signum, sizeof(signum)) > 0) {
                    DBG("signum = %d", signum);
                    switch(signum) {
                        case SIGINT:
                            DBG("write_pipe3_* make me return");
                            return;
                        case SIGQUIT:
                            DBG("write_pipe3_* make me return");
                            return;
                        case SIGALRM:
                            break;
                    }
                }
            }
        }
    }
}

void server::test() {
    return;
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

void server::pthread_work1(const int client_fd, const char *pr_content) {
    DBG("in nbnb");
    printf("nbnb: %d, %s", client_fd, pr_content);
    send_message(client_fd, pr_content);
    delete(pr_content);
}

bool server::read_message_head(const int client_fd) {
    DBGIN("read_message_head sockfd[%d]", client_fd);
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

    ref_user.heartbeat_count_ = user::LIMIT_HEARTBEAT_COUNT_;
    DBG("header_index_=%d, n_len = %d",message.header_index_, n_len);
    message.header_index_ += n_len;
    message.print();
    if (message.header_index_ < sizeof(message.header_)) {
        DBG("still need read_message_head.");
    } else {
        DBG("change to read_message_content.");
        message.header_index_ = 0;
        message.check_state_ = message::CHECK_STATE_CONTENT;
    }
    return true;
}

bool server::read_message_content(const int client_fd, char *&pr_content, int &content_length) {
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

    ref_user.heartbeat_count_ = user::LIMIT_HEARTBEAT_COUNT_;
    printf("sock[%d] says: %s", client_fd, message.content_pr_ + message.content_index_);
    message.content_index_ += n_len;
    message.print();
    if (message.content_index_ < content_size_) {
        DBG("still need read_message_content.");
    } else {        
        DBG("change to read_message_head.");
        pr_content = message.content_pr_;
        content_length = message.content_index_;
        message.content_pr_ = nullptr;
        message.reset_content();
    }
    test();
    return true;
}

void server::work_reactor() {
    const int &epollfd = reactor_epollfd;
    set<int> ev_sockfds;
    struct epoll_event ev, events[limit_sockfd_];
    ev.events = EPOLLIN;
    for (;;) {
        DBGIN(" epoll_wait");
        const int nfds = epoll_wait(epollfd, events, limit_sockfd_, -1);
        DBGOUT(" epoll_wait");
        if (nfds == -1) {
            perror("epoll_wait");
            exit(errno);
        }

        bool read_pipe3_2_is_ready = false;
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
                    ev_sockfds.insert(client_fd);
                    DBG("66666666666666666666666666666666666666666666666666666666");
                    const user &mid_user= pr_users_[client_fd];
                    DBG("sockfd[%d] ip[%s]", mid_user.sockfd_, inet_ntoa(mid_user.addr_.sin_addr));
                }
                DBGOUT(" read_pipe2");
            } else if (events[i].data.fd == read_pipe3_2) {
                read_pipe3_2_is_ready = true;
            } else {
                DBGIN(" read message");
                const int client_fd = events[i].data.fd;
                user &mid_user = pr_users_[client_fd];
                
                mid_user.heartbeat_count_ = user::LIMIT_HEARTBEAT_COUNT_;
                printf("hea=%d\n", mid_user.heartbeat_count_);

                if (mid_user.message_.check_state_ == message::CHECK_STATE_HEADER) {
                    DBG("CHECK_STATE_HEADER");
                    if (read_message_head(client_fd) == false) {
                        printf("sockfd[%d] closed connection.\n", client_fd);
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, client_fd, nullptr);
                        ev_sockfds.erase(client_fd);
                        close_sockfd(client_fd, message_header::TP_ERROR);
                    }
                } else if (mid_user.message_.check_state_ == message::CHECK_STATE_CONTENT) {
                    DBG("CHECK_STATE_CONTENT");
                    char *pr_content = nullptr;
                    int content_length = 0;
                    if (read_message_content(client_fd, pr_content, content_length) == false) {
                        printf("sockfd[%d] closed connection.\n", client_fd);
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, client_fd, nullptr);
                        ev_sockfds.erase(client_fd);
                        close_sockfd(client_fd, message_header::TP_ERROR);
                    }
                    if (pr_content == nullptr || content_length == 0) {
                        continue;
                    }

                    DBG("content = %s, lenth = %d", pr_content, content_length);
                    type_threadpoll_request request{this, &server::pthread_work1, client_fd, pr_content};
                    
                    threadpool_.append(std::move(request));
                }
                DBGOUT(" read message_");
            }
        }
        if (read_pipe3_2_is_ready) {
            DBGIN(" read_pipe3_2");
            int signum;
            while (read(read_pipe3_2, &signum, sizeof(signum)) > 0) {
                DBG("signum = %d", signum);
                switch(signum) {
                    case SIGINT:
                    case SIGQUIT:
                        for (int sockfd : ev_sockfds) {              
                            close_sockfd(sockfd, message_header::TP_FIN, strlen("heartbeat"), "heartbeat");
                        }
                        DBG("write_pipe3_* make me return");
                        return;
                    case SIGALRM:
                        vector<int> deleted_sockfd;
                        for (int sockfd : ev_sockfds) {
                            if (pr_users_[sockfd].heartbeat_count_ <= 0) {
                                epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, NULL);
                                deleted_sockfd.push_back(sockfd);          
                                close_sockfd(sockfd, message_header::TP_FIN);
                                DBG("sockfd[%d] byebye!", sockfd);
                                continue;
                            }
                            pr_users_[sockfd].heartbeat_count_--;
                            printf("pr_users_[%d].heartbeat_count_ = %d", sockfd, pr_users_[sockfd].heartbeat_count_);
                            send_message(sockfd, message_header::TP_HEARTBEAT);
                        }
                        for (int sockfd : deleted_sockfd) {
                            ev_sockfds.erase(sockfd);
                        }
                        break;
                }
            }
            DBGOUT(" read_pipe3_2");
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

void server::init_signal() {
    sigemptyset(&signal_mask_);
    sigaddset(&signal_mask_ , SIGINT);
    sigaddset(&signal_mask_ , SIGQUIT);
    sigaddset(&signal_mask_ , SIGALRM);

    if(pthread_sigmask(SIG_BLOCK, &signal_mask_ , nullptr) != 0) {
        perror("error in pthread_sigmask");
        exit(errno);
    }
}


void server::init_pipe_and_epoll() {

    // pipefds1: bridge of communication from login_tid to verify_token_tid.
    // pipefds2: bridge of communication from verify_token_tid to reactor_tid.
    // pipefds3_*: bridge of communication from reactor_tid to other thress threads.
    int pipefds1[2] = {};
    int pipefds2[2] = {};
    int pipefds3_1[2] = {};
    int pipefds3_2[2] = {};
    int pipefds3_3[2] = {};
    if (pipe(pipefds1) < 0 || pipe(pipefds2) < 0 || pipe(pipefds3_1) < 0 || pipe(pipefds3_2) < 0 || pipe(pipefds3_3)) {
        perror("pipe");
        exit(1);
    }
    read_pipe1 = pipefds1[0];
    write_pipe1 = pipefds1[1]; 
    read_pipe2 = pipefds2[0];
    write_pipe2 = pipefds2[1];
    read_pipe3_1 = pipefds3_1[0];
    write_pipe3_1 = pipefds3_1[1]; 
    read_pipe3_2 = pipefds3_2[0];
    write_pipe3_2 = pipefds3_2[1];
    read_pipe3_3 = pipefds3_3[0];
    write_pipe3_3 = pipefds3_3[1];
    make_nonblock(read_pipe1);
    make_nonblock(write_pipe1);
    make_nonblock(read_pipe2);
    make_nonblock(write_pipe2);
    make_nonblock(read_pipe3_1);
    make_nonblock(write_pipe3_1);
    make_nonblock(read_pipe3_2);
    make_nonblock(write_pipe3_2);
    make_nonblock(read_pipe3_3);
    make_nonblock(write_pipe3_3);

    struct epoll_event ev;

    verify_token_epollfd = epoll_create(1);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = read_pipe1;
    if (epoll_ctl(verify_token_epollfd, EPOLL_CTL_ADD, read_pipe1, &ev) < 0) {
        perror("eoill_ctr");
        exit(errno);
    }
    ev.data.fd = read_pipe3_3;
    if (epoll_ctl(verify_token_epollfd, EPOLL_CTL_ADD, read_pipe3_3, &ev) < 0) {
        perror("eoill_ctr");
        exit(errno);
    }

    reactor_epollfd = epoll_create(1);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = read_pipe2;
    if (epoll_ctl(reactor_epollfd, EPOLL_CTL_ADD, read_pipe2, &ev) < 0) {
        perror("eoill_ctr");
        exit(errno);
    }
    ev.data.fd = read_pipe3_2;
    if (epoll_ctl(reactor_epollfd, EPOLL_CTL_ADD, read_pipe3_2, &ev) < 0) {
        perror("eoill_ctr");
        exit(errno);
    }

}

void server::run() {
    pthread_t tid = pthread_self();
    DBG("tid = %ld", tid);

    init_signal();
    init_pipe_and_epoll();

    // threadpool_.init(); must behind init_signal();
    threadpool_.init();

	using type_func = void (server::*)(void);
    using type_call_back = template_call_back_without_arg<server *, type_func>;
    type_call_back *login_tid_struc = new type_call_back{this, &server::work_accept};
    type_call_back *verify_token_tid_struc = new type_call_back{this, &server::work_verify_token};
    type_call_back *reactor_struc = new type_call_back{this, &server::work_reactor};
    type_call_back *signal_handler_struc = new type_call_back{this, &server::work_signal_emitter};

    pthread_create(&login_tid, nullptr, type_call_back::call_back, login_tid_struc);
    pthread_create(&verify_token_tid, nullptr, type_call_back::call_back, verify_token_tid_struc);
    pthread_create(&reactor_tid, nullptr, type_call_back::call_back, reactor_struc);
    pthread_create(&signal_handler_tid, nullptr, type_call_back::call_back, signal_handler_struc);

    pthread_join(login_tid, nullptr);
    pthread_join(verify_token_tid, nullptr);
    pthread_join(reactor_tid, nullptr);
    pthread_join(signal_handler_tid, nullptr);
}