#include <stdlib.h>
#include <stdio.h>

int depth;

int
gcd(int x, int y) {
start:
	if (x < y) {
		int tmp = x;
		printf("x < y, setting x to y and y to x\n");
		x = y;
		y = tmp;
		goto start;
	} else if ((x % y) != 0) {
		int tmp = x % y;
		printf("remainder %d, setting x to y (%d) and y to (x %% y)\n",
			x % y, y);
		x = y;
		y = tmp;
		goto start;
	} else {
		printf("remainder 0, returning y (%d)\n", y);
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
