#include <stdio.h>
#include <math.h>
const double PI = 3.14;

int main() {
	double radius, height;
	printf("give me the value of radius and height: ");
	scanf("%lf%lf", &radius, &height);
	printf("V=%.2lf\nS=%.2lf\n",
		PI*pow(radius, 2)*height, 2*PI*(pow(radius, 2)+radius*height));
}