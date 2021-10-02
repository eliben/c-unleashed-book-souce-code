
/* 

speed.c

DESCRIPTION
    Give timings for basic operations and library functions
    in C.  This code was used to generate the table
    of operation vs. time values in Chapter 3.

    You must compile this with optimizations off.  Many
    trivial operations would be optimized completely out of
    existence and become unmeasurable.

    For best results: run twice in a row, ignore first
    batch of results.

    To add a new timing test, enclose it between the BEGIN_TEST
    and END_TEST macros.

      BEGIN_TEST("your test name");
        your_code_here();
      END_TEST();

    Your new timing test may appear anywhere after the
    computation of the baseline variable, i.e. right
    after the "loop overhead" test.

    There are two #define'd constants that may be adjusted
    to improve the accuracy of the report:

    MIN_DURATION is the minimum number of seconds to run
    each test.  The larger this is, the more accurate the
    results will be.

    MIN_ITER is the minimum number of trials to run each
    test.  Normally, MIN_DURATION will be the deciding
    factor, but some extremely long calls like system() may
    take longer than MIN_DURATION seconds to complete 1
    operation.  In that case, MIN_ITER is used to make sure
    that at least that many calls are made, no matter how
    much time it takes.

NOTE
    This is not a statistically meaningful benchmark and so
    it should not be used to compare results on different
    machines.

AUTHOR
    Copyright 1999 Michael Lee
    Use subject to the GNU Public License.

*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MIN_DURATION 10.0  /* repeat each text for at least this many seconds */
#define MIN_ITER  10      /* or at least this often, whichever is greater */
#define SIGDIGIT   5      /* don't round when first non-zero digit
                             of precision is at least this much */

/* CLK_TCK was documented in K&R2 first printing, but was later renamed. */
/* Old but otherwise okay C89 compilers may still use it */
#ifndef CLOCKS_PER_SEC
  #define CLOCKS_PER_SEC CLK_TCK
#endif

/* some variables used by the macros, below */
static clock_t op_start, op_end;
static double baseline, usec, resolution;
static int i, prec;

/* macros for starting and ending each particular test; made them
   into macros so the rest of the code isn't full of tedious bookkeeping 
   stuff. */

#define BEGIN_TEST(name) printf("%20s ", name); fflush(stdout); \
  i = 0; op_start = clock(); \
  while(clock() == op_start) /* (busy) wait for clock to turn over */; \
  op_start = clock(); \
  while ((op_end = clock()) - op_start < CLOCKS_PER_SEC * MIN_DURATION) { \
    int q; \
    for (q=0; q < MIN_ITER; q++) {

#define END_TEST()  i++; } } \
  usec = \
    1000000.0 * (double) (op_end - op_start - baseline * i) / \
    (double) CLOCKS_PER_SEC / (double) i; \
  /* just zero out anything too inherently noisy to be useful */ \
  if (usec <= 0.0) usec = 0.0; \
  printf("%9.3f usec", usec); \
  /* uncomment if you would like to display equiv frequency \
  if (usec > 0.0) printf("%9.3f Mhz", 1.0 / usec); \
             else printf("      n/a");  \
  */ \
  printf("\n");

/* empty function, had to put it somewhere */
void funccall()
{
  return;
}

int main(void)
{
  double tempres;
  int x;
  int array[100010];
  char buf[80];
  float foo;
  float bar = 1.23456789;
  double foo2;
  int able;
  int baker = 1234;
  char * charlie;
  FILE * fd;

  srand(1);

  /* simple arithmetic to figure out how precise the measurements
     might be */
  printf("clocks_per_sec = %d\n", CLOCKS_PER_SEC);
  resolution = 1000000.0 / CLOCKS_PER_SEC / MIN_ITER;
  printf("worst case resolution = %5.4f usec\n", resolution);

  /* figure out how many significant digits to output */
  prec = 0;
  for (tempres = resolution; tempres < SIGDIGIT; tempres *= 10)
  {
    prec ++;
  }
  printf("meaningful precision = %d decimal digits\n", prec);

  /* this could happen, so we might as well check */
  if (clock() == -1)
  {
    fprintf(stderr, "clock() is broken.\n");
    exit(1);
  }

  baseline = 0;

  /* we need to do this because the first time through a program
     there is a significant penalty while the system loads needed
     resources. */
  BEGIN_TEST("(cache & vm load)");
  END_TEST();

  /* figure out the loop overhead so that we can deduct it 
     from the remainder of the tests.  sometimes this measurement is
     noisy because it's so trivial.  increase MIN_DURATION if it's
     a problem */
  BEGIN_TEST("(loop overhead)");
    /* nothing */;
  END_TEST();

  baseline = (double) (op_end - op_start) / (double) i;

  BEGIN_TEST("empty");
  END_TEST();

  BEGIN_TEST("/* comments */");
    /* a comment */
  END_TEST();

  BEGIN_TEST("#define");
    #define e 2.7182818
  END_TEST();

  BEGIN_TEST("declaration");
  {
    int bob;
  }
  END_TEST();

  BEGIN_TEST("array[]");
    x = array[910];
  END_TEST();

  BEGIN_TEST("*pointer");
    x = *(array+910);
  END_TEST();

  BEGIN_TEST("int =");
    able = baker;
  END_TEST();

  BEGIN_TEST("empty func()");
    funccall();
  END_TEST();

  BEGIN_TEST("bit shift");
    able = baker << 11;
  END_TEST();

  BEGIN_TEST("if-then-else");
    if (baker == 1002)
    {}
    else 
    {}
  END_TEST();

  BEGIN_TEST("int + int");
    able = baker + baker;
  END_TEST();

  BEGIN_TEST("int - int");
    able = baker - baker;
  END_TEST();

  BEGIN_TEST("int ^ int");
    able = baker ^ baker;
  END_TEST();

  BEGIN_TEST("int * int");
    able = baker * baker;
  END_TEST();

  BEGIN_TEST("int / int");
    able = baker / baker;
  END_TEST();

  BEGIN_TEST("(int) float");
    foo = (float) baker;
  END_TEST();

  BEGIN_TEST("float + float");
    foo = bar + bar;
  END_TEST();

  BEGIN_TEST("float * float");
    foo = bar * bar;
  END_TEST();

  BEGIN_TEST("float / float");
    foo = bar / bar;
  END_TEST();

  BEGIN_TEST("strcpy()");
    #define xx "My name is not ralph, it is fred."
    (void) strcpy(buf, xx);
  END_TEST();

  BEGIN_TEST("strcmp()");
    #define xx "My name is not ralph, it is fred."
    (void) strcmp(xx, "My name is not bjork, it is sven.");
  END_TEST();

  BEGIN_TEST("rand()");
    able = rand();
  END_TEST();

  BEGIN_TEST("sqrt()");
    foo2 = sqrt(bar);
  END_TEST();

  BEGIN_TEST("malloc/free");
    charlie = malloc(rand() % 4096);
    free(charlie);
  END_TEST();

  BEGIN_TEST("fopen/fclose");
    fd = fopen("speed.c", "r");
    fclose(fd);
  END_TEST();

  BEGIN_TEST("system()");
    system("true");
  END_TEST();

  return 0;
}
