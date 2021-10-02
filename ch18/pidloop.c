/*********************************************************
 *
 *  pidloop.c - contains code to simulate PID control
 *              loops
 *
 *  Copyright (C) 2000  Jack Klein
 *                      Macmillan Computer Publishing
 *
 *  This program is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU General
 *  Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A
 *  PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General
 *  Public License along with this program; if not, write
 *  to the Free Software Foundation, Inc., 675 Mass Ave,
 *  Cambridge, MA 02139, USA.
 *
 *  Jack Klein may be contacted by email at:
 *     The_C_Guru@yahoo.com
 *
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/*********************************************************
 *
 * Data Type:   PID_PARAMS structure
 *
 * Contents:    all values used by the PID simulation,
 *              plus a cycle count
 *
 ********************************************************/
typedef struct
{
  double p_gain;      /* 'P' proportional gain          */
  double i_gain;      /* 'I' integral gain              */
  double d_gain;      /* 'D' derivative gain            */
  double acc_ff;      /* 'A' acceleration feed forward  */
  double fri_ff;      /* 'F' friction feed forward      */
  double vel_ff;      /* 'V' velocity feed forward      */
  double hold;        /* 'H' output Hold                */
  double bias;        /* 'B' bias                       */
  double accel;       /* 'R' acceleration rate          */
  double setpt;       /* 'S' set point                  */
  double trans;       /* 'T' transfer ratio             */
  double lag;         /* 'L' lag in output change       */
  double min;         /* 'N' minimum output value       */
  double max;         /* 'M' maximum output value       */
  double cycles;      /* 'Y' repeat cycle count         */
  double slew;        /* 'W' maximum slew rate          */
} PID_PARAMS;

static PID_PARAMS params =
{
   0.00,              /* 'P' proportional gain          */
   0.00,              /* 'I' integral gain              */
   0.00,              /* 'D' derivative gain            */
   0.00,              /* 'A' acceleration feed forward  */
   0.00,              /* 'F' friction feed forward      */
   0.00,              /* 'V' velocity feed forward      */
   0.00,              /* 'H' output Hold                */
   0.00,              /* 'B' bias                       */
   0.00,              /* 'R' acceleration rate          */
   0.00,              /* 'S' set point                  */
   0.00,              /* 'T' transfer ratio             */
   0.00,              /* 'L' lag in output change       */
   0.00,              /* 'N' minimum output value       */
   0.00,              /* 'M' maximum output value       */
   0.00,              /* 'Y' repeat cycle count         */
   0.00,              /* 'W' slew limit                 */
};

/*********************************************************
 *
 * Data Type:   PARAM_PARSE structure
 *
 * Contents:    values used for entering, validating, and
 *              displaying the values in the PID_PARAMS
 *              structure type
 *
 ********************************************************/
typedef struct
{
  int token;
  double *destination;
  double min_val;
  double max_val;
  int force_to_int;
  char *param_name;
} PARAM_PARSE;

static const PARAM_PARSE parse [] =
{
  { 'A', &params.acc_ff, -100.0, 100.0,
        0, "Acceleration feed forward" },
  { 'B', &params.bias,  0.00, 100.0,
        0, "constant Bias" },
  { 'D', &params.d_gain, 0.00, 100.0,
        0, "Derivative gain" },
  { 'F', &params.fri_ff, 0.00, 100.0,
        0, "Friction feed forward" },
  { 'H', &params.hold, 0.00, 100.0,
        1, "output Hold" },
  { 'I', &params.i_gain, 0.00, 100.0,
        0, "Integral gain" },
  { 'L', &params.lag,  0.00, 1.0,
        0, "response Lag" },
  { 'M', &params.max,  0, 100.0,
        0, "Maximum output" },
  { 'N', &params.min,  -100.0, 0,
        0, "miNimum output" },
  { 'P', &params.p_gain, 0.00, 100.0,
        0, "Proportional gain" },
  { 'R', &params.accel, 0.00, 100.0,
        0, "acceleration Rate" },
  { 'S', &params.setpt,  -10000, 10000,
        1, "Set point" },
  { 'T', &params.trans,  0.10, 100,
        0, "Transfer ratio" },
  { 'V', &params.vel_ff, 0.00, 100,
        0, "Velocity feed forward" },
  { 'W', &params.slew,  0.00, 100.0,
        0, "sleW limit" },
  { 'Y', &params.cycles,  0.00, 99999999.0,
        0, "cYcle count" }
};

