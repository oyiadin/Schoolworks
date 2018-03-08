#include <stdio.h>

int main() {
	int i, j, n, total, left, right;
	scanf("%d", &n);
	total = 3*n-2;  // n+2(n-1)
	left = n-1;
	right = total-n;
	
	for (i=1; i<=n; ++i) {
		for (j=0; j<total; ++j) {
			if ((i==1 || i==n) && (j>left && j<right)) printf("*");
			else if (j==left || j==right) printf("*");
			else if (j<right && j!=left) printf(" ");
			else if (j>right) break;
		}
		--left;
		++right;
		printf("\n");
	}
}