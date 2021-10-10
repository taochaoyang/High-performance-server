/*************************************************************************
	> File Name: head.h
	> Author: tcy
	> Mail: 1281042827@qq.com
	> Created Time: Sat 20 Mar 2021 07:22:19 PM CST
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>

#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>

#include <sys/wait.h>

#include <sys/ipc.h>
#include <sys/shm.h>


#include <sys/sem.h>

#include <sys/msg.h>

#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/epoll.h>
#include <string>
#include <iostream>

// For canceling the color.
#define NONE "\033[m"

// For serious warning.
#define RED "\033[0;32;31m" 

// For healthy logging.
#define GREEN "\033[0;32;32m"

// For warning.
#define YELLOW "\033[1;33m"

// For debug. 
#define BLUE "\033[0;32;34m"

#define CYAN "\033[0;31m"
#ifndef DBG
    #define DBG(fmt, args...) \
    printf(BLUE "FileAndLine: %s:%d, Function: %s : " NONE YELLOW fmt NONE "\n", __FILE__, __LINE__ , __FUNCTION__,##args);
    #define DBGIN(fmt, args...) \
    printf(RED "FileAndLine: %s:%d, Function: %s : " NONE GREEN fmt NONE "\n", __FILE__, __LINE__ , __FUNCTION__,##args);
    #define DBGOUT(fmt, args...) \
    printf(BLUE "FileAndLine: %s:%d, Function: %s : " NONE CYAN fmt NONE "\n", __FILE__, __LINE__ , __FUNCTION__,##args);
    #define CUT(fmt, args...) \
    printf(RED "FileAndLine: %s:%d, Function: %s : -------------------------------------------------------" NONE fmt NONE "\n", __FILE__, __LINE__ , __FUNCTION__,##args);	
#endif


using namespace std;






struct message_header {
    enum MESSAGE_HEADER_TYPE{TP_MESSAGE=0x01, TP_ACK=0x02, TP_FIN=0x04, TP_HEARTBEAT=0x08,
                            TP_FAILED=0x10, TP_ERROR=0x20, TP_SUCCESS=0x40, TP_VERIFY=0x80};
    uint32_t content_size_;
    uint32_t type_;
};


int sockfd;


int socket_connect(const char *ip, const int port) {
    int sockfd;
    struct sockaddr_in server;
    server.sin_family = AF_INET;//ipv4
    server.sin_port = htons(port);//端口号
    server.sin_addr.s_addr = inet_addr(ip);//ip地址

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {//ipv4，套接字数据类型，0（因为前两个参数已经足够确认是tcp连接了）
        perror("socket()");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connet");
        exit(1);
    }
    return sockfd;
}

void test() {
    string  s;
    cin>>s;
    send(sockfd, s.c_str(), s.size(), 0);

}

void send_message(const int client_fd, const message_header::MESSAGE_HEADER_TYPE type = message_header::TP_ACK, const int content_size = 1, const char *pr_content = "\0") {
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

void send_message(const int client_fd, const char *pr_content = "\0") {
    send_message(client_fd, message_header::TP_ACK, strlen(pr_content), pr_content);
}

void real() {
    send_message(sockfd, "666");

    while(1) {        
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd ,&readfds);
        FD_SET(0 ,&readfds);
        printf("in select.\n");
        select(sockfd + 1, &readfds, nullptr, nullptr, nullptr);

        char buffer[1024] = {"none"};
        if (FD_ISSET(0, &readfds)) {
            scanf("%[^\n]s", buffer);
            getchar();
            send_message(sockfd, buffer);
        }

        if (FD_ISSET(sockfd, &readfds)) {
            int len = 0;
            message_header header;

            if ((len = recv(sockfd, &header, sizeof(header), 0)) <= 0) {
                printf("close111");
                close(sockfd);
                return;
            }                
            printf("recv=> header={%d,%d},\n", header.content_size_, header.type_);

            if (header.content_size_ > 0) {
                if ((len = recv(sockfd, &buffer, header.content_size_, 0)) <= 0) {                
                    printf("close22222");

                    close(sockfd);
                    return;
                }
            }

            printf("recv=> buffer:{%s}\n\n", buffer);

            if (header.type_ == message_header::TP_FAILED) {                
                printf("close3333");
                close(sockfd);
                return;
            }
        }
    }
}

int main(int argc,char **argv) {
    printf("wefeewf\n");

    if (argc != 3) {
        fprintf(stderr, "Usage : %s ip prot!\n", argv[0]);
        exit(1);
    }

    if ((sockfd = socket_connect(argv[1], atoi(argv[2]))) < 0) {
        perror("socket_connect");
        exit(1);
    }

    real();


	return 0;
}
