#include <stdlib.h>
#include <stdio.h>

static unsigned long count;

long
fact(int x) {
	long accumulator = 1;
start:
	++count;
	/* fact(0) is 1, negative numbers don't make sense so we pretend
	 * they're 1. */
	if (x < 2)
		return accumulator;

	/* we multiply the accumulator by our top-level x */
	accumulator *= x;
	/* and trim x by one */
	x = x - 1;
	goto start;
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
