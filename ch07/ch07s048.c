#include <stdio.h>
#include <assert.h>

#define  SWAP(x, y)  x^= y^= x;

int
main()
{
    char *s="uvwxyz";
    char *t;
    unsigned long a, b, c;
    int i;

    printf("String: %s (%d entries).\n"
           "Change which character to '1'? ", s,
           strlen(s));
    scanf("%d", &a);

    assert((0 <= a) && (a < strlen(s)));
    s[a]= '1';

    printf("Original changed to %s\n", s);
    printf("Reverse which range of characters (from-to)? ",
           b, c);

    scanf("%lu%lu", &a, &b);
    assert(a<=b);

    t= malloc(strlen(s+1));

    strcpy(t, s);
    for (i=b; i<(b+c)/2; i++)
        SWAP(t[b+i], t[c-i]);

    printf("Result of reverse is %s -> %s\n", s, t);
    return 0;
}
