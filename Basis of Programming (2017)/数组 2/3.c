// 矩阵乘积

// 1. 由于传递二维数组的过程中会损失长度信息，
//    所以 array[i][j] 只得写成 array[i*n+j] 的形式了。
// 2. 由于长度是动态获取的，所以也无法设置为全局变量，
//    也无法在 multiply() 中把类型强制转换成 int [*n][*m]。
// 3. 我不喜欢 int array[100][100];
// 综上，代码只能用这种方法曲线救国了…

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
int n1, m1, n2, m2;

int *multiply(int*, int*);
int *input(int*, int*);
int main() {
    int *a = input(&n1, &m1);
    int *b = input(&n2, &m2);

    int *result = multiply(a, b);
    if (!result) printf("error\n");
    else {
        for (int i=0; i<n1; ++i) {
            for (int j=0; j<m2; ++j)
                printf("%d ", result[i*n1+j]);
            printf("\n");
        }
    }

    free(a); free(b); free(result);

    return 0;
}

int *input(int *n, int *m) {
    scanf("%d%d", n, m);
    int *matrix = (int *) malloc(sizeof(int[*n][*m]));
    for (int i=0; i<*n; ++i)
        for (int j=0; j<*m; ++j)
            scanf("%d", &matrix[(*m)*i+j]);
    return matrix;
}

int *multiply(int *a, int *b) {
    if (m1 != n2) return 0;
    int *result = (int *) malloc(sizeof(int[n1][m2]));
    memset(result, 0, sizeof(int[n1][m2]));

    for (int i=0; i<n1; ++i)
        for (int j=0; j<m2; ++j)
            for (int k=0; k<m1; ++k)
                result[i*m2+j] += a[i*m1+k] * b[k*m2+j];
    return result;
}