/* m_test.c - test code for matrix library */

#include "m_matrix.h"

int main( void) {
    FILE *fp;
    MATRIX_T *a, *b, *c, *d, *inverse, *test, *x, *ainv;
    double D;

    /* initialize all MATRIX_T pointers to NULL */
    a = NULL;
    b = NULL;
    c = NULL;
    d = NULL;
    inverse = NULL;
    test = NULL;
    x = NULL;
    ainv = NULL;

    /* it is good practice to reset the error code
       before doing matrix calculations */
    m_reseterr();

    /* open matrix file to initialize matrix variables */
    if ((fp = fopen("mtest1.mat","r"))==NULL) {
        printf("cannot open file mtest1.mat\n");
        exit(0);
    }

    /* use m_printf functions here */

    /* test matrix addition */
    a = m_fnew( fp);
    m_printf( "\n# matrix a from file: mtest1.mat", "%6.2f", a);
    b = m_fnew( fp);
    m_printf( "\n# matrix b from: mtest1.mat", "%6.2f", b);
    c = m_new( 3, 2);
    c = m_add( c, a, b);
    m_printf( "\n# sum of a and b", "%6.2f", c);

    /* test matrix subtraction */
    c = m_sub( c, a, b);
    m_printf( "\n# difference of a and b", "%6.2f", c);

    /* change to using comma separated format output */

    /* multiply matrix by a constant */
    printf( "\ncomma separated format: matrix a\n");
    m_fputcsv(stdout,a);
    printf( "\ncomma separated format: matrix c\n");
    m_fputcsv(stdout,c);
    m_mupconst( c, 2.5, a);
    printf( "\ncsv format: 2.5 times matrix c\n");
    m_fputcsv(stdout,c);

    /* find maximum element in matrix */
    printf( "\nmax element in c is %f\n", m_max_abs_element(c));

    /* test euclidean norm */
    d = m_fnew( fp);
    printf( "\ncsv format: matrix d\n");
    m_fputcsv(stdout,d);
    printf( "\neuclidean norm of d is %f\n", m_e_norm( d));

    /* test assignment of identity matrix to a square matrix */
    inverse = m_new( d->rows, d->cols);
    m_assign_identity( inverse);
    m_printf( "\nidentity matrix", "%6.2f", inverse);

    /* test matrix inversion */
    inverse = m_inverse( inverse, d, &D, 0.0001);
    test = m_new(d->rows,d->cols);
    test = m_mup( test, d, inverse);
    m_printf( "\nmatrix d", "%6.2f", d);
    m_printf( "\nmatrix inverse", "%6.2f", inverse);
    m_printf( "\nproduct of d and d-inverse", "%6.2f", test);

    /* test solution of linear equations */

    /* start by getting new values for matrices a and b
       note: b is a 1 by n vector (as is x) */
    if ((a = m_fnew( fp)) == NULL) exit(0);
    if ((b = m_fnew( fp)) == NULL) exit(0);
    if ((x = m_new(b->rows,b->cols)) == NULL) exit(0);
    printf("\ncsv: a\n");
    m_fputcsv(stdout,a);
    printf("\ncsv: b\n");
    m_fputcsv(stdout,b);
    x = m_solve( x, a, b, 0.0000001);
    printf("\n\nx=ab\ncsv: solution x\n");
    m_fputcsv(stdout,x);

    /* close files and clean up */
    fclose(fp);
    m_free(a);
    m_free(b);
    m_free(c);
    m_free(d);
    m_free(inverse);
    m_free(test);
    m_free(x);
    m_free(ainv);
 }
