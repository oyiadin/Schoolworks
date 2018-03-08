#include <stdio.h>
#include <math.h>

int main() {
	int at_least[10] = {0, 0, 0, 0, 0, 0, 2, 3, 5, 7};
	int i, j, k;
	for (i=1; i<=9; ++i) {
		for (j=0; j<=9; ++j) {
			// if ((6<=j && j<=9) && i<at_least[j]) continue;
			/*
			 * 9^3=729, 8^3=512, 7^3=343, 6^3=216
			 * if 9 exists, then it cant be less than 700.
			 * other digits do in the same way.
			 */
			for (k=0; k<=9; ++k) {
				if ((i*100 + j*10 + k) == (int) (pow(i, 3)+pow(j, 3)+pow(k, 3)))
					printf("%d\n", i*100+j*10+k);
			}
		}
	}
}
