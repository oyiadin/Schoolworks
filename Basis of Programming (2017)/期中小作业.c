#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main() {
    srand((unsigned)time(0));
    rand();
    printf("欢迎来到猜数字游戏，输入 -1 可以随时退出\n");

    int times, answer, guess;
    while (1) {
        times = 0;
        answer = rand()/100;
        do {
            if (times == 0) printf("我猜: ");
            else if (guess > answer) printf("太大啦! 再猜一遍: ");
            else printf("太小啦! 再猜一遍: ");

            scanf("%d", &guess);
            if (guess == -1) {
                printf("退出中…\n");
                exit(0);
            }
            ++times;
        } while (guess!=answer);
        printf("恭喜~ 你猜了%d次终于猜到数字%d啦!\n", times, answer);
    }
    return 0;
}
