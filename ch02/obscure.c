#include    <stdio.h>
#include   <stdlib.h>
#define   P    printf
#define   I      atoi
int main(int a,char*v
[]){int r=7, i;if(a>1
) r=I(v[1]); if(r<=0)
r=5;if(r%2==0)++r;for
(i=0; i<r*r; P(i/r==(
3*r)/2-(i%r+1)||i/r==
r/2 - i%r||i/r==r/2+i
%r||i/r==i%r-r/2?"*":
" "),i++, i % r==0?P(
"\n") : 0);return 0;}

