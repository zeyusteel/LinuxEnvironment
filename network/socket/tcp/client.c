#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PORT 8888
#define BUF_MAX 2048

int main(int argc, char const *argv[])
{
    //用于通信的套接字与服务端地址
    int sockfd;    
	struct sockaddr_in servaddr;

    //创建套接字
	if ((sockfd = socket(AF_INET , SOCK_STREAM , 0)) == -1) {
		perror("socket error");
		exit(1);
	}

    //设置地址结构
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

	if (inet_pton(AF_INET , "127.0.0.1" , &servaddr.sin_addr) < 0) {
		perror("inet_pton error!\n");
		exit(1);
	}

    //请求连接
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("connect error!\n");
        exit(1);
    }

    //消息处理
    char send[BUF_MAX] = {0}, receive[BUF_MAX] = {0};
    while (fgets(send, BUF_MAX, stdin) != NULL) {
        write(sockfd, send, strlen(send));

        bzero(receive, sizeof(receive));
        if (read(sockfd, receive, BUF_MAX) < 0) {
            perror("read error!\n");
            exit(1);
        }

        if (fputs(receive, stdout) == EOF) {
            perror("fputs error!\n");
            exit(1);
        }
        bzero(send, sizeof(send));
    }

    close(sockfd);
    return 0;
}