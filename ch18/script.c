#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define X_SIZE 6.0
#define Y_SIZE 4.0
#define MAX_POINTS 1000

double set [MAX_POINTS];
double pv [MAX_POINTS];
double out [MAX_POINTS];

double o_scale, o_offset;   /* vertical scale for output    */
double p_scale, p_offset;   /* vertical scale for process   */
double s_scale, s_offset;   /* vertical scale for set point */
double x_scale, x_offset;   /* must have a horizontal scale */
int grid_x, grid_y;

const char *header =
"limits\n"
"0,0\n"
"6,4\n"
"zoom\n"
"w\n"
"-.5,-.5\n"
"6.5,4.5\n"
"grid\n"
".25\n";

const char *pedit =
"\npedit\nl\nw\n.01\nx\nchange\nl\n\np\nc\n%s\n\n";

const char *pline = "pline\n";

void parse_scale(const char *s, double *d1, double *d2)
{
  char *cp1, *cp2;
  
  *d1 = strtod(s + 1, &cp2);
  if (NULL == (cp1 = strchr(cp2, ',')))
  {
    return;
  }
  *d2 = strtod(cp1 + 1, NULL);
}

int main(int argc, char **argv)
{
  FILE *fin = NULL, *fout = NULL;
  char buff[100];
  int count, total;
  char *cp1, *cp2, *cp3;
  double d1, d2, d3;
  double error = 0;

  if (argc < 4)
  {
    puts("usage: script param-file-name source-file-name script-file-name");
    return EXIT_FAILURE;
  }
  else if (NULL == (fin = fopen(argv[1], "r")))
  {
    printf("script: can't open %s\n", argv[1]);
    goto crash;
  }
  else if (NULL == (fout = fopen(argv[3], "w")))
  {
    printf("script: can't make %s\n", argv[3]);
    goto crash;
  }

  while (NULL != (fgets(buff, sizeof buff, fin)))
  {
    if (strchr("gGoOpPsSxX", *buff))
    {
      parse_scale(buff, &d1, &d2);

      switch (*buff)
      {
        case 'g':
        case 'G':
          grid_x = (int)d1;
          grid_y = (int)d2;
          break;
        case 'o':
        case 'O':
          o_offset = d1;
          if (d2) o_scale = 4.0 / d2;
          break;
        case 'p':
        case 'P':
          p_offset = d1;
          if (d2) p_scale = 4.0 / d2;
          break;
        case 's':
        case 'S':
          s_offset = d1;
          if (d2) s_scale = 4.0 / d2;
          break;
        case 'x':
        case 'X':
          x_offset = d3;
          if (d2) x_scale = 6.0 / d2;
          break;
      }
    }
  }

  fclose(fin);
  fin = NULL;

  if (0 == x_scale)
  {
    puts("no x scale data");
    goto crash;
  }
  else if (0 == p_scale)
  {
    puts("no p scale data");
    goto crash;
  }

  printf("X: scale %f offset %f\n", x_scale, x_offset);
  printf("O: scale %f offset %f\n", o_scale, o_offset);
  printf("P: scale %f offset %f\n", p_scale, p_offset);
  printf("S: scale %f offset %f\n", s_scale, s_offset);
  printf("G: grid x %d, grid y %d\n", grid_x, grid_y);

  if (NULL == (fin = fopen(argv[2], "r")))
  {
    printf("script: can't open %s\n", argv[2]);
    goto crash;
  }

  for (count = 0; count < MAX_POINTS; )
  {
    if (NULL == fgets(buff, sizeof buff, fin))
    {
      fclose(fin);
      printf("EOF after %d lines\n", count);
      break;
    }

    cp1 = strstr(buff, "SP");
    cp2 = strstr(buff, "PV");
    cp3 = strstr(buff, "Out");

    if (!cp1 && !cp2 && !cp2)
    {
      continue;
    }

    if (!cp1 || !cp2 || !cp2)
    {
      printf("bad data: %s\n", buff);
    }

    set[count] = atof(cp1 + 2);
    pv [count] = atof(cp2 + 2);
    out[count] = atof(cp3 + 3);

    error += ((set[count]-pv[count]) * (set[count]-pv[count]));

    ++count;
  }

  fclose(fin);
  fin = NULL;
  
  total = count;
  if (total)
  {
    printf("%d lines parsed error: %f\n", total, sqrt(error/total));
  }
  else
  {
    puts("no lines parsed");
    goto crash;
  }
  
  fprintf(fout, header);

  if (grid_x > 0)
  {
    fprintf(fout, "%s0,0\n0,4\n\n", pline);
    fputs("array\nl\n\nr\n\n", fout);
    fprintf(fout, "%d\n%f\n", grid_x + 1, 6.0 / grid_x);
  }
  
  if (grid_y > 0)
  {
    fprintf(fout, "%s0,0\n6,0\n\n", pline);
    fputs("array\nl\n\nr\n", fout);
    fprintf(fout, "%d\n\n%f\n", grid_y + 1, 4.0 / grid_y);
  }

  if (s_scale)
  {
    fprintf(fout, pline);
    for (count = 0; count < total; ++count)
    {
      fprintf(fout, "%f,%f\n",
              count * x_scale - x_offset,
              (set[count] * s_scale) - s_offset);
    }
    fprintf(fout, pedit, "red");
  }

  fprintf(fout, pline);
  for (count = 0; count < total; ++count)
  {
    fprintf(fout, "%f,%f\n",
            count * x_scale - x_offset,
            (pv[count] * p_scale) - p_offset);
  }
  fprintf(fout, pedit, "blue");

  if (o_scale)
  {
    fprintf(fout, pline);
    for (count = 0; count < total; ++count)
    {
      d1 = out[count];
      d1 *= o_scale;
      d1 -= o_offset;
      fprintf(fout, "%f,%f\n",
              count * x_scale - x_offset,
              (out[count] * o_scale) - o_offset);
    }
    fprintf(fout, pedit, 'b');
  }

  if (fin) fclose(fin);
  if (fout) fclose(fout);
  return 0;

  crash:
    if (fin) fclose(fin);
    if (fout) fclose(fout);
    return EXIT_FAILURE;
}
