#include "types.h"
#include "memlayout.h"

void*
memset(void *dst, int c, uint n)
{
  char *cdst = dst;
  if ((uint64)dst <= (uint64)MAX_USED_PHYSADDR)
    cdst = (char *)P2V(dst);
  int i;
  for(i = 0; i < n; i++){
    cdst[i] = c;
  }
  return dst;
}

int
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  if ((uint64)v1 <= (uint64)MAX_USED_PHYSADDR)
  	s1 = P2V(v1);
  if ((uint64)v2 <= (uint64)MAX_USED_PHYSADDR)
  	s2 = P2V(v2);
  while(n-- > 0){
    if(*s1 != *s2)
      return *s1 - *s2;
    s1++, s2++;
  }

  return 0;
}

void*
memmove(void *dst, const void *src, uint n)
{
  const char *s = src;
  char *d = dst;

  if(n == 0)
    return dst;
  
  if ((uint64)dst <= (uint64)MAX_USED_PHYSADDR)
  	d = P2V(dst);
  if ((uint64)src <= (uint64)MAX_USED_PHYSADDR)
  	s = P2V(src);
  if(s < d && s + n > d){
    s += n;
    d += n;
    while(n-- > 0)
      *--d = *--s;
  } else
    while(n-- > 0)
      *d++ = *s++;

  return dst;
}

// memcpy exists to placate GCC.  Use memmove.
void*
memcpy(void *dst, const void *src, uint n)
{
  return memmove(dst, src, n);
}

int
strncmp(const char *p, const char *q, uint n)
{
  const char *cp = p;
  const char *cq = q;
  if ((uint64)p <= (uint64)MAX_USED_PHYSADDR)
	  cp = P2V(p);
  if ((uint64)q <= (uint64)MAX_USED_PHYSADDR)
	  cq = P2V(q);
  while(n > 0 && *cp && *cp == *cq)
    n--, cp++, cq++;
  if(n == 0)
    return 0;
  return (uchar)*cp - (uchar)*cq;
}

char*
strncpy(char *s, const char *t, int n)
{
  char *cs = s;
  const char *ct = t;
  if ((uint64)s <= (uint64)MAX_USED_PHYSADDR)
	  cs = P2V(s);
  if ((uint64)t <= (uint64)MAX_USED_PHYSADDR)
	  ct = P2V(t);

  while(n-- > 0 && (*cs++ = *ct++) != 0)
    ;
  while(n-- > 0)
    *cs++ = 0;
  return s;
}

// Like strncpy but guaranteed to NUL-terminate.
char*
safestrcpy(char *s, const char *t, int n)
{
  char *cs = s;
  const char *ct = t;
  if ((uint64)s <= (uint64)MAX_USED_PHYSADDR)
	  cs = P2V(s);
  if ((uint64)t <= (uint64)MAX_USED_PHYSADDR)
	  ct = P2V(t);

  if(n <= 0)
    return s;
  while(--n > 0 && (*cs++ = *ct++) != 0)
    ;
  *cs = 0;
  return s;
}

int
strlen(const char *s)
{
  int n;
  const char *cs = s;

  if ( (uint64)s <= (uint64)MAX_USED_PHYSADDR )
	  cs = P2V(s);

  for(n = 0; cs[n]; n++)
    ;
  return n;
}

