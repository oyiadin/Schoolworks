// 队满的条件：length >= MAXSIZE

#include <cstdlib>

// pre-definitions...

#define OVERFLOW -2
#define MAXSIZE 10
typedef int ElemType;
struct CircularQueue {
    ElemType base[MAXSIZE];
    int rear, length;
};


void init(CircularQueue *queue) {
    queue->rear = 0;
    queue->length = 0;
}


bool enQueue(CircularQueue *queue, ElemType elem) {
    if (queue->length >= MAXSIZE) return false;

    queue->base[queue->rear] = elem;
    queue->rear = (queue->rear + 1) % MAXSIZE;

    ++queue->length;

    return true;
}

ElemType deQueue(CircularQueue *queue) {
    if (queue->length <= 0) exit(OVERFLOW);

    ElemType ret = queue->base[(queue->rear - queue->length + MAXSIZE) % MAXSIZE];
    --queue->length;

    return ret;
}
