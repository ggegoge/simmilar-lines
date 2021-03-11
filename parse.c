#include <stdio.h>

#include "parse.h"


int try_int_format(PLine pline, const char* s, const char* format)
{
  unsigned long long num;

  if (sscanf(s, format, &num) == 1) {
    add_parsed_int(pline, (long long) num);
    return 1;
  } else
    return 0;
}

int try_int(PLine pline, const char* s)
{
  /* if (try_int_format(pline, s, "%llo") || try_int_format(pline, s, "%llx") ||
   *     try_int_format(pline, s, "%lld"))
   *   return 1;
   * else
   *   return 0; */

  unsigned long long num;
  if (sscanf(s, "%lli", &num)) {
    add_parsed_int(pline, (long long) num);
    return 1;
  } else
    return 0;  
}

int try_float_format(PLine pline, const char* s, const char* format)
{
  double num;

  if (sscanf(s, format, &num)) {
    add_parsed_float(pline, (double) num);
    return 1;
  } else
    return 0;
}


int try_float(PLine pline, const char* s)
{
  if (try_float_format(pline, s, "%lg") || try_float_format(pline, s, "%lG") ||
      try_float_format(pline, s, "%llf"))
    return 1;
  else
    return 0;
}


int try_polymorphic(PLine pline, const char* s, const char* format,
                    void(*add)(PLine, void*))
{
  unsigned long long val;

  if (sscanf(s, format, &val)) {
    add(pline, &val);
    return 1;
  } else
    return 0;
}

void add_parsed_int(PLine pline, void * val)
{
  long long num = *(long long *)val;
  pline.ints.used++;
  if (pline.ints.used)
  /* append_ints(&pline.ints, num); */
  
}
