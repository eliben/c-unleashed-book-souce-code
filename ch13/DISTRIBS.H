/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/


/*
** Prototypes for creation of data distributions.
** Testing purposes only.
*/
typedef struct {
    char           *dtype;
    char           *dname;
    char           *iname;
    FILE           *fdbl;
    FILE           *fint;
}               sortfile;

enum distribution_type {
    constant, five, ramp, haphazard, reverse, sorted,
    ten, twenty, two, perverse, trig, unknown
};

int             dopen(void);
int             dclose(void);
void            dsave(int[], double[], unsigned int, const char *);
long            dload(int[], double[], unsigned int, const char *);
int             drandom(int);

void            init_random(int[], double[], const int, const int, const int);
void            init_two(int[], double[], const int, const int);
void            init_five(int[], double[], const int, const int);
void            init_ten(int[], double[], const int, const int);
void            init_twenty(int[], double[], const int, const int);
void            init_sorted(int[], double[], const int, const int, const int);
void            init_reverse(int[], double[], const int);
void            init_constant(int[], double[], const int, const int);
void            init_ramp(int[], double[], const int, const int, const int);
void            init_trig(int[], double[], const int);
void            init_perverse(int[], double[], const int);

void            create_distribs(double[], int[], unsigned int);
void            make_distrib(double d[], int a[], size_t n, enum distribution_type which);
