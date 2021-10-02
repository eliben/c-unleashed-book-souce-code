/*  cfollow.h - trace macros
 *
 *  CFollow - Code Tracing Library
 *
 *  Copyright (C) 2000  Richard Heathfield,
 *                      Eton Computer Systems Ltd,
 *                      Macmillan Computer Publishing
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Richard Heathfield may be contacted at binary@eton.powernet.co.uk
 *
 */

#ifndef CFOLLOW_H__
#define CFOLLOW_H__

#define CFOLLOW_MAXLINE 100000L /* Max lines that will
                                 * appear in trace file
                                 */

/* Tracing is turned off by default. To enable it, you
 * could change #if 0 to #if 1 (below) but a better
 * way, if available, is to use your compiler's command
 * line #define capability.
 * Typically, -DCFOLLOW_ON__ would be the Right Thing,
 * but of course it may be different for your compiler.
 */

#if 0
#define CFOLLOW_ON__
#endif

#ifdef CFOLLOW_ON__

/* S for string */
#define S_FOLLOW(svar) CFollow(__FILE__,\
                               __LINE__,\
                               0,\
                               #svar"=[%s]",\
                               svar)

/* A for array (of char, not necessarily terminated) */
#define A_FOLLOW(avar, bytes) CFollow(__FILE__,\
                                      __LINE__,\
                                      0,\
                #avar"(first %d bytes) =[%*.*s]",\
                                      (int)bytes,\
                                      (int)bytes,\
                                      (int)bytes,\
                                      avar)
/* I for Int */
#define I_FOLLOW(ivar) CFollow(__FILE__,\
                               __LINE__,\
                               0,\
                               #ivar"=[%d]",\
                               ivar)
/* U for Unsigned int */
#define U_FOLLOW(uvar) CFollow(__FILE__,\
                               __LINE__,\
                               0,\
                               #uvar"=[%u]",\
                               uvar)
/* HI for sHort Int */
#define HI_FOLLOW(sivar) CFollow(__FILE__,\
                                 __LINE__,\
                                 0,\
                                 #sivar"=[%hd]",\
                                 sivar)
/* HU sHort Unsigned int */
#define HU_FOLLOW(suvar) CFollow(__FILE__,\
                                 __LINE__,\
                                 0,\
                                 #suvar"=[%hu]",\
                                 suvar)
/* LI for Long Int */
#define LI_FOLLOW(lvar) CFollow(__FILE__,\
                                __LINE__,\
                                0,\
                                #lvar"=[%ld]",\
                                lvar)
/* LU for long unsigned int */
#define LU_FOLLOW(ulvar) CFollow(__FILE__,\
                                 __LINE__,\
                                 0,\
                                 #ulvar"=[%lu]",\
                                 ulvar)

/* D for Double */
#define D_FOLLOW(dvar) CFollow(__FILE__,\
                               __LINE__,\
                               0,\
                               #dvar"=[%f]",\
                               dvar)
/* B for Boolean condition (true/false) */
#define B_FOLLOW(cond) CFollow(__FILE__,\
                               __LINE__,\
                               0,\
                               "Condition "#cond" = %s",\
                               (cond) ? "TRUE" : "FALSE")
/* P for pointer */
#define P_FOLLOW(ptr) CFollow(__FILE__,\
                              __LINE__,\
                              0,\
                              "Pointer "#ptr" = %p",\
                              (void *)(ptr))
/* F for user-specified Format
 * integer suffix indicates
 * number of extra args
 */
#define F_FOLLOW0(s) CFollow(__FILE__,\
                             __LINE__,\
                             0,\
                             s)
#define F_FOLLOW1(format, a) \
                        CFollow(__FILE__,\
                                __LINE__,\
                                0,\
                                format,\
                                a)
#define F_FOLLOW2(format, a, b) \
                        CFollow(__FILE__,\
                                __LINE__,\
                                0,\
                                format,\
                                a, b)
#define F_FOLLOW3(format, a, b, c) \
                        CFollow(__FILE__,\
                                __LINE__,\
                                0,\
                                format,\
                                a, b, c)
#define F_FOLLOW4(format, a, b, c, d) \
                        CFollow(__FILE__,\
                                __LINE__,\
                                0,\
                                format,\
                                a, b, c, d)
#define F_FOLLOW5(format, a, b, c, d, e) \
                        CFollow(__FILE__,\
                                __LINE__,\
                                0,\
                                format,\
                                a, b, c, d, e)

#define CF_CODE(code) code
#define CF_FUNCIN(funcname) CFollow(__FILE__,\
                                    __LINE__,\
                                    1,\
         "Function %s() entry point.",\
                                    #funcname)

#define CF_FUNCOUT(funcname, typespec, rval) \
                    CFollow(__FILE__,\
                            __LINE__,\
                            -1,\
         "Function %s() returns [%"#typespec"].",\
                            #funcname,\
                            rval)
#define CFOLLOW_CLOSEDOWN CFollow(NULL, 0, 0, NULL)

void CFollow(char *FileName,
             int   LineNumber,
             int   DepthModifier,
             char *FormatString,
             ...);

#else

#define S_FOLLOW(svar)
#define A_FOLLOW(avar, bytes)
#define I_FOLLOW(ivar)
#define U_FOLLOW(uvar)
#define HI_FOLLOW(sivar)
#define HU_FOLLOW(suvar)
#define LI_FOLLOW(lvar)
#define LU_FOLLOW(ulvar)
#define D_FOLLOW(dvar)
#define B_FOLLOW(cond)
#define P_FOLLOW(ptr)
#define F_FOLLOW0(s)
#define F_FOLLOW1(format, a)
#define F_FOLLOW2(format, a, b)
#define F_FOLLOW3(format, a, b, c)
#define F_FOLLOW4(format, a, b, c, d)
#define F_FOLLOW5(format, a, b, c, d, e)

#define CF_CODE(code) /* no code! */
#define CF_FUNCIN(funcname)
#define CF_FUNCOUT(funcname, typespec, rval)

#define CFOLLOW_CLOSEDOWN

#endif /* Trace statements are on/off */

#endif /* CFOLLOW_H__ */

