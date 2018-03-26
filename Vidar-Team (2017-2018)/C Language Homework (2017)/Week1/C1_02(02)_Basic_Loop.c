#include <stdio.h>

int main() {
	int n, i, j, total, pos1, pos2;
	scanf("%d", &n);
	total = 2*n+1;
	pos1 = 0;
	pos2 = total-1;

	for (i=1; i<=total; ++i) {
		for (j=0; j<total; ++j) {
			if (j==pos1 || j==pos2) printf("*");
			if ((pos2>pos1) ? (j<pos1) : (j<pos2)) printf(" ");
			// pos1 can be bigger than pos2
			if ((j-pos1)*(j-pos2) < 0) printf(" ");
			// if different sign, i.e. between pos1 and pos2
			if ((pos2>pos1) ? (j>pos2) : (j>pos1)) break;
		}
		++pos1;
		--pos2;
		printf("\n");
	}
}