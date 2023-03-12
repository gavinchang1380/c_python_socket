// gcc client.c -o client && ./client
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5000
#define HOST "127.0.0.1"

int8_t send_buffer[1024 * 1024];
int8_t recv_buffer[1024 * 1024];

static void send_msg(int sockfd, void *buf, size_t len)
{
    int32_t len_n = htonl(len);
    send(sockfd, &len_n, sizeof(len_n), 0);
    send(sockfd, buf, len, 0);
}

static ssize_t recvall(int sockfd, void *buf, size_t len)
{
    int32_t received = 0;
    do {
        ssize_t ret = recv(sockfd, &((int8_t*)buf)[received], len - received, 0);
        if (ret == 0) {
            return 0;
        }
        if (ret == -1) {
            printf("recv socket error: %s(errno: %d)\n", strerror(errno), errno);
            exit(-1);
        }
        received += ret;
    } while (received < len);
    return received;
}

static ssize_t recv_msg(int sockfd, void *buf, size_t len)
{
    int32_t len_n;
    if (0 == recvall(sockfd, &len_n, sizeof(len_n))) {
        return 0;
    }
    len_n = ntohl(len_n);
    if (len_n > len) {
        printf("received buf size %d is larger than buf size %ld\n", len_n, sizeof(recv_buffer));
        exit(-1);
    }

    return recvall(sockfd, recv_buffer, len_n);
}

int main(int argc, char** argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    memcpy(&send_buffer[0], "abcd", 4);
    memcpy(&send_buffer[sizeof(send_buffer) - 4], "dcba", 4);

    // connect with the server
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, HOST, &servaddr.sin_addr) <= 0){
        printf("inet_pton error for %s\n", argv[1]);
        return -1;
    }
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }

    while (1) {
        getchar();

        // send buffer
        send_msg(sockfd, send_buffer, sizeof(send_buffer));
        printf("send to server %ld bytes: [%c,%c,%c,%c,...%c,%c,%c,%c]\n", sizeof(send_buffer),
                    send_buffer[0], send_buffer[1], send_buffer[2], send_buffer[3],
                    send_buffer[sizeof(send_buffer) - 4], send_buffer[sizeof(send_buffer) - 3],
                    send_buffer[sizeof(send_buffer) - 2], send_buffer[sizeof(send_buffer) - 1]);

        // receive buffer
        memset(recv_buffer, 0x00, sizeof(recv_buffer));
        ssize_t ret = recv_msg(sockfd, recv_buffer, sizeof(recv_buffer));
        if (!ret) {
            return 0;
        }
        printf("receive from server %ld bytes: [%c,%c,%c,%c,...%c,%c,%c,%c]\n", sizeof(recv_buffer),
                    recv_buffer[0], recv_buffer[1], recv_buffer[2], recv_buffer[3],
                    recv_buffer[sizeof(recv_buffer) - 4], recv_buffer[sizeof(recv_buffer) - 3],
                    recv_buffer[sizeof(recv_buffer) - 2], recv_buffer[sizeof(recv_buffer) - 1]);
    }
    close(sockfd);
    return 0;
}
