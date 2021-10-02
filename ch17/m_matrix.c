/* m_matrix.c - matrix arithmetic */

#include "m_matrix.h"

static int mmerrcode = 0;      /* error code for matrix
                                * library */
static char line[LINELEN];     /* line buffer for file
                                * input */

/* static declarations of functions intended purely for
 * internal use */

/* multiplies the specified row in matrix a by a constant
 * "c" */
static MATRIX_T *
  m_muprow(int row, double c, MATRIX_T * a);

/* subtracts multiples of pivot row from each row below the 
 * pivot to make each column element below the diagonal in
 * column "col" equal to zero */
static void
  set_low_zero(MATRIX_T * t, int col);

/* swaps rows "row" and "swap" in matrix t */
static void
  swaprows(MATRIX_T * t, int row, int swap);

/* function to find the row (on or below the diagonal) in
 * which the maximum element of a column occurs */
static int
  maxelementrow(MATRIX_T * t, int col);

/* function to subtract multiples of the pivot row from all 
 * other rows to force each element in the pivot column to
 * * zero (except the actual pivot element) */
static void
  set_col_zero(MATRIX_T * t, MATRIX_T * v, int col);

/* swap row numbers row and swap in matrices t and v */
static void
  swaprows2(MATRIX_T * t, MATRIX_T * v, int row, int swap);

/*** END OF STATIC FUNCTION DECLARATIONS ***/

/* function to compute index of 1D array corresponding to
 * i,j indices of 2D array 
 * mdx is defined as a function for development
 * and testing; it is commented out and replaced
 * with a macro for production
static int
mdx(MATRIX_T * a, int i, int j)
{
    return i * a->cols + j;
}
 */

#define mdx(a,i,j) (i)*(a)->cols+(j)
/* note use of () around macro arguments for safety */

/* allocates a new matrix, elements not initialized */
MATRIX_T *
m_new(int nrows, int ncols)
{
    double *temp;
    MATRIX_T *m = NULL;
    if ((temp = malloc(nrows * ncols * sizeof(double)))
              == NULL) {
        mmerrcode = ALLOCFAIL;
        return NULL;
    }
    if ((m = malloc(sizeof(MATRIX_T))) == NULL) {
        mmerrcode = ALLOCFAIL;
        free(temp);
        return NULL;
    }
    m->rows = nrows;
    m->cols = ncols;
    m->val = temp;
    return m;
}

/* frees matrix */
void
m_free(MATRIX_T * m)
{
    if ( m == NULL) return;
	 /* not an error; used for recovery in 
	    other matrix library routines */
    free(m->val);
    free(m);
}

char *
m_getline(FILE * fp)
{
    do {
        if (!fgets(line, LINELEN, fp)) {
            mmerrcode = FILEREADFAIL;
            return NULL;
        }
    } while (*line == '#');
    return line;
}

MATRIX_T *
m_fnew(FILE * fp)
{
    int i, j, rows, cols, index;
    char *tok;
    char *lineptr;
    MATRIX_T *a;

    /* get number of rows */
    if (!m_getline(fp)) { return NULL;}
    if (!(tok = strtok(line, ","))) {
        mmerrcode = ROWPARSEFAIL;
        return NULL;
    }
    if (strcmp(tok, "rows")) {
        mmerrcode = ROWPARSEFAIL;
        return NULL;
    }
    if (!(tok = strtok(NULL, ","))) {
        mmerrcode = ROWPARSEFAIL;
        return NULL;
    }
    rows = atoi(tok);
    if (!rows) {
        mmerrcode = ROWPARSEFAIL;
        return NULL;
    }
    /* get number of columns */
    if (!m_getline(fp)) { return NULL;}
    if (!(tok = strtok(line, ","))) {
        mmerrcode = COLPARSEFAIL;
        return NULL;
    }
    if (strcmp(tok, "cols")) {
        mmerrcode = COLPARSEFAIL;
        return NULL;
    }
    if (!(tok = strtok(NULL, ","))) {
        mmerrcode = COLPARSEFAIL;
        return NULL;
    }
    cols = atoi(tok);
    if (!cols) {
        mmerrcode = COLPARSEFAIL;
        return NULL;
    }
    /* allocate new matrix */
    if (!(a = m_new(rows, cols)))
        return NULL;           /* error checking done in
                                * m_new */
    index = 0;
    for (i = 0; i < rows; i++) {
        if (!m_getline(fp)) {
            m_free(a);
            return NULL;
        }
        lineptr = line;
        for (j = 0; j < cols; j++) {
            if (!(tok = strtok(lineptr, ","))) {
                m_free(a);
                return NULL;
            }
            a->val[index] = atof(tok);
            index++;
            lineptr = NULL;    /* NULL arg to strtok after
                                * 1st pass */
        }
    }
    return a;
}

