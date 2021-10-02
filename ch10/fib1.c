#include <stdlib.h>
#include <stdio.h>

static unsigned long count;

int
fib(int x) {
	int r;
	++count;
	if (x == 1 || x == 2)
		return 1;
	return fib(x - 2) + fib(x - 1);
}

int
main(void) {
	int i, f;

	for (i = 1; i < 20; ++i) {
		count = 0;
		f = fib(i);
		printf("fib(%02d) gives %d after %lu iteration%s.\n",
			i, f, count, count == 1 ? "" : "s");
	}

	return 0;
}