/*********************************************************
 *
 * Data Type:   unnamed enumeration
 *
 * Contents:    specifies the offsets of the parse[] array
 *              in order for use by ShowParams()
 *
 ********************************************************/
enum
{
  ACC_FF,
  BIAS,
  D_GAIN,
  FRI_FF,
  HOLD,
  I_GAIN,
  LAG,
  MAX,
  MIN,
  P_GAIN,
  ACCEL,
  SETPT,
  TRANS,
  VEL_FF,
  SLEW,
  CYCLES
};

#define MAX_EVENTS 100

/*********************************************************
 *
 * Data Type:   EVENT structure
 *
 * Contents:    holds events set by reading an input file,
 *              to be processed at specific times during
 *              execution of the simulation
 *
 ********************************************************/
typedef struct
{
  double            *event_dest;
  double             event_value;
  unsigned long      event_time;
  int                event_token;
} EVENT;

static EVENT events[MAX_EVENTS];

/*********************************************************
 *
 * File Scope Variables
 *
 *              event_index is shared by the ParseEvent()
 *              and CheckEvent() functions
 *              
 *              this_target and next_target would normally
 *              be internal to the ComputePID() function,
 *              but have file scope so main() can output
 *              them
 *
 *              rms_error is computed by ComputePID() and
 *              output at the end of the simulation by
 *              main()
 *
 ********************************************************/
static int          event_index             = 0;

static double       this_target             = 0.0;
static double       next_target             = 0.0;

static double       rms_error               = 0.0;

/*********************************************************
 *
 * Function:    ShowMenuItem
 *
 * Parameters:  const PARAM_PARSE *pp, pointer to a structure
 *              describing a PID_PARAMS member
 *
 * Returns:     nothing
 *
 * Description: displays the parameter and its current value
 *
 ********************************************************/
static void
ShowMenuItem(const PARAM_PARSE *pp)
{
  int     count;
  int     padding     = 28 - (int)strlen(pp->param_name);
  
  fprintf(stderr, "    [%c]  %s", pp->token, pp->param_name);

  for (count = 0; count < padding; ++count)
  {
    putc(' ', stderr);
  }

  if (pp->force_to_int)
  {
    fprintf(stderr, "%9.0f\n", *pp->destination);
  }
  else
  {
    fprintf(stderr, "%9.4f\n", *pp->destination);
  }
}

/*********************************************************
 *
 * Function:    ShowParams
 *
 * Parameters:  none
 *
 * Returns:     nothing
 *
 * Description: shows all parameters in a formatted
 *              display
 *
 ********************************************************/
static void
ShowParams(void)
{
  fputs("\nCurrent Parameter Values\n", stderr);
  ShowMenuItem(parse + P_GAIN);
  ShowMenuItem(parse + I_GAIN);
  ShowMenuItem(parse + D_GAIN);
  ShowMenuItem(parse +   HOLD);
  ShowMenuItem(parse + ACC_FF);
  ShowMenuItem(parse + FRI_FF);
  ShowMenuItem(parse + VEL_FF);
  ShowMenuItem(parse +   BIAS);
  ShowMenuItem(parse +  ACCEL);
  ShowMenuItem(parse +  SETPT);
  ShowMenuItem(parse +  TRANS);
  ShowMenuItem(parse +    LAG);
  ShowMenuItem(parse +    MIN);
  ShowMenuItem(parse +    MAX);
  ShowMenuItem(parse +   SLEW);
  ShowMenuItem(parse + CYCLES);
  fputs("\nEnter letter to change value\n\n", stderr);
}