MATRIX_T *
m_add(MATRIX_T * sum, MATRIX_T * a, MATRIX_T * b)
{
    int i, j, index;
    if (sum == NULL || a == NULL || b == NULL) {
        mmerrcode = NULLARG;
        return sum;
    }
    if (a->rows != b->rows) {
        mmerrcode = RMISMATCH;
        return sum;
    }
    if (sum->rows != b->rows) {
        mmerrcode = RMISMATCH;
        return sum;
    }
    if (a->cols != b->cols) {
        mmerrcode = CMISMATCH;
        return sum;
    }
    if (sum->cols != b->cols) {
        mmerrcode = CMISMATCH;
        return sum;
    }
    index = 0;
    for (i = 0; i < sum->rows; i++) {
        for (j = 0; j < sum->cols; j++) {
            sum->val[index] =
                a->val[index] + b->val[index];
            index++;
        }
    }
    return sum;
}

MATRIX_T *
m_sub(MATRIX_T * diff, MATRIX_T * a, MATRIX_T * b)
{
    int i, j, index;
    if (diff == NULL || a == NULL || b == NULL) {
        mmerrcode = NULLARG;
        return diff;
    }
    if (a->rows != b->rows) {
        mmerrcode = RMISMATCH;
        return diff;
    }
    if (diff->rows != b->rows) {
        mmerrcode = RMISMATCH;
        return diff;
    }
    if (a->cols != b->cols) {
        mmerrcode = CMISMATCH;
        return diff;
    }
    if (diff->cols != b->cols) {
        mmerrcode = CMISMATCH;
        return diff;
    }
    index = 0;
    for (i = 0; i < diff->rows; i++) {
        for (j = 0; j < diff->cols; j++) {
            diff->val[index] =
                a->val[index] - b->val[index];
            index++;
        }
    }
    return diff;
}

MATRIX_T *
m_assign(MATRIX_T * a, MATRIX_T * b)
{
    int i, j, index;
    if (a == NULL || b == NULL) {
        mmerrcode = NULLARG;
        return a;
    }
    if (a->rows != b->rows) {
        mmerrcode = RMISMATCH;
        return a;
    }
    if (a->cols != b->cols) {
        mmerrcode = CMISMATCH;
        return a;
    }
    index = 0;
    for (i = 0; i < a->rows; i++) {
        for (j = 0; j < a->cols; j++) {
            a->val[index] = b->val[index];
            index++;
        }
    }
    return a;
}

MATRIX_T *
m_mup(MATRIX_T * prod, MATRIX_T * a, MATRIX_T * b)
{
    int i, j, k;
    if (prod == NULL || a == NULL || b == NULL) {
        mmerrcode = NULLARG;
        return prod;
    }
    if (prod->rows != a->rows) {
        mmerrcode = RMISMATCH;
        return prod;
    }
    if (prod->cols != b->cols) {
        mmerrcode = CMISMATCH;
        return prod;
    }
    if (a->cols != b->rows) {
        mmerrcode = RCMISMATCH;
        return prod;
    }
    for (i = 0; i < a->rows; i++) {
        for (j = 0; j < b->cols; j++) {
            prod->val[mdx(prod,i,j)] = 0.0;
            for (k = 0; k < a->cols; k++) {
                prod->val[mdx(prod,i,j)] +=
                    a->val[mdx(a,i,k)] * b->val[mdx(b,k,j)];
            }
        }
    }
    return prod;
}

MATRIX_T *
m_transpose(MATRIX_T * trans, MATRIX_T * a)
{
    int i, j;
    if (trans == NULL || a == NULL) {
        mmerrcode = NULLARG;
        return trans;
    }
    if (trans->rows != a->cols) {
        mmerrcode = RCMISMATCH;
        return trans;
    }
    if (trans->cols != a->rows) {
        mmerrcode = RCMISMATCH;
        return trans;
    }
    for (i = 0; i < a->rows; i++) {
        for (j = 0; j < a->cols; j++) {
            trans->val[mdx(trans, j, i)] =
                a->val[mdx(a, i, j)];
        }
    }
    return trans;
}

