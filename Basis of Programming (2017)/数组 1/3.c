// 求一个3*3矩阵对角线元素之和
#include <stdio.h>

int main() {
    int i, j, t, sum=0;
    for (i=0; i<3; ++i) {
        for (j=0; j<3; ++j) {
            scanf("%d", &t);
            if (i==j) sum+=t;
        }
    }
    printf("%d\n", sum);
    return 0;
}