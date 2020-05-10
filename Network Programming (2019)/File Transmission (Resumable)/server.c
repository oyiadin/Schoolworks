#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>

#include "common.h"


#define RESPONSE_ERROR \
    do { \
        res->IS_INFO = 0; \
        res->IS_FILE = 0; \
        res->IS_ERROR = 1; \
        res->PACKET_SIZE = 0; \
        write(fd, res_buf, sizeof(RES_HEADER)); \
    } while (0)


int main(int argc, char const *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);

    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_size = sizeof(clnt_addr);

    if (argc < 3)
        error_handler("usage: %s port filename\n", argv[0]);

    FILE *file_fd = fopen(argv[2], "rb");
    if (file_fd == NULL)
        error_handler("fopen() error, perhaps the file doesn't exist\n");
    fseek(file_fd, 0L, SEEK_END);
    uint32_t total_length = (uint32_t) ftell(file_fd);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handler("socket() error\n");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
        error_handler("bind() error, try to change the listening port\n");

    if (listen(sock, 5) == -1)
        error_handler("listen() error\n");

    puts("listening...");

    uint8_t req_buf[sizeof(REQ_HEADER)] = {0};
    uint8_t res_buf[sizeof(RES_HEADER) + 1024] = {0};
    uint8_t *res_content = res_buf + sizeof(RES_HEADER);

    REQ_HEADER *req = (REQ_HEADER *) req_buf;
    RES_HEADER *res = (RES_HEADER *) res_buf;

    res->TOTAL = total_length;

    int fd;
    while ((fd = accept(sock, (struct sockaddr *) &clnt_addr, &clnt_size))) {
        if (fd == -1)
            error_handler("accept() error\n");

        printf("connection established, from %s:%d\n",
            inet_ntoa(clnt_addr.sin_addr),
            ntohs(((struct sockaddr_in *) &clnt_addr)->sin_port));

        while (read(fd, (void *) req_buf, sizeof(REQ_HEADER)) != -1) {
            if (req->IS_FETCH_INFO && req->IS_FETCH_FILE) {
                RESPONSE_ERROR;
                continue;
            
            } else {
                if (req->IS_FETCH_INFO) {
                    puts("[!] fetch info");
                    res->IS_INFO = 1;
                    res->IS_FILE = 0;
                    res->IS_ERROR = 0;
                    res->PACKET_SIZE = (uint32_t) strlen(argv[2]) + 1;
                    write(fd, res_buf, sizeof(RES_HEADER));

                    char *filename = strrchr(argv[2], '/') + 1;
                    strncpy((char *) res_content, filename, res->PACKET_SIZE);
                    write(fd, (void *) res_content, res->PACKET_SIZE);

                } else if (req->IS_FETCH_FILE) {
                    printf("\r[!] fetch file, id=%d", req->REQ_PACK_ID);
                    if (req->REQ_PACK_ID > total_length) {
                        RESPONSE_ERROR;
                        continue;
                    }

                    fseek(file_fd, (long) req->REQ_PACK_ID, SEEK_SET);
                    size_t len = fread(res_content, sizeof(uint8_t), 1024, file_fd);

                    res->IS_INFO = 0;
                    res->IS_FILE = 1;
                    res->IS_ERROR = 0;
                    res->RES_PACK_ID = req->REQ_PACK_ID;
                    res->PACKET_SIZE = (uint32_t) len;
                    write(fd, (void *) res_buf, sizeof(RES_HEADER));
                    write(fd, (void *) res_content, res->PACKET_SIZE);
                }
            }
        }

    }


    return 0;
}