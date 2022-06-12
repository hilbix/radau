/*
 */

#include <ctype.h>

static int
STR_rtrim(char *s)
{
  size_t pos;

  for (pos=strlen(s); pos && isspace(s[pos-1]); s[--pos]=0);
  return pos;
}

