/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/


/*
** Header files for Mersenne Twister pseudo-random number generator.
*/
extern void     mtsrand(unsigned long seed);
extern unsigned long reloadMT(void);
extern unsigned long mtrand(void);
