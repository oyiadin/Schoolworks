// 最大最小数的差值（指针）
#include <stdio.h>
#define LENGTH 10

float diff(float*);
int main() {
    int i = 10;
    float input, array[LENGTH];
    while (i--) {
        scanf("%f", &input);
        array[i] = input;
    }
    printf("%.2f\n", diff(array));
    return 0;
}

float diff(float *array) {
    float min = array[0], max = array[0];
    for (int i=1; i<LENGTH; ++i) {
        min = (array[i] < min) ? array[i] : min;
        max = (array[i] > max) ? array[i] : max;
    }
    return max-min;
}