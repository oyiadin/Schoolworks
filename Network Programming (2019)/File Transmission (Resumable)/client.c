#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <signal.h> // signal functions

#include "common.h"


volatile sig_atomic_t flag = 0;

static void interrupt_handler(int sig) {
    flag = 1; // set flag
}


char filename[1024] = {0};

int main(int argc, char const *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);

    signal(SIGINT, interrupt_handler);  // Register signals
    struct sockaddr_in serv_addr;

    if (argc < 3)
        error_handler("usage: %s host port\n", argv[0]);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handler("socket() error\n");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
        error_handler("connect() error\n");

    REQ_HEADER req = {0};
    uint8_t res_buf[sizeof(RES_HEADER) + 1024] = {0};
    RES_HEADER *res = (RES_HEADER *) res_buf;
    uint8_t *res_content = (uint8_t *) res_buf + sizeof(RES_HEADER);

    req.IS_FETCH_INFO = 1;
    req.IS_FETCH_FILE = 0;

    write(sock, (void *) &req, sizeof(REQ_HEADER));

    FILE *fd = 0;
    uint32_t total = 0;

    while (!flag && read(sock, (void *) res, sizeof(RES_HEADER)) != -1) {
        if (res->IS_ERROR) {
            error_handler("error occurred!");
        }

        if (res->IS_INFO) {
            read(sock, (void *) res_content, res->PACKET_SIZE);
            strncpy(filename, (void *) res_content, res->PACKET_SIZE);

            printf("filename: %s\n", filename);

            fd = fopen(filename, "ab");
            total = res->TOTAL;
            break;
        }
    }

    fseek(fd, 0L, SEEK_END);
    uint32_t furthest_pack = (uint32_t) ftell(fd);

    if (furthest_pack != 0) {
        puts("[!] recovering the latest state of the file...");
        printf("[!] %d bytes saved in the last time\n", furthest_pack);
    }

    while (!flag && furthest_pack < total) {
        req.IS_FETCH_FILE = 1;
        req.IS_FETCH_INFO = 0;
        req.REQ_PACK_ID = furthest_pack;
        write(sock, (void *) &req, sizeof(REQ_HEADER));

        if (!flag && read(sock, (void *) res, sizeof(RES_HEADER)) != -1) {
            if (res->IS_ERROR) {
                error_handler("error occurred!");
            }

            if (res->IS_FILE) {
                printf("\r[*] file part received: PACK_ID=%d, PACK_SIZE=%d",
                       res->RES_PACK_ID, res->PACKET_SIZE);
                read(sock, (void *) res_content, res->PACKET_SIZE);

                if (res->RES_PACK_ID != furthest_pack) {
                    printf("\x0d[!] received pack_id %d != furthest_pack_id %d, ignored",
                           res->RES_PACK_ID, furthest_pack);
                    continue;
                }

                fwrite((void *) res_content, sizeof(uint8_t), res->PACKET_SIZE, fd);

                furthest_pack += res->PACKET_SIZE;
            }
        }
    }

    if (flag) {
        close(sock);
        fclose(fd);
        shutdown(sock, SHUT_RDWR);
        puts("interrupt");
        return -1;
    } else {
        fclose(fd);
        printf("\n[*] finished, saved to %s\n", filename);
    }

    return 0;
}