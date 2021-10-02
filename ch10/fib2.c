#include <stdlib.h>
#include <stdio.h>

static unsigned long count;

int
fib(int x) {
	int i, *a, r;

	if (x < 3)
		return 1;

	a = malloc(x * sizeof(int));

	if (!a)
		return -1;

	a[1] = a[0] = 1;
	for (i = 2; i < x; ++i) {
		a[i] = a[i - 1] + a[i - 2];
	}

	r = a[x - 1];
	free(a);
	return r;
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
