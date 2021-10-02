C Unleashed - Chapter 21 - Toolcraft

Here are my recommended compilation commands for
various common microcomputer compilers:

Unix/Linux/DJGPP:

gcc -Wall -ansi -pedantic -c -o foo.o foo.c [-lm]

Microsoft C (any 16-bit version including VC1.0 and 1.5):

cl -AL -W4 -Za -Zi -Od -c foo.c

(NB early versions don't support -W4, so use -W3 instead)

Microsoft C (any 32-bit version):

cl -W4 -Za -Zi -Od -c foo.c

Turbo C (any 16-bit version):

tcc -ml -A -c foo.c

Borland C (any 16-bit version):

bcc -ml -A -c foo.c

Borland C (any 32-bit version):

bcc32 -A -c foo.c

(Of course, none of these /links/ your code.)


Here's a quick summary of all the source in this
directory.

You may get a few warnings when compiling the code.
None are anything terrible, but one is rather confusing.
It happens in Microsoft VC5Pro, and it's to do with
floating point overflow on values such as 1.0 and -1.0.

You may consider multiplying by -1.0 to be a very
dangerous practice, in which case no doubt you will
treat this warning seriously.



limn.c is a standalone program which limits lines
to n characters. If you are on a UNIX system, use
indent instead.

hd.c is a standalone program (a hex dumper).

maxline.c is a standalone program. I didn't have
space to cover line metrics tools in the chapter,
but I included it here anyway in case you find
it useful.

tee.c is a standalone program which acts like a T-pipe
for standard output. It copies stdout, and directs one
copy to a file and the other to stdout.

sandra.c is a standalone program. It is designed
to be a generally useful tool, once you have mended
it! It will exhibit undefined behaviour under certain
conditions; this is for explanatory purposes.


DataGen.c is a standalone program; it is an
example of how to generate simple random test data.


quine.c is a standalone program which generates
its own source code.


The code generator program, GENARGS, comprises

dllist.h
dllist.c
argproc.c
genargs.c

The files dllist.h and dllist.c are copied from
Chapter 11 (data structures) for your convenience.
They contain a rudimentary Double-Linked List library.

Once you have built this program, you can regenerate
argproc.c automatically, using the files:


grammar
help

and the command

GENARGS grammar argproc.c logfile help

(do not specify the -internal switch on this occasion)





Further example datafiles:

genhelp - generic helpfile - somewhat terse
copygram - example grammar for a copying program
calcgram - example grammar for a calculator

GENARGS copygram copyit.c logcopy genhelp -internal

GENARGS calcgram calcit.c logcalc genhelp -internal






Richard Heathfield

10 January 2000

(the last year of the Second Millennium)


