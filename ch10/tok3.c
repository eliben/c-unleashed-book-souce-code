#include <stdio.h>
#include <string.h>

char *
sepstr(char **s, char *delim) {
	char *ret;
	size_t n;

	/* sanity check */
	if (!s || !*s || !delim)
		return NULL;

	/* we return a pointer to the current value of s */
	ret = *s;

	/* first, we find out how much of '*s' is *not* in 'delim' - we'll
	 * call that value 'n'.
	 *
	 * if (*s)[n] is a null byte, the entire string contains no instances
	 * of any character in delim; otherwise, it's the first delimiter
	 * found.
	 */
	n = strcspn(*s, delim);
	if ((*s)[n]) {
		(*s)[n] = '\0';
		/* advance s to one past the first delimiter */
		*s += (n + 1);
	} else {
		/* no more strings to return */
		*s = NULL;
	}

	/* and we return the saved pointer to the old contents of *s. */
	return ret;
}

void
tok(char *s, char *delim) {
	char *tmp;

	tmp = sepstr(&s, delim);
	while (tmp) {
		if (strspn(tmp, "0123456789,") == strlen(tmp) &&
			strchr(tmp, ',')) {
			tok(tmp, ",");
		} else {
			printf("token: %s\n", tmp);
		}
		tmp = sepstr(&s, delim);
	}
}

int
main(void) {
	/* never used a fixed size buffer like this in real code! */
	char buf[1024];
	/* to eat the newline */
	char *nl;

	while (fgets(buf, 1024, stdin)) {
		/* I consider the != NULL superfluous, but some compilers
		 * will give a warning without it.
		 */
		if ((nl = strchr(buf, '\n')) != NULL) {
			/* no need for newline */
			*nl = '\0';
		}
		tok(buf, " ");
	}
}
