C Unleashed - Chapter 11 - Classical Data Structures

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
directory. Several of the source files are used in
more than one program. Some of the source files
are mere fragments, not intended for compilation.

You may get a few warnings when compiling the code.
None are anything terrible, but one is rather confusing.
It happens in Microsoft VC5Pro, and it's to do with
floating point overflow on values such as 1.0 and -1.0.

You may consider multiplying by -1.0 to be a very
dangerous practice, in which case no doubt you will
treat this warning seriously.


Listings C11_001 through CL11_009 are not expected to
compile. They are code fragments, not programs.

Listing C11_010 is a standalone program.

Listing C11_011 is a code fragment, and will not
compile. It's a bug illustration anyway.

Listing C11_012 will compile, but pointlessly. It's
not intended to be executable.

Listings C11_013 and C11_014 are code fragments,
which will not compile.

Listing C11_015 is a complete standalone program.
It is intended to be a demonstration only.

Listing C11_016 will compile, but will not link
unless you write a driver for it. It is intended
purely as an illustration.

*** Update 8 June 2000 ***

Listings C11_015 and C11_016 should be treated as
interesting curiosities, at best. They use a non-
portable assumption about type alignment which
means they invoke undefined behaviour. Bottom line,
they might work on forgiving platforms, but you
can't trust them as far as you can spit them.

My apologies for letting them remain in the paper
text. I spotted my mistake too late to change the
book itself.

(NB always own up to your mistakes. Your sins *will*
find you out, so you might as well take the credit
for being honest about them...)

***    RJH 8/6/2000    ***

Listing C11_017 is a complete standalone program.
It is intended to be a demonstration only.

Listing C11_018 is a project comprising the following files:

C11_018.c
strarr.h
strarr.c

The string array functionality in strarr.c is by
no means exhaustive, but is re-usable in other
programs.
	 
Listing C11_019 is a code fragment. It compiles, but
is intended only for illustration. It contains no error-
checking code, and should not be used 'as is'.
 
Listing C11_020 is a complete standalone program.
It is intended to be a demonstration only.

Listing C11_021 is a complete standalone program.
It is intended to be a demonstration only.

Listing C11_022 is a complete standalone program.
It is intended to be a demonstration only.

Listing SLLISTMN is a project comprising the following files:

sllistmn.c
sllist.h
sllist.c

The SLLIST library code is designed to be re-usable in other projects.

Listing DLLISTMN is a project comprising the following files:

dllistmn.c
dllist.h
dllist.c

The DLLIST library code is designed to be re-usable in other projects.

Listing DLLISTEG is a project comprising the following files:

dllisteg.c
dllist.h
dllist.c

Listing CLISTMN is a project comprising the following files:

clistmn.c
clist.h
clist.c

The CLIST library code is designed to be re-usable in other projects.

Listing CLISTMN2 is a project comprising the following files:

clistmn2.c
clist.h
clist.c

Listing STACKMN is a project comprising the following files:

stackmn.c
stack.h
stack.c
sllist.h
sllist.c
strarr.h
strarr.c

The STACK library code is designed to be re-usable in other projects.

Listing QUEUEMN is a project comprising the following files:

queuemn.c
queue.h
queue.c
sllist.h
sllist.c

The QUEUE library code is designed to be re-usable in other projects.

Listing HEAPMN is a project comprising the following files:

heapmn.c
heap.h
heap.c

The HEAP library code is designed to be re-usable in other projects.

Listing DEQUEMN is a project comprising the following files:

dequemn.c
deque.h
deque.c
dllist.h
dllist.c

The DEQUE library code is designed to be re-usable in other projects.



Richard Heathfield
binary@eton.powernet.co.uk

21 December 1999


