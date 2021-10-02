/* m_matrix.h - matrix arithmetic */
#ifndef M_MATRIX_H /* only include the header file once */

/* include other library header files needed */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MMAXITERATIONS 10
#define LINELEN 4096

#define RMISMATCH       110 /* finalize later */
#define CMISMATCH       111
#define NOTSQUARE       112
#define ALLOCFAIL       113
#define FILEREADFAIL    114
#define ROWPARSEFAIL    115
#define COLPARSEFAIL    116
#define RCMISMATCH      117
#define INDEXOUTOFRANGE 118
#define LENMISMATCH     119
#define NULLARG         120

typedef struct {
    int rows;
    int cols;
    double *val;
} MATRIX_T;

/* allocates a new matrix, elements not initialized */
MATRIX_T *
m_new( int nrows, int ncols);

/* frees a MATRIX_T */
void 
m_free( MATRIX_T *m);

/* allocates a new matrix and initializes it from a file */
MATRIX_T *
m_fnew( FILE *fp);

/* assigns the element values of a matrix from a 2D array */
MATRIX_T *
m_assign_arr2( MATRIX_T *a, int nrows, int cols, double *arr);

/* assigns the element values of a matrix from a 1D array */
MATRIX_T *
m_assign_arr1( MATRIX_T *a, int alen, double *arr);

/* matrix assignment a = b */
MATRIX_T *
m_assign( MATRIX_T *a, MATRIX_T *b);

/* matrix addition sum = a + b */
MATRIX_T *
m_add( MATRIX_T *sum, MATRIX_T *a, MATRIX_T *b);

/* matrix subtraction diff = a - b */
MATRIX_T *
m_sub( MATRIX_T *diff, MATRIX_T *a, MATRIX_T *b);

/* matrix transposition trans = a transpose */
MATRIX_T *
m_transpose( MATRIX_T *trans, MATRIX_T *a);

/* matrix multiplication prod = a * b */
MATRIX_T *
m_mup( MATRIX_T *prod, MATRIX_T *a, MATRIX_T *b);

/* matrix inversion inv = a inverse */
MATRIX_T *
m_inverse( MATRIX_T *inv, MATRIX_T *a, 
           double *det, double epsilon);

/* assigns the values of the identity matrix to iden */
MATRIX_T *
m_assign_identity( MATRIX_T *iden);

/* returns the value of the determinant */
double
m_det( MATRIX_T *a, double epsilon);

/* returns the value of a specified matrix element */
double
m_getij( MATRIX_T *a, int i, int j);

/* returns the number of rows in a matrix */
int
m_getrows( MATRIX_T *a);

/* returns the number of columns in a matrix */
int
m_getcols( MATRIX_T *a);

/* sets a specified matrix element to specified value */
int
m_putij( MATRIX_T *a, int i, int j, double value);

/* prints a matrix to stdout with a label
   using a specified numeric format  */
void 
m_printf( char *label, char *format, MATRIX_T *a);

/* outputs a matrix to a file in 
 * comma separated value format */
void 
m_fputcsv( FILE *fp, MATRIX_T *a);

/* sets the error code  */
void 
m_seterr( int errcode);

/* resets the error code  */
void 
m_reseterr( void);

/* returns the string describing the error code  */
const char *
m_errmsg( int errcode);

/* returns the value of the error code */
int 
m_errcode( void);

/* .  */
char *
m_getline( FILE *fp);

/* multiply each element of matrix by a constant c
   prod = c * a */
MATRIX_T *
m_mupconst( MATRIX_T *cprod, double c, MATRIX_T *a);

/* returns the absolute value of the matrix element
   with the largest absolute value  */
double
m_max_abs_element( MATRIX_T *a);

/* returns the Euclidean norm of the matrix, i.e.,
   the square root of the sum of the squares of
   each element */
double
m_e_norm( MATRIX_T *a);

void tprintf( char *comment, char *nformat, 
              MATRIX_T *a, MATRIX_T *b);

static void
set_low_zero( MATRIX_T *t, int col);

static void
swaprows( MATRIX_T *t, int row, int swap);

MATRIX_T *
m_solve(MATRIX_T *x, MATRIX_T *a, MATRIX_T *b, 
        double epsilon);

MATRIX_T *
m_ecsolve(MATRIX_T *x, MATRIX_T *a, MATRIX_T *b, 
          double epsilon);

#endif