/*********************************************************
 *
 * Function:    ClearStdin
 *
 * Parameters:  none
 *
 * Returns:     EOF or '\n'
 *
 * Description: this function reads and discards any
 *              characters remaining in stdin, up until
 *              a '\n' is discarded or EOF is found, should
 *              only be used when it is known that
 *              there is left-over input to be discarded
 *
 ********************************************************/
static int
ClearStdin(void)
{
  int               ch;
  do
  {
    ch = getchar();
  } while (ch != EOF && ch != '\n');

  return ch;
}

/*********************************************************
 *
 * Function:    SkipWhiteSpace
 *
 * Parameters:  const char *cp, a C string (array of chars
 *              terminated with '\0'
 *
 * Returns:     pointer to first non white space character
 *              if any in the string, otherwise NULL
 *
 * Description: this function is used to skip over leading
 *              white space in a string
 *
 ********************************************************/
static char
*SkipWhiteSpace(const char *cp)
{
  unsigned int      ch;

  for ( ; (ch = *(unsigned char *)cp) != 0; ++cp)
  {
    if (!isspace(ch))
    {
      return (char *)cp;
    }
  }
  return NULL;
}

/*********************************************************
 *
 * Function:    Compare
 *
 * Parameters:  const void *ip, actually pointer to an int,
 *              should contain upper case alpha character
 *
 *              const void *pp, actually pointer to a
 *              PARAM_PARSE structure
 *
 * Returns:     0 if the int matches the token in
 *              the structure
 *
 *              < 0 if the int is less than the token
 *              in the structure
 *
 *              > 0 if the int is larger than the token
 *              in the structure
 *
 * Description: this function is not called direcely but
 *              instead passed to bsearch() as the comparison
 *              function for locating a PARAM_PARSE
 *              structure matching a specific input character
 *
 ********************************************************/
static int
Compare(const void *ip, const void *pp)
{
  return *(int *)ip - ((PARAM_PARSE *)pp)->token;
}

/*********************************************************
 *
 * Function:    EventCompare
 *
 * Parameters:  const void *p1, actually a pointer to an
 *              EVENT structure
 *
 *              const void *p2, actually another pointer
 *              to an EVENT structure
 *
 * Returns:     0 if event_time member of both structures
 *              are the same
 *
 *              -1 of the event_time member of the first
 *              structure is earlier (lower numerically)
 *
 *              +1 if the event_time member of the first
 *              structure is later (higher numerically)
 *
 * Description: this function is not called direcely but
 *              instead passed to qsort() as the comparison
 *              function for sorting the array of entered
 *              events in ascending chronological order
 *
 ********************************************************/
static int
EventCompare(const void *p1, const void *p2)
{
  
  unsigned long     u1        = ((EVENT *)p1)->event_time;
  unsigned long     u2        = ((EVENT *)p2)->event_time;
  
  if (u1 < u2)
  {
    return -1;
  }
  else if (u1 > u2)
  {
    return 1;
  }

  return 0;
}

/*********************************************************
 *
 * Function:    ParseValue
 *
 * Parameters:  const PARAM_PARSE pp, pointer to a structure
 *              containing the acceptable limits for a
 *              PID parameter value
 *
 *              const char *cp, points to a C string,
 *              array of chars terminated by '\0'
 *
 *              const double *d, pointer to a double where
 *              the value represented by the text string will
 *              be stored if it is witing range
 *
 * Returns:     zero if a successful conversion is made and
 *              stored, non zero if not
 *
 * Description: validates, converts, and stores a value
 *
 ********************************************************/
