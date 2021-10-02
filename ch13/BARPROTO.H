/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/


/*
** Definitions for our priority queue model merge sort.
*/
typedef struct tag_fileset {
    FILE           *fin;
    char            empty;
    char            filename[FILENAME_MAX];
    char            buffer[8192];       /* max record size */
}               fileset;

extern int      fgetitem(struct tag_fileset * p);
extern char    *fdeletemin(struct tag_fileset * p, char *end);
extern void     fshellsort(struct tag_fileset * fset, unsigned int count);
extern void     fnormalize(struct tag_fileset * fset, unsigned int count);
extern void     foutputr(char *e);
extern int      ge(char *l, char *r);
extern int      le(char *l, char *r);
extern int      lt(char *l, char *r);
static int      readlines(char *file_name, char **lines, int maxlines, unsigned int *offset);
extern void     writelines(char **t, int nlines, FILE * fout);
extern int      main(int argc, char **argv);
