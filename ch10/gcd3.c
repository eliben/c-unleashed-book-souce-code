#include <stdlib.h>
#include <stdio.h>

static int depth;

int
gcd(int x, int y) {
	int tmp;
	if (x < y) {
		tmp = x;
		x = y;
		y = tmp;
	}
	while ((x % y) != 0) {
		tmp = x % y;
		x = y;
		y = tmp;
	}
	printf("returning %d\n", y);
	return y;
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
