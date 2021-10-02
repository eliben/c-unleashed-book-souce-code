/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/


/*
** This test driver is for a merge sort you have never heard about.
** It is a very important idea.  That's because we can now merge as
** many subfiles as we like in a single pass.  It could be made a lot
** better, but this file demonstrates the concept nicely.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "inteltyp.h"
#include "distribs.h"
#include "genproto.h"
#include "mtrand.h"

#include "barproto.h"

/*
** This is set to 40 megs, but it can be whatever you like.
*/
static const unsigned long max_buffer = 40000000L;

#define MAX_STR_LEN 8192        /* make this anything you like */

#define MAXLINES 1500000        /* max #lines in input file */

static char    *backup[MAXLINES]; /* Array to hold a subsection of input */
static FILE    *fout; /* the output file */

/*
   Read from stdin and create an array with one element
   per line. Return the number of lines.
 */

/* Your operating system will probably have some limit */
#define MAX_TEMP_FILES 256

/* How many partitions are there? */
static int      count = 0;

/* This object defines our set of ordered subsets */
/* We will sort the file in chunks.  The sorted   */
/* chunks are described here. */
static fileset  fset[MAX_TEMP_FILES] = {0};     /* about 2 megs */

/* Read the next item from a file set */
int             fgetitem(fileset * p)
{
    char           *pc;
    pc = fgets(p->buffer, sizeof(p->buffer), p->fin);
#ifdef _DEBUG
    if (!pc) {
        if (!feof(p->fin))
            puts(strerror(errno));
    }
#endif
    p->empty = (pc == NULL);
    return p->empty;
}


/*
** remove the smallest item from a fileset and indicate EOF
*/
char           *fdeletemin(fileset * p, char *end)
{
    if (p->empty || p->fin == NULL) {
        puts("Error! Deletion from empty fileset");
        exit(EXIT_FAILURE);
    }
    p->empty = *end = fgetitem(p);
    return p->buffer;
}

/*
** Shell-sort a list of filesets.  This is not sorting the data
** in the file.  We are sorting the fileset objects by their
** current smallest object (which will be the first item, since
** we are using sorted subsets of the original data.
*/
void            fshellsort(fileset fset[], size_t count)
{
    size_t          i,
                    inc,
                    j;
    fileset         tmp;
    char           *etmp;
    for (inc = count; inc > 0;) {
        for (i = inc; i < count; i++) {
            j = i;
            tmp = fset[i];
            etmp = tmp.buffer;
            while (j >= inc && (lt(etmp, fset[j - inc].buffer))) {
                fset[j] = fset[j - inc];
                j -= inc;
            }
            fset[j] = tmp;
        }                       /* calculate the next h-increment */
        inc = (size_t) ((inc > 1) && (inc < 5)) ? 1 : 5 * inc / 11;
    }
}


/*
** Normalize is needed after we remove an item to ensure that the
** set is still ordered.  This will take O(log(q)) operations, where
** q is the number of filesets [NOT the number of data items].
*/
void            fnormalize(fileset * fset, size_t count)
{
    long            beg;        /* search beginning here (this moves toward
                                 * ipg) */
    long            ipg;        /* current guess for the insertion point */
    long            end;        /* search ending here (this moves toward ipg) */
    fileset         temp;       /* hold one fileset in temporary storage */
    long            i;

    char           *mcguffin = fset[0].buffer;
    /* maybe we don't need to do anything (i'm an optimist) */
    if (count < 2 || le(mcguffin, fset[1].buffer))
        return;

    /* inline binary search to find point of insertion */
    beg = ipg = 1;              /* the first element of the ordered part of
                                 * the data is element 0 */
    end = count - 1;            /* the last element already ordered is
                                 * element fileset */
    /* without this check, loop terminates only if an equal element is
     * already sorted */
    while (end >= beg) {
        /* insertion point guess of halfway between beginning and ending */
        ipg = ((end + beg) >> 1);
        if (ge(fset[ipg].buffer, mcguffin))
            end = ipg - 1;
        else
            beg = ++ipg;
    }
    /* make room at fset[ipg] for fset[0] */
    temp = fset[0];             /* save the new element we are ready to
                                 * insert */
    for (i = 0; i < ipg; i++)
        fset[i] = fset[i + 1];
    fset[ipg - 1] = temp;       /* put the new element in its sorted order */
    return;
}

/*
** Is a string greater than or equal to another one?
*/
int             ge(char *l, char *r)
{
    return (strcmp(l, r) >= 0);
}

/*
** Is a string less than or equal to another one?
*/
int             le(char *l, char *r)
{
    return (strcmp(l, r) <= 0);
}

/*
** Is a string strictly less than or equal to another one?
*/
int             lt(char *l, char *r)
{
    return (strcmp(l, r) < 0);
}