static int
ParseValue(const PARAM_PARSE *pp, const char *cp, double *d)
{
  char              *end_ptr;
  double            val;
  
  /* see if strtod() can convert to a double          */
  val = strtod(cp, &end_ptr);
  
  /* strtod() sets end_ptr to the input parameter if  */
  /* it could not perform a conversion                */
  if (end_ptr == cp)
  {
    fprintf(stderr, "error: non number entered for '%c'\n",
            pp->token);
    return 1;
  }

  /* did convert a value, check it for range          */
  if ((val < pp->min_val) || (val > pp->max_val))
  {
    fprintf(stderr, "error: %0.4f out of range for '%c'\n",
            val, pp->token);  
    return 1;
  }
  
  /* see if the value should be forced to an integer  */
  if (pp->force_to_int)
  {
    val = (long)val;
  }
  
  /* value is valid, build a pointer to store it      */
  *d = val;
  return 0;
}

/*********************************************************
 *
 * Function:    FindParseParam
 *
 * Parameters:  const char *cp, points to a C string,
 *              array of chars terminated by '\0'
 *
 * Returns:     NULL if the string does not start with the
 *              sequence of a letter followed by an equal
 *              sign, or the letter does not match one
 *              of the PARAM_PARSE structures in the array
 *
 *              if there is a match, a pointer to the
 *              matching PARAM_PARSE pointer is returned
 *
 * Description: find a match for a properly formed string
 *              which indicates a parameter to be changed
 *
 ********************************************************/
static PARAM_PARSE
*FindParseParams(const char *cp)
{
  PARAM_PARSE       *pp                     = NULL;
  int               upper;

  /* must have at least 2 chars for token and '='     */
  if (cp && strlen(cp) >= 2)
  {
    /* make all letters uppercase for comparison      */
    upper = toupper(*(unsigned char *)cp);

    /* converted copy of first character must be an   */
    /* upper case letter and the second character     */
    /* must be an equal sign                          */
    if (isupper(upper) && '=' == cp[1])
    {
      /* look for a match in the parse[] array        */
      pp = bsearch(&upper, parse,
                   sizeof parse / sizeof *parse,
                   sizeof *parse, Compare);
    }
  }
  return pp;
}

/*********************************************************
 *
 * Function:    ParseEvent
 *
 * Parameters:  const char *cp, pointing to a string
 *              beginning with the character '@'
 *
 * Returns:     zero if an event was successfully parsed and
 *              stored, non zero if not
 *
 * Description: if there is space remaining in the events
 *              array, parses the string for a pattern of
 *
 *              "@#####[optional white space]letter=#####" 
 *              
 *              where the first "#####' is convertable into
 *              an unsigned long value representing the cycle
 *              count at which the event is to take effect,
 *              and the "letter=#####" is an ordinary parameter
 *              setting, or is 'q' or 'Q' for quit
 *
 ********************************************************/
static int
ParseEvent(const char *cp)
{
  char              *end_ptr;
  PARAM_PARSE       *pp;
  EVENT             ev                      = { 0 };

  /* make sure of room to store the event             */
  if (event_index >= MAX_EVENTS)
  {
    return 1;
  }

  /* now get the time value after the '@'             */
  ev.event_time = strtoul(cp + 1, &end_ptr, 10);
  
  /* check for errors, strtoul unable to perform a    */
  /* conversion or the value is 0                     */
  if (end_ptr == cp || 0 == ev.event_time)
  {
    return 1;
  }

  /* skip any white space between time and token      */
  if (NULL == (end_ptr = SkipWhiteSpace(end_ptr)))
  {
     return 1;
  }

  /* now see if the remaining string contains a valid */
  /* parameter specifier                              */
  if (NULL == (pp = FindParseParams(end_ptr)))
  {
    /* it might still be a 'q' or 'Q' for a command   */
    /* to quit the program                            */
    if ('q' == *end_ptr || 'Q' == *end_ptr)
    {
      /* it is a quit command, fill in the token and  */
      /* store it, return successful indication       */
      ev.event_token = 'Q';
      events[event_index++] = ev;
      fputs("Stored Quit Event\n", stderr);
      return 0;
    }
    return 1;
  }

  if (ParseValue(pp, end_ptr + 2, &ev.event_value))
  {
    return 1;
  }

  /* ev.event_time and ev.event_value contain valid   */
  /* values, ev_event_token is 0, just need to fill    */
  /* in the event_dest value from the pp structure    */
  ev.event_dest = pp->destination;
  ev.event_token = toupper(*(unsigned char *)end_ptr);
  events[event_index++] = ev;
  fprintf(stderr, "Event @%lu, value %f, to %l\n",
      ev.event_time, ev.event_value, 
      (long)((double *)ev.event_dest - &params.p_gain));
  return 0;
}

