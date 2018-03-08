// 电影票售票程序
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool seats[5][5];

void show_seats(void) {
    int x, y;
    for (x=0; x<5; ++x) {
        for (y=0; y<5; ++y) printf("%d ", seats[x][y]);
        printf("\n");
    }
    printf("\n");
}

void show_status(void) {
    int x, y, sold=0;
    for (x=0; x<5; ++x)
        for (y=0; y<5; ++y)
            if (seats[x][y])
                ++sold;
    show_seats();
    printf("共有座位 25 个，已售 %d，售票率为 %lf\n\n", sold, sold/25.0);
}

int main() {
    int x, y, cmd=1;
    for (x=0; x<5; ++x) for (y=0; y<5; ++y) seats[x][y] = false;
    while (cmd) {
        printf("1: 售票；2: 显示售票率；0: 退出程序\n"
            "请输入一个数字: ");
        scanf("%d", &cmd);
        system("cls");

        if (cmd == 1) {
            show_seats();
            printf("请输入座位，左上为(1, 1): ");
            scanf("%d%d", &x, &y);
            system("cls");
            bool sold = seats[x-1][y-1];
            if (sold) {
                printf("该座位已售，请另选其他座位！\n\n");
                continue;
            } else seats[x-1][y-1] = true;
            show_seats();
        } else if (cmd == 2) {
            show_status();
        } else printf("退出中…\n");
    }
    return 0;
}