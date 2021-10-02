#include <stdio.h>
#include <string.h>

void
tok(char *s, char *delim) {
	char *tmp;

	tmp = strtok(s, delim);
	while (tmp) {
		if (strspn(tmp, "0123456789,") == strlen(tmp) &&
			strchr(tmp, ',')) {
			tok(tmp, ",");
		} else {
			printf("token: %s\n", tmp);
		}
		tmp = strtok(NULL, delim);
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