void
tprintf(char *comment, char *nformat,
        MATRIX_T * a, MATRIX_T * b)
{
    int i, j, index;
    if (a == NULL || b == NULL) {
        mmerrcode = NULLARG;
        printf("tprintf NULL argument error\n");
        return;
    }
    printf("%s\n", comment);
    index = 0;
    for (i = 0; i < a->rows; i++) {
        for (j = 0; j < a->cols; j++) {
            printf(nformat, a->val[a->cols * i + j]);
        }
        printf(" ** ");
        for (j = 0; j < a->cols; j++) {
            printf(nformat, b->val[a->cols * i + j]);
        }
        printf("\n");
    }
    printf("\n");
}

MATRIX_T *
m_assign_identity(MATRIX_T * iden)
{
    int i, j, index;
    if (iden == NULL) {
        mmerrcode = NULLARG;
        return iden;
    }
    if (iden->rows != iden->cols) {
        mmerrcode = NOTSQUARE;
        return iden;
    }
    index = 0;
    for (i = 0; i < iden->rows; i++) {
        for (j = 0; j < iden->cols; j++) {
            iden->val[index] = 0.0;
            if (i == j)
                iden->val[index] = 1.0;
            index++;
        }
    }
    return iden;
}

double
m_getij(MATRIX_T * a, int i, int j)
{
    if (a == NULL) {
        mmerrcode = NULLARG;
        return 0.0;
    }
    if (i > a->rows - 1) {
        mmerrcode = INDEXOUTOFRANGE;
        return 0.0;
    }
    if (j > a->cols - 1) {
        mmerrcode = INDEXOUTOFRANGE;
        return 0.0;
    }
    return a->val[mdx(a, i, j)];
}

int
m_putij(MATRIX_T * a, int i, int j, double value)
{
    if (a == NULL) {
        mmerrcode = NULLARG;
        return mmerrcode;
    }
    if (i > a->rows - 1) {
        mmerrcode = INDEXOUTOFRANGE;
        return mmerrcode;
    }
    if (j > a->cols - 1) {
        mmerrcode = INDEXOUTOFRANGE;
        return mmerrcode;
    }
    a->val[mdx(a, i, j)] = value;
    return 0;
}

int
m_getrows(MATRIX_T * a)
{
    if (a == NULL) {
        mmerrcode = NULLARG;
        return mmerrcode;
    }
    return a->rows;
}

int
m_getcols(MATRIX_T * a)
{
    if (a == NULL) {
        mmerrcode = NULLARG;
        return mmerrcode;
    }
    return a->cols;
}

void
m_printf(char *label, char *format, MATRIX_T * a)
{
    int i, j;
    if (a == NULL) {
        printf("m_printf NULL argument error\n");
        return;
    }
    printf("%s\n", label);
    printf("rows = %d, cols = %d\n", a->rows, a->cols);
    for (i = 0; i < a->rows; i++) {
        for (j = 0; j < a->cols; j++) {
            printf(format, a->val[mdx(a,i,j)]);
        }
        printf("\n");
    }
}

void
m_fputcsv(FILE * fp, MATRIX_T * a)
{
    int i, j;
    char *sep;
    char comma[] = ",";
    char nocomma[] = "";
    if (a == NULL) {
        mmerrcode = NULLARG;
        return;
    }
    fprintf(fp, "rows,%d\n", a->rows);
    fprintf(fp, "cols,%d\n", a->cols);
    for (i = 0; i < a->rows; i++) {
        sep = nocomma;
        for (j = 0; j < a->cols; j++) {
            fprintf(fp, "%s%f", sep, a->val[mdx(a,i,j)]);
            sep = comma;
        }
        fprintf(fp, "\n");
    }
}

void
m_seterr(int errcode)
{
    mmerrcode = errcode;
}

void
m_reseterr(void)
{
    mmerrcode = 0;
}

