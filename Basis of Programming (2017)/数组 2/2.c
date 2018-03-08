// 数组的变换
#include <stdio.h>
#define N 10

void operation(int *array) {
    for (int i=0; i<N; ++i)
        if (array[i]<0) array[i] = -array[i];
        else if (array[i]>0) array[i]*=array[i];
}

int main() {
    int array[N];
    for (int i=0; i<N; ++i) scanf("%d", &array[i]);
    operation(array);
    for (int i=0; i<N; ++i) printf("%d ", array[i]);
    printf("\n");
    return 0;
}