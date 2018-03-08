// 对数组进行排序
#include <stdio.h>
#include <stdlib.h>
#define LENGTH 5

int cmp(const void *a, const void *b) {
    int *aa = (int*) a, *bb = (int*) b;
    printf("-> %d %d\n", *(aa+1), *(bb+1));
    
    return *(aa+1) - *(bb+1);
}

int main() {
    int array[LENGTH][2];
    for (int i=0; i<LENGTH; ++i) {
        array[i][0] = i;
        scanf("%d", &array[i][1]);
    }
    qsort(array, LENGTH, sizeof(array[0]), cmp);

    for (int i=0; i<LENGTH; ++i)
        printf("%d %d\n", array[i][0], array[i][1]);

    return 0;
}
