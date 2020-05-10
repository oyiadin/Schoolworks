#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <inttypes.h>

typedef struct {
    bool IS_FETCH_INFO:1;
    bool IS_FETCH_FILE:1;
    int _RESERVED:30;
    uint32_t REQ_PACK_ID;
} REQ_HEADER;


typedef struct {
    bool IS_INFO:1;
    bool IS_FILE:1;
    bool IS_ERROR:1;
    int _RESERVED:29;
    uint32_t RES_PACK_ID;

    uint32_t TOTAL;
    uint32_t PACKET_SIZE;
} RES_HEADER;


#define error_handler(...) (printf(__VA_ARGS__), exit(0))

#endif
