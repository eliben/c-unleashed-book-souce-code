#include <stdlib.h>
#include <stdio.h>

static unsigned long count;

long
fact(int x) {
	long accumulator = 1;
	++count;
	while (x > 1) {
		accumulator *= x--;
	}
	return accumulator;
}

int
main(void) {
	int i;
	long f;

	for (i = 1; i < 20; ++i) {
		count = 0;
		f = fact(i);
		printf("fact(%02d) gives %ld after %lu iteration%s.\n",
			i, f, count, count == 1 ? "" : "s");
	}

	return 0;
}
