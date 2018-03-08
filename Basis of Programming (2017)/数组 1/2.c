// 数组逆序
#include <stdio.h>

int main() {
    int a[10], i=10;
    while (i--) scanf("%d", &a[i]);
    for (i=0; i<10; ++i) {
        printf("%d", a[i]);
        if (i==9) printf("\n");
        else printf(" ");
    }
    return 0;
}