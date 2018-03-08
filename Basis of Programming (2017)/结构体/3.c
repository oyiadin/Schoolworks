// 矩形相关操作
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef struct { int x, y; } point;
typedef struct { point upper_left, lower_right; } rectangle;

int area(rectangle r) {
    return abs(
        (r.upper_left.x - r.lower_right.x) * (r.upper_left.y - r.lower_right.y));
}

point *center(rectangle r) {
    point *p = (point *) malloc(sizeof(point));
    p->x = (r.upper_left.x + r.lower_right.x) / 2;
    p->y = (r.upper_left.y + r.lower_right.y) / 2;
    return p;
}

int is_within(point p, rectangle r) {
    if ((p.x - r.upper_left.x) * (p.x - r.lower_right.x) <= 0)
        if ((p.y - r.upper_left.y) * (p.y - r.lower_right.y) <= 0)
            return 1;
    return 0;
}

int main() {
    point p;
    rectangle r;
    scanf("%d%d%d%d", &r.upper_left.x, &r.upper_left.y,
        &r.lower_right.x, &r.lower_right.y);
    scanf("%d%d", &p.x, &p.y);

    point *c = center(r);
    printf("Area of r is %d\n"
           "Center of r is <%d,%d>\n"
           "Point <%d,%d> is %sin r\n",
           area(r), c->x, c->y, p.x, p.y,
           (is_within(p, r)) ? "" : "not ");

    free(c);
    return 0;
}