const char *
m_errmsg(int errcode)
{
    switch (errcode) {
        case RMISMATCH:
            return "row mismatch";
        case CMISMATCH:
            return "column mismatch";
        case NOTSQUARE:
            return "not a square matrix";
        case ALLOCFAIL:
            return "allocation failure";
        case FILEREADFAIL:
            return "file read failure";
        case ROWPARSEFAIL:
            return "row parse failure";
        case COLPARSEFAIL:
            return "column parse failure";
        case RCMISMATCH:
            return "row-column mismatch";
        case INDEXOUTOFRANGE:
            return "index out of range";
        case LENMISMATCH:
            return "length mismatch";
        case NULLARG:
            return "NULL argument";
        default:
            return NULL;
    }
}

int
m_errcode(void)
{
    return mmerrcode;
}

MATRIX_T *
m_assign_arr2(MATRIX_T * a, int nrows,
              int ncols, double *arr)
{
    int len;
    if (a == NULL) {
        mmerrcode = NULLARG;
        return a;
    }
    if (nrows != a->rows) {
        mmerrcode = RMISMATCH;
        return a;
    }
    if (ncols != a->cols) {
        mmerrcode = CMISMATCH;
        return a;
    }
    len = nrows * ncols;
    return m_assign_arr1(a, len, arr);
}

MATRIX_T *
m_assign_arr1(MATRIX_T * a, int alen, double *arr)
{
    int i, j, index;
    if (a == NULL) {
        mmerrcode = NULLARG;
        return a;
    }
    if (alen != a->rows * a->cols) {
        mmerrcode = LENMISMATCH;
        return a;
    }
    index = 0;
    for (i = 0; i < a->rows; i++) {
        for (j = 0; j < a->cols; j++) {
            a->val[index] = arr[index];
            index++;
        }
    }
    return a;
}

MATRIX_T *
m_mupconst(MATRIX_T * cprod, double c, MATRIX_T * a)
{
    int i, j, index;
    if (cprod == NULL || a == NULL) {
        mmerrcode = NULLARG;
        return cprod;
    }
    if (cprod->rows != a->rows) {
        mmerrcode = RMISMATCH;
        return cprod;
    }
    if (cprod->cols != a->cols) {
        mmerrcode = CMISMATCH;
        return cprod;
    }
    index = 0;
    for (i = 0; i < a->rows; i++) {
        for (j = 0; j < a->cols; j++) {
            cprod->val[index] = c * a->val[index];
            index++;
        }
    }
    return cprod;
}

double
m_max_abs_element(MATRIX_T * a)
{
    int i, j, index;
    double maxelement = 0.0;
    if (a == NULL) {
        mmerrcode = NULLARG;
        return 0.0;
    }
    index = 0;
    for (i = 0; i < a->rows; i++) {
        for (j = 0; j < a->cols; j++) {
            if (fabs(a->val[index]) > maxelement)
                maxelement = fabs(a->val[index]);
            index++;
        }
    }
    return maxelement;
}

double
m_e_norm(MATRIX_T * a)
{
    int i, j, index;
    double norm = 0.0;
    if (a == NULL) {
        mmerrcode = NULLARG;
        return 0.0;
    }
    index = 0;
    for (i = 0; i < a->rows; i++) {
        for (j = 0; j < a->cols; j++) {
            norm += a->val[index] * a->val[index];
            index++;
        }
    }
    return sqrt(norm);
}

MATRIX_T *
m_inverse(MATRIX_T * v, MATRIX_T * a,
          double *det, double epsilon)
{
/* calculates the inverse of matrix a using Gauss-Jordan
 * elimination with partial pivot maximization */
    int row, col, swap, sign;
    double pivot, e_norm;
    MATRIX_T *t = NULL;
    if (v == NULL || a == NULL) {
        mmerrcode = NULLARG;
        return v;
    }
    if (v->rows != a->rows) {
        mmerrcode = RMISMATCH;
        return v;
    }
    if (v->cols != a->cols) {
        mmerrcode = CMISMATCH;
        return v;
    }
    if (v->rows != v->cols) {
        mmerrcode = NOTSQUARE;
        return v;
    }
    e_norm = m_e_norm(a);
    *det = 1.0;
    sign = 1;

    /* allocate a "scratch" matrix to invert */
    if (!(t = m_new(a->rows, a->cols))) return v;
    t = m_assign(t, a);

    /* set target matrix to the identity matrix */
    v = m_assign_identity(v);

    for (row = 0; row < t->rows; row++) {
        /* find largest element below diagonal in column */
        swap = maxelementrow(t, row);
        /* swap rows to put largest element on pivot */
        if (swap != row) {
            if (sign > 0)
                sign = -1;
            else
                sign = 1;
            swaprows2(t, v, row, swap);
        }
        /* divide each element on pivot row by pivot
         * element putting a 1 in the pivot element */
        pivot = t->val[mdx(t, row, row)];
        *det *= pivot;
        if ((fabs(*det) / e_norm) < epsilon) {
            return v;          /* potentially singular
                                * matrix */
        }
        t = m_muprow(row, 1. / pivot, t);
        v = m_muprow(row, 1. / pivot, v);

        /* subtract a multiple of the pivot row from each
         * row to put 0's in all elements of the pivot
         * column except the pivot row */
        col = row;
        set_col_zero(t, v, col);
    }
    m_free(t);
    if (sign < 0)
        *det = -*det;
    return v;
}

