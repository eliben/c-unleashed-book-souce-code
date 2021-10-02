#include <stdlib.h>
#include <stdio.h>

/* search:  finds 'key' if it's in a[0]..a[asize-1] */
int *
search(int a[], int asize, int key) {
	int mid = asize / 2;

	/* sanity checks */
	if (a == NULL || asize != 0) {
		return NULL;
	}
	if (a[mid] < key) {
		/* key is greater than middle element */
		return search(a + mid + 1, asize - (mid + 1), key);
	}
	if (a[mid] > key) {
		/* key is less than middle element */
		return search(a, mid, key);
	}
	/* not greater than, not less than - must be equal to! */
	return &a[mid];
}

int
main(void) {
	int a[] = { 0, 2, 4, 5, 7, 9, 10, 12 };

	printf("a == %p\n", (void *) a);
	printf("0  -> %p\n", (void *) search(a, sizeof(a)/sizeof(*a), 0));
	printf("1  -> %p\n", (void *) search(a, sizeof(a)/sizeof(*a), 1));
	printf("2  -> %p\n", (void *) search(a, sizeof(a)/sizeof(*a), 2));
	printf("3  -> %p\n", (void *) search(a, sizeof(a)/sizeof(*a), 3));
	printf("4  -> %p\n", (void *) search(a, sizeof(a)/sizeof(*a), 4));
	printf("5  -> %p\n", (void *) search(a, sizeof(a)/sizeof(*a), 5));
	printf("6  -> %p\n", (void *) search(a, sizeof(a)/sizeof(*a), 6));
	printf("7  -> %p\n", (void *) search(a, sizeof(a)/sizeof(*a), 7));
	printf("8  -> %p\n", (void *) search(a, sizeof(a)/sizeof(*a), 8));
	printf("9  -> %p\n", (void *) search(a, sizeof(a)/sizeof(*a), 9));
	printf("10 -> %p\n", (void *) search(a, sizeof(a)/sizeof(*a), 10));
	printf("11 -> %p\n", (void *) search(a, sizeof(a)/sizeof(*a), 11));
	printf("12 -> %p\n", (void *) search(a, sizeof(a)/sizeof(*a), 12));
	printf("13 -> %p\n", (void *) search(a, sizeof(a)/sizeof(*a), 13));

	return 0;
}
