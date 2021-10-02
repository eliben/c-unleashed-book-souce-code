#include <stdlib.h>
#include <stdio.h>

int depth;

int
gcd(int x, int y) {
	int r;
	++depth;
	if (x < y) {
		printf("%*sswap...\n", depth, "");
		r = gcd(y, x);
		printf("%*sreturning %d\n", depth, "", r);
		--depth;
		return r;
	} else if ((x % y) != 0) {
		printf("%*sremainder %d, calling gcd(%d, %d)\n",
			depth, "", x % y, y, x % y);
		r = gcd(y, x % y);
		printf("%*sreturning %d\n", depth, "", r);
		--depth;
		return r;
	} else {
		printf("%*sremainder 0, returning %d\n", depth, "", y);
		--depth;
		return y;
	}
}

int
main(void) {
	int i, j;
	printf("two numbers? ");
	fflush(stdout);
	if (scanf("%d %d", &i, &j) == 2) {
		printf("calling gcd(%d, %d):\n", i, j);
		gcd(i, j);
	} else if (feof(stdin)) {
		printf("\nbye-bye!\n");
		return 0;
	} else {
		printf("\ninput confusing.  giving up.\n");
		return 1;
	}
	return 0;
}
