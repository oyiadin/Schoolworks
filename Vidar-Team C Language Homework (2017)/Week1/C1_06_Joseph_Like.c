#include <stdio.h>

int main() {
    int i, n = 1000;
    bool seized[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int at = 0, step = 1;

    while (n--) {
        seized[at] = 1;
        at = (at+step+1)%10;
        step = (step+1)%10;
    }
    for (i=0; i<10; ++i) if (!seized[i])
        printf("%d\n", i+1);
}
