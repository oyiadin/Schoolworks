//求最大最小数
#include <stdio.h>

int main() {
    int i=10;
    double a[10], min, max;
    while (i--) {
        scanf("%lf", &a[i]);
        if (i==9) min=max=a[i];
        else if (a[i] < min) min = a[i];
        else if (a[i] > max) max = a[i];
    }
    printf("%lf\n%lf", max, min);

    return 0;
}