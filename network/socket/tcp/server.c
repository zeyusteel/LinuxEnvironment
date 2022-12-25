#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#define PORT 8888
#define LISTEN_NUM 100 
#define BUF_SIZE 2048

int main(int argc, char const *argv[])
{
    //用于监听的socket与连接用的socket
    int listenfd, connfd; 

    //<netinet/in.h> 服务器地址,客户端地址
    struct sockaddr_in servaddr, cliaddr;

    //子进程处理通信
    pid_t pid;

    if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error!\n");
        exit(1);
    }

    //初始化 servaddr
    bzero(&servaddr, sizeof(servaddr));
    
    servaddr.sin_family = PF_INET;
    servaddr.sin_port = htons(PORT); //主机字节序转换为网络字节序 #include <arpa/inet.h>
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //可以接受任意IP

    //关联地址和套接字
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind error!\n");
        exit(1);
    }

    //监听
    if (listen(listenfd, LISTEN_NUM) == -1) {
        perror("listen error!\n");
        exit(1);
    }

    //接受客户端请求
    while(1) {
        bzero(&cliaddr, sizeof(cliaddr));
        socklen_t clilen = sizeof(cliaddr);

        printf("start accept!\n");
        //接受请求
        if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) == -1) {
            perror("accept error!\n");
            exit(1);
        }

        if ((pid = fork()) == 0) {
            printf("child id:%d to handle mesg", (int)getpid());
            ssize_t len;
            close(listenfd);

            char buf[BUF_SIZE] = {0};
            while ((len = read(connfd, buf, BUF_SIZE)) > 0) {
                for (int i = 0; i < len; ++i) {
                    buf[i] = toupper(buf[i]);
                }
                write(connfd, buf, len);
            }
            close(connfd);
            exit(0);
        }
        close(connfd);
    }

    close(listenfd);
    return 0;
}