static int
maxelementrow(MATRIX_T * t, int col)
{
    int row, i, ilargest;
    double maximum, trial;
    maximum = 0.0;
    row = col;
    ilargest = row;
    for (i = row; i < t->rows; i++) {
        trial = fabs(t->val[mdx(t, i, col)]);
        if (trial > maximum) {
            maximum = trial;
            ilargest = i;
        }
    }
    return ilargest;
}

static void
set_col_zero(MATRIX_T * t, MATRIX_T * v, int col)
{
    int i, j;
    double pivot, factor;
    pivot = t->val[mdx(t, col, col)];
    for (i = 0; i < t->rows; i++) {
        if (i == col) {
            continue;
        }
        factor = t->val[mdx(t, i, col)] / pivot;
        for (j = 0; j < t->cols; j++) {
            t->val[mdx(t, i, j)] -=
                factor * t->val[mdx(t, col, j)];
            v->val[mdx(v, i, j)] -=
                factor * v->val[mdx(v, col, j)];
        }
    }
}

static void
swaprows2(MATRIX_T * t, MATRIX_T * v, int row, int swap)
{
    int j;
    double temp;
    for (j = 0; j < t->cols; j++) {
        temp = t->val[mdx(t, row, j)];
        t->val[mdx(t, row, j)] = t->val[mdx(t, swap, j)];
        t->val[mdx(t, swap, j)] = temp;
        temp = v->val[mdx(v, row, j)];
        v->val[mdx(v, row, j)] = v->val[mdx(v, swap, j)];
        v->val[mdx(v, swap, j)] = temp;
    }
}

MATRIX_T *
m_muprow(int row, double c, MATRIX_T * a)
{
    int j, index;
    if (a == NULL) {
        mmerrcode = NULLARG;
        return a;
    }
    index = row * a->cols;
    for (j = 0; j < a->cols; j++) {
        a->val[index] *= c;
        index++;
    }
    return a;
}

double
m_det(MATRIX_T * a, double epsilon)
{
/* calculates the determinant of matrix a using Gaussian
 * elimination with partial pivot maximization */
    int row, col, swap, sign;
    double pivot, e_norm, det;
    MATRIX_T *t;
    if (a == NULL) {
        mmerrcode = NULLARG;
        return 0.0;
    }
    if (a->rows != a->cols) {
        mmerrcode = NOTSQUARE;
        return 0.0;
    }
    e_norm = m_e_norm(a);
    det = 1.0;
    sign = 1;

    /* allocate a "scratch" matrix to work with */
    if (!(t = m_new(a->rows, a->cols))) return 0.0;
    t = m_assign(t, a);

    /* for each row */
    for (row = 0; row < t->rows; row++) {
        /* find largest element below diagonal in column */
        swap = maxelementrow(t, row);
        /* swap rows to put largest element on pivot */
        if (swap != row) { 
            sign = -sign;
            swaprows(t, row, swap);
        }
        /* multiply running product of det by the pivot
         * element */
        pivot = t->val[mdx(t, row, row)];
        det *= pivot;
        if ((fabs(det) / e_norm) < epsilon) {
            return 0;          /* potentially singular
                                * matrix */
        }
        /* subtract a multiple of the pivot row from each
         * row (below the diagonal) to put 0's in all
         * elements of the pivot column below the diagonal */
        col = row;
        set_low_zero(t, col);
    }
    m_free(t);
    if (sign < 0)
        det = -det;
    return det;
}

