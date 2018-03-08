#include <stdio.h>

int main() {
	int step=0, n;
	scanf("%d", &n);

	while (n!=1) {
		if (n&1) n=(3*n+1)/2; // odd
		else n/=2;  // even
		++step;
	}
	printf("%d\n", step);
}