/*
** Read a block of lines from a file.
*/
static int      readlines(char *file_name, char *lines[], int maxlines, size_t * offset)
{
    int             nlines = 0;
    size_t          size;
    static size_t   limit;
    char           *newline;
    static FILE    *in_file;
    static char    *basep,
                   *cur;

    if (*offset == 0) {
        if (!(in_file = fopen(file_name, "rb"))) {
            perror(file_name);
                        printf("Error opening file %s\n", file_name);
            exit(EXIT_FAILURE);
        }
        fseek(in_file, 0, SEEK_END);
        size = ftell(in_file) + 10000;
        limit = size - *offset > max_buffer ? max_buffer : size - *offset;
        fseek(in_file, *offset, SEEK_SET);
        if (!(basep = calloc((limit + 1), 1)))
            return -1;
    }
    fseek(in_file, *offset, SEEK_SET);
    cur = basep;
    while (fgets(cur, limit - (cur - basep), in_file)) {
        lines[nlines] = cur;
        if ((newline = strchr(lines[nlines], '\n'))) {
            cur = newline + 2;
        } else {
            puts("Error -- lines in text in file should end in newline");
            exit(EXIT_FAILURE);
        }
        nlines++;
        if (nlines == maxlines || limit - (cur - basep) < MAX_STR_LEN) {
            *offset = ftell(in_file);
            break;
        }
    }
    if (feof(in_file))
        *offset = 0;
    return nlines;
}
/*
** Write the subset of the original data held in array t.
*/
void            writelines(char *t[], int nlines, FILE * fout)
{
    int             i;

    for (i = 0; i < nlines; i++)
        if (fprintf(fout, "%s", t[i]) < 0)
                {
                        puts("Error writing data to output file.");
                        exit(EXIT_FAILURE);
                }
}

/*
** Test driver
*/
int             main(int argc, char *argv[])
{
    int             nlines;
    int             i;
    char            end = 0;
    char           *e;
    size_t          offset = 0;
    char           *name;
    int             savecount;
    fileset        *fs = fset;
    mtsrand(4357U);
    if (argc != 3) {
        fprintf(stderr, "Usage: %s input_file output_file\n", argv[0]);
        return 1;
    }
    fout = fopen(argv[2], "wb");
    if (fout == NULL) {
        printf("Error!  Count not open %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\nFile: %s\n", argv[1]);

    do {
        /* Read a block from the input file */
        if ((nlines = readlines(argv[1], backup, MAXLINES, &offset)) >= 0) {
            /* Sort that block we just read */
            Iqsort5_str(backup, nlines);
#ifdef _DEBUG
            if (!InSort_str(backup, nlines)) {
                puts("Error in algorithm!!!");
                exit(EXIT_FAILURE);
            }
#endif
            /* Write the sorted block to disk */
            if ((name = tmpnam(NULL)) != NULL) {
                strcpy(fset[count].filename, name);
                fset[count].fin = fopen(fset[count].filename, "wt");
                writelines(backup, nlines, fset[count].fin);
                count++;
            } else {
                puts("Error creating output file");
                exit(EXIT_FAILURE);
            }
        }
    /* Loop until the file is empty */
    } while (offset > 0);

    /* flush to disk all open files */
    if (fflush(NULL) == EOF)
        {
                puts("Error.  Unable to flush temporary files to disk.");
                exit(EXIT_FAILURE);
        }
    /* Close temp files (originally opened in write mode)
    ** and reopen in read mode.  Then get the first item.
    */
    for (i = 0; i < count; i++) {
        if (fclose(fset[i].fin) == EOF)
                {
            printf("Error closing file %s\n", fset[i].filename);
            puts(strerror(errno));
                exit(EXIT_FAILURE);
                }
        fset[i].fin = fopen(fset[i].filename, "rt");
                if (fset[i].fin == NULL)
                {
            printf("Error opening file %s\n", fset[i].filename);
            puts(strerror(errno));
                exit(EXIT_FAILURE);
                }
        fseek(fset[i].fin, 0, SEEK_SET);
        fgetitem(&fset[i]);
    }
    /* Shell sort our partitions.  If the number is huge, perhaps
    ** quick-sort or radix-sort should be used instead.
    */
    fshellsort(fset, count);
    savecount = count; /* Remember how many paritions we had */

    /* Merge the partitions using our strange priority queue */
    while (count > 0) {
        e = fs[0].buffer;
        fprintf(fout, "%s", fs[0].buffer);
        fdeletemin(fs, &end);
        if (end) {
            fs++;
            count--;
        }
        fnormalize(fs, count);
    }
    /* Flush all open files to disk */
    /* Close and remove all temp files */
    for (i = 0; i < savecount; i++) {
        if (fclose(fset[i].fin) == EOF)
                {
            printf("error closing file %s\n", fset[i].filename);
            puts(strerror(errno));
                }
        if (remove(fset[i].filename) != 0) {
            printf("unable to delete file %s\n", fset[i].filename);
            puts(strerror(errno));
        }
    }
    return 0;
}