/*********************************************************
 *
 * Function:    ParseImmediate
 *
 * Parameters:  const char *cp, pointing to a string
 *              beginning with a latter, to be checked
 *              for containing a valid parameter setting
 *
 * Returns:     zero if a parameter setting was successfully
 *              parsed and stored, non zero if not
 *
 * Description: test for a properly formatted parameter
 *              setting, then verifies that the value
 *              is within range, and sets the parameter
 *              if so
 *
 ********************************************************/
static int
ParseImmediate(const char *cp)
{
  PARAM_PARSE       *pp;

  /* now see if the remaining string contains a valid */
  /* parameter specifier                              */
  if (NULL == (pp = FindParseParams(cp)))
  {
      return 1;
  }

  if (ParseValue(pp, cp + 2, pp->destination))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/*********************************************************
 *
 * Function:    ParseParams
 *
 * Parameters:  const char *cp, a command line argument
 *              assumed to be the name of a file containing
 *              immediate parameter settings and timed
 *              events
 *
 * Returns:     nothing
 *
 * Description: attempts to used the string to open a file
 *
 *              if successful, the file is read and
 *              parameter settings and timed events are
 *              parsed and stored
 *
 ********************************************************/
static void
ParseParams(const char *fname)
{
  FILE              *fin;
  char              inbuff[1000];
  int               upper;

  if (NULL == (fin = fopen(fname, "r")))
  {
    fprintf(stderr, "error: can't open %s\n", fname);
    return;
  }

  while (NULL != (fgets(inbuff, sizeof inbuff, fin)))
  {
    upper = toupper(*(unsigned char *)inbuff);
    
    /* check for an event command                         */
    if ('@' == upper)
    {
      if (ParseEvent(inbuff))
      {
        fprintf(stderr, "bad event: %s", inbuff);
      }
    }
    else if (isupper(upper))
    {
      if (ParseImmediate(inbuff))
      {
        fprintf(stderr, "bad param: %s", inbuff);
      }
    }
  } /* end of the while loop, finished reading file       */

  fclose(fin);

  /* must sort the events array into time order if any    */
  /* were entered                                         */
  if (event_index > 1)
  {
    qsort(events, event_index, sizeof *events, EventCompare);
  }

  event_index = 0;
  
  for (upper = 0; upper < event_index; ++upper)
  {
    fprintf(stderr, "%3d: time %6lu ", upper, events[upper].event_time);
    if ('Q' == events[upper].event_token)
    {
      fprintf(stderr, "Q = Quit!\n");
    }
    else
    {
      fprintf(stderr, "%c %f to %p\n", events[upper].event_token,
              events[upper].event_value,
              (void *)events[upper].event_dest);
    }
  }

  fprintf(stderr, "Set Point @ %p\n", (void *)&params.setpt);
  fprintf(stderr, "Cycles    @ %p\n", (void *)&params.cycles);
}

/*********************************************************
 *
 * Function:    CheckEvents
 *
 * Parameters:  unsigned long count, the count of cycles
 *              executed in the simulation so far
 *
 * Returns:     1 if the simulation should continue to run,
 *              0 if the program should exit
 *
 * Description: controls the process of the simulation
 *              when not running a preset number of cycles
 *
 *              on each iteration it checks for timed
 *              events ready to be processed
 *
 *              returns 0 if a 'Q' event is processed,
 *              otherwise 1 after processing all events
 *              whose time has come
 *
 ********************************************************/
static int
CheckEvents(unsigned long count)
{
  int               quit                    = 0;
  
  /* except for the very first pass, check for timed  */
  /* events ready to be executed                      */
  if (0 != count)
  {
    /* look for events with lower cycle counts not    */
    /* yet executed as well as the current count, in  */
    /* case running preset cycles passed them up      */
    while (0 != events[event_index].event_time
             && events[event_index].event_time <= count)
    {
      /* if event token is 'Q', exit the simulation   */
      if ('Q' == events[event_index].event_token)
      {
        quit = 1;
      }
      else
      {
        *events[event_index].event_dest =
              events[event_index].event_value;
      }
      ++event_index;
    }
  }
  return quit;
}

/*********************************************************
 *
 * Function:    Continue
 *
 * Parameters:  none
 *
 * Returns:     1 if the simulation should continue to run,
 *              0 if the program should exit
 *
 * Description: controls the process of the simulation
 *              when not running a preset number of cycles
 *
 *              allows parameter setting by entries of the
 *              form token=value, or just a token letter
 *              in which case it displays a prompt and
 *              accepts the value entry
 *
 *              continues until one of the following:
 *
 *              receives a 'q' or 'Q' from stdin, or sees EOF
 *              on stdin, all of which cause it to return 0
 *              to terminate the simulation
 *
 *              processes a '\n' not part of a parameter
 *              entry, causing it to return 1 to continue
 *              the simulation
 *
 *              processes a parameter setting which sets
 *              cycle count to a non-zero value, indicating
 *              that the simulation should run the new
 *              number of cycles without user input, in
 *              which case it also returns 1 to continue
 *
 ********************************************************/
static int
Continue(void)
{
  char              inbuff[100];
  int               ch;
  int               show_params;
  char              *pequal;
  PARAM_PARSE       *pp;
  double            value;
  
  /* allow interactive input                          */
  do
  {
    show_params = 0;    /* don't show the menu        */
    ch = getchar();
    if (ch == EOF)
    {
      break;
    }
    else
    {
      ch = toupper((unsigned char)ch);
    }
    
    if (isupper(ch))
    {
      /* see if the letter represents a parameter     */
      pp = bsearch(&ch, parse,
                   sizeof parse / sizeof *parse,
                   sizeof *parse, Compare);

      if (NULL != pp) /* found a match                */
      {
        /* allow for the complete entry on the line   */
        if (NULL != fgets(inbuff, sizeof inbuff, stdin))
        {
          /* check for equal sign in rest of line     */
          if (NULL != (pequal = strchr(inbuff, '=')))
          {
            /* yes, get the value                     */
            ParseValue(pp, pequal + 1, pp->destination);
            show_params = 1;  /* show all values      */
          }
          else  /* NULL == (pequal = strchr(...))     */
          {
            /* display a prompt for the value         */
            /* get current value for display          */
            value = *(pp->destination);
        
            if (pp->force_to_int)
            {
              fprintf(stderr, "\nEnter %s current value "
                      "%0.0f range %0.0f to %0.0f ",
                      pp->param_name, value,
                      pp->min_val,
                      pp->max_val);
            }
            else
            {
              fprintf(stderr, "\nEnter %s current value "
                      "%0.4f range %0.4f to %0.4f ",
                      pp->param_name, value, pp->min_val,
                      pp->max_val);
            }
            fflush(stderr);

            if (NULL != fgets(inbuff,
                sizeof inbuff, stdin))
            {
              ParseValue(pp, inbuff, pp->destination);
              show_params = 1;  /* show all values    */
            }
          }     /* else NULL == (pequal = strchr(...))*/
        }     /* if (NULL != fgets(...)               */
      }     /* if (NULL != pp)                        */
    }     /* isupper(ch)                              */
    else if ('?' == ch)
    {
      /* user requesting menu and current values      */
      if ((ch = ClearStdin()) == EOF)
      {
        break;
      }
      show_params = 1;
    }

    /* if a new cycle count was just sent, return a   */
    /* continue status and go back to automatic       */
    if (0 != params.cycles)
    {
      ch = '\n';
    }
    else if (show_params)
    {
      ShowParams();   /* display values and menu      */
      ch = 0;         /* force another pass           */
    }

  } while (ch != '\n' && ch != EOF && ch != 'Q' && ch != 'q');

  return (ch == '\n');  
}
  
/*********************************************************
 *
 * Function:    GetCurrentPV
 *
 * Parameters:  double output, the output value calculated
 *              by the last call to ComputerPID()
 *
 * Returns:     the new value of the process variable (PV)
 *
 * Description: simulates the response of the system being
 *              controlled, taking the value of the lag
 *              member of the PID_PARAMS structure into
 *              account
 *
 * Algorithm:   if the output value remains the same for
 *              a great length of time the process variable
 *              will eventually reach and remain at a value
 *              determined entirely by the transfer ratio
 *              (params.trans) times the output value
 *
 *              the more inertia the controlled system has,
 *              that is the higher the lag member, the longer
 *              this takes
 *
 *              if the PV on the last iteration is not the
 *              same as the final PV value for the new
 *              output, the new PV will be somewhere in
 *              between the two
 *
 *              if the lag value is near 1 (high inertia),
 *              the PV will only change a small part of the
 *              way to the value indicated by the new
 *              output
 *
 *              if the lag value is near 0 (low inertia),
 *              the PV will change most of the way to the
 *              value indicated by the new output
 *
 *              the extremes are a lag of 1, in which case
 *              the system is a perpetual motion machine and
 *              the PV will never change no matter what the
 *              output does, and a lag of 0, in which
 *              case the PV responds fully to a change in
 *              output completely by the next pass through
 *              the loop
 *
 ********************************************************/
static double
GetCurrentPV(double output)
{
  static double     build_up                = 0.0;
         double     value;

  build_up += output;
  value = build_up * (1 - params.lag) * params.trans;
  build_up *= params.lag;
  return value;
}

/*********************************************************
 *
 * Function:    ComputePID
 *
 * Parameters:  this contains the actual PID calculation
 *              algorithm, including optional accelleration,
 *              velocity, and friction feed forwards, and
 *              a slew rate limitation
 *
 * Returns:     a double containg the calculated new
 *              output value
 *
 * Description: main control algorithm
 *
 ********************************************************/
static double
ComputePID(double PV)
{
  /* the three static variables are required to retain  */
  /* information between passes through the loop        */
  static double     integral                = 0.0;
  static double     last_error              = 0.0;
  static double     last_output             = 0.0;

         double     this_error;
         double     this_output;
         double     accel;
         double     deriv;
         double     friction;

  /* the desired PV for this iteration is the value     */
  /* calculated as next_target during the last loop     */
  this_target = next_target;
  
  /* test for acceleration, compute new target PV for   */
  /* the next pass through the loop                     */
  if (params.accel > 0 && this_target != params.setpt)
  {
    if (this_target < params.setpt)
    {    
      next_target += params.accel;
      if (next_target > params.setpt)
      {
        next_target = params.setpt;
      }
    }
    else /* params.target > params.setpoint */
    {    
      next_target -= params.accel;
      if (next_target < params.setpt)
      {
        next_target = params.setpt;
      }
    }
  }
  else
  {
    next_target = params.setpt;
  }
  
  /* acceleration is the difference between the PV      */
  /* target on this pass and the next pass through the  */
  /* loop                                               */
  accel = next_target - this_target;

  /* the error for the current pass is the difference   */
  /* between the current target and the current PV      */
  this_error = this_target - PV;

  /* the derivative is the difference between the error */
  /* for the current pass and the previous pass         */
  deriv = this_error - last_error;

  /* a very simple determination of whether there is    */
  /* special friction to be overcome on the next pass,  */
  /* if the current PV is 0 and the target for the next */
  /* pass is not 0, stiction could be a problem         */
  friction = (PV == 0.0 && next_target != 0.0);

  /* the new error is added to the integral             */
  integral += this_target - PV;

  /* the square of the error is accumulated in          */
  /* rms_error, for reporting at the end of the program */
  /* it has no part in the PID loop calculations        */
  rms_error += (this_error * this_error);

  /* now that all of the variable terms have been       */
  /* computed they can be multiplied by the appropriate */
  /* coefficients and the resulting products summed     */
  this_output = params.p_gain * this_error
              + params.i_gain * integral
              + params.d_gain * deriv
              + params.acc_ff * accel
              + params.vel_ff * next_target
              + params.fri_ff * friction
              + params.bias;

  last_error   = this_error;

  /* check for slew rate limiting on the output change  */
  if (0 != params.slew)
  {
    if (this_output - last_output > params.slew)
    {
      this_output = last_output + params.slew;
    }
    else if (last_output - this_output > params.slew)
    {
      this_output = last_output - params.slew;
    }
  }
  
  /* now check the output value for absolute limits     */
  if (this_output < params.min)
  {
    this_output = params.min;
  }
  else if (this_output > params.max)
  {
    this_output = params.max;
  }
  
  /* store the new output value to be used as the old   */
  /* output value on the next loop pass                 */
  return last_output = this_output;
}

/*********************************************************
 *
 * Function:    main
 *
 * Parameters:  int argc, number of command line parameters,
 *              including the name of the executable, if
 *              the platform provides it
 *
 *              char **argv, array of pointers to '\0'
 *              terminated strings containing the
 *              executable name, if available, and the
 *              command line arguments
 *
 * Returns:     integer value of 0
 *
 * Description: entry and main driver for the PID
 *              simulation
 *
 ********************************************************/
int main(int argc, char **argv)
{
  double            current_output          = 0.0;
  double            old_output              = 0.0;
  unsigned long     repetitions             = 0;
  unsigned long     hold_time               = 0;
  int               verbose                 = 0;
  double            PV;
  char              results [100];
  char              *args;
  int               current_arg;
  
  /* this eliminates a warning on some compilers,     */
  /* comment it out if you don't need it              */
  argc = argc;
  
  /* check command line arguments, can accept one     */
  /* -v or -V and one representing a file name with   */
  /* paremeter and event settings                     */
  for (current_arg = 1;
       NULL != (args = argv[current_arg]);
       ++current_arg)
  {
    if (*args == '-')
    {
      if (args[1] == 'v' || args[1] == 'V')
      {
        verbose = 1;
      }
    }
    else
    {
      ParseParams(args);
    }
  }
  
  params.cycles = floor(params.cycles);

  ShowParams();

  /* before starting the loop, check for the case of no */
  /* acceleration and preset the this_target and        */
  /* next_target variables                              */
  if (0.0 == params.accel)
  {
    this_target = next_target = params.setpt;
  }
 
  /* loop until a quit event is returned by CheckEvents */
  while (0 == CheckEvents(repetitions))
  {
    /* only call interactive function Continue() if     */
    /* there is no preset continuous cycle count, exit  */
    /* if Continue() receives quit command or EOF       */
    if (0 == params.cycles && 0 == Continue())
    {
      break;
    }

    PV = GetCurrentPV(current_output);
    current_output = ComputePID(PV);
    if (hold_time > 0)
    {
      --hold_time;
      current_output = old_output;
    }
    else if (current_output != old_output)
    {
      hold_time = (unsigned long)floor(params.hold);
      old_output = current_output;
    }

    sprintf(results,
          "%5lu: SP %7.2f PV %7.2f Out %7.2f ",
           ++repetitions, this_target, PV,
           current_output);
    fputs(results, stdout);
    if (verbose)
    {
      fputs(results, stderr);
      putc('\n', stderr);
    }
    if (params.cycles)
    {
      params.cycles = floor(params.cycles) - 1;
      putc('\n', stdout);
    }
  }

  if (repetitions)
  {
    rms_error /= repetitions;
  }
  else
  {
    rms_error = 0;
  }
  
  fprintf(stderr, "\n%lu repetitions, RMS error %f\n",
          repetitions, sqrt(rms_error));
  
  return 0;
}