static void
set_low_zero(MATRIX_T * t, int col)
{
    int i, j;
    double pivot, factor;
    pivot = t->val[mdx(t, col, col)];
    for (i = col + 1; i < t->rows; i++) {
        factor = t->val[mdx(t, i, col)] / pivot;
        for (j = col; j < t->cols; j++) {
            t->val[mdx(t, i, j)] -=
                factor * t->val[mdx(t, col, j)];
        }
    }
}

static void
swaprows(MATRIX_T * t, int row, int swap)
{
    int j;
    double temp;
    for (j = 0; j < t->cols; j++) {
        temp = t->val[mdx(t, row, j)];
        t->val[mdx(t, row, j)] = t->val[mdx(t, swap, j)];
        t->val[mdx(t, swap, j)] = temp;
    }
}

MATRIX_T *
m_solve(MATRIX_T * x, MATRIX_T * a, MATRIX_T * b,
        double epsilon)
{
/* solves linear equation Ax = b for x using matrix
 * inversion NOTE: no explicit error checking; (except for
 * allocation failures) all potential errors are checked in 
 * called functions */
    MATRIX_T *ainv = NULL;
    double det = 0.0;
    if (!(ainv = m_new(a->rows, a->cols))) {
        return x;
    }
    if (!(ainv = m_inverse(ainv, a, &det, epsilon)))
        return x;
    x = m_mup(x, ainv, b);
    m_free(ainv);
    return x;
}

MATRIX_T *
m_ecsolve(MATRIX_T * x, MATRIX_T * a, MATRIX_T * b,
          double epsilon)
{
/* solves linear equation Ax = b for x using matrix
 * inversion and a followup iterative approach for error
 * correction */
    MATRIX_T *ainv = NULL;
    MATRIX_T *bprime = NULL;
    MATRIX_T *adj = NULL;
    MATRIX_T *newx = NULL;
    MATRIX_T *newadj = NULL;
    MATRIX_T *err = NULL;
    int iteration;
    double adjenorm, newadjenorm, det;
    if (!(ainv = m_new(a->rows, a->cols))) {
        mmerrcode = ALLOCFAIL;
        goto ending;
    }
    if (!(bprime = m_new(b->rows, b->cols))) {
        mmerrcode = ALLOCFAIL;
        goto ending;
    }
    if (!(adj = m_new(x->rows, x->cols))) {
        mmerrcode = ALLOCFAIL;
        goto ending;
    }
    if (!(newx = m_new(x->rows, x->cols))) {
        mmerrcode = ALLOCFAIL;
        goto ending;
    }
    if (!(newadj = m_new(adj->rows, adj->cols))) {
        mmerrcode = ALLOCFAIL;
        goto ending;
    }
    if (!(err = m_new(x->rows, x->cols))) {
        mmerrcode = ALLOCFAIL;
        goto ending;
    }
    /* calculate the first try at a solution including
     * calculation of first adjustment */
    ainv = m_inverse(ainv, a, &det, epsilon);
    x = m_mup(x, ainv, b);
    bprime = m_mup(bprime, a, x);
    err = m_sub(err, b, bprime);
    adj = m_mup(adj, ainv, err);
    adjenorm = m_e_norm(adj);


    /* iteratively calculate new solutions while accuracy
     * improves do no more than 10 iterations to prevent a
     * runaway calculation */
    for (iteration = 0; iteration < MMAXITERATIONS; iteration++) {
        newx = m_add(newx, x, adj);
        bprime = m_mup(bprime, a, newx);
        err = m_sub(err, b, bprime);
        newadj = m_mup(newadj, ainv, err);
        newadjenorm = m_e_norm(newadj);
        /* this is a test to see if complete else clause
         * operates to break out of loop if no improvement
         * since previous iteration; otherwise try again */
        if (newadjenorm < adjenorm) {  /* still improving */
            adjenorm = newadjenorm;
            x = m_assign(x, newx);
            adj = m_assign(adj, newadj);
        } else {
            break;
        }
    }
  ending:
    m_free(err);
    m_free(newadj);
    m_free(newx);
    m_free(adj);
    m_free(bprime);
    m_free(ainv);
    return x;
}

