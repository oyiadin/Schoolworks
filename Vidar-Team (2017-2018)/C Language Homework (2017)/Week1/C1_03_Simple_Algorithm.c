#include <stdio.h>
#include <math.h>

int main() {
	const int N = 3;

	int a, b, i, digits[N];
	for (i=0; i<N; ++i) scanf("%d", &digits[i]);

	int sum=0;
	for (i=0; i<N; ++i) sum+=(digits[i]*11)*(N-1);
	// 每个数都会分别作为十位、个位出现 N-1 次
	printf("%d\n", sum);
}