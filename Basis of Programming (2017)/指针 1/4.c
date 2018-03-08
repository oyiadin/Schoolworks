// 矩阵交换行
#include <stdio.h>
#define X 5
#define Y 5

void swap(int[X][Y], int, int);
int main() {
    int matrix[X][Y];
    for (int i=0; i<X; ++i) for (int j=0; j<Y; ++j)
        scanf("%d", &matrix[i][j]);
    int l1, l2;
    scanf("%d%d", &l1, &l2);

    swap(matrix, l1, l2);
    for (int i=0; i<X; ++i) {
        printf("%s", (i==l1 || i==l2) ? "->" : "  ");
        for (int j=0; j<Y; ++j) printf("%d ", matrix[i][j]);
        printf("\n");
    }

    return 0;
}


void swap(int m[X][Y], int l1, int l2) {
    int t;
    for (int i=0; i<Y; ++i) {
        t = m[l1][i];
        m[l1][i] = m[l2][i];
        m[l2][i] = t;
    }
}