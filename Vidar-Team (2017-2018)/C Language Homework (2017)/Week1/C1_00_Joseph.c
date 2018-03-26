#include <stdio.h>

struct Link {
    struct Link *prev, *next;
    int num;
};

struct Link *next_for(struct Link *at, int step) {
    while (step--) at = at->next;
    return at;
}

int kill_at(struct Link *at) {
    at->prev->next = at->next;
    at->next->prev = at->prev;
    return at->num;
}

int main() {
    int i, n, step;
    scanf("%d%d", &n, &step);

    struct Link link[n];

    for (i=0; i<n; ++i) {
        link[i].next = &link[(i + 1) % n];
        link[i].prev = &link[(i == 0) ? (n - 1) : (i - 1)];
        link[i].num = i+1;
    }

    struct Link *at = &link[0];
    int count = 1;
    while (at->next != at) {
        at = next_for(at, step);
        kill_at(at->prev);  // printf("%d: killed %d\n", count++, kill_at(at->prev));
    }
    printf("only the %dth one left now\n", at->num);
}