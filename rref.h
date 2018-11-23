/* Reference counters
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#if	RADAU_PHASE==1

struct rref	*refs, *refc;

#elif	RADAU_PHASE==2

#include "rlist.h"

typedef	struct rref	*R_REF;

struct rref
  {
    struct rref	*next, *last;
    void *	ref;
    int		count;
    void (*	free_fn)(R, void *);	/* this function will be called (once) if count hits 0	*/
  };

/* Create or lookup reference of some pointer.
 * For optimization, the reference is returned.
 * Without optimization, the reference must be searched, which may take quite some time.
 *
 * The given "refs" count (which can be 0) is initialized.
 * If the given ptr is on the collection list, it is moved away from it.
 */
static R_REF
r_ref(R, void *ref, int refs, void (*free_fn)(R, void *))
{
  R_REF	p;

  FATAL(refs<0);
  for (p=r->refs; p; p=p->next)
    if (p->ref == ref)
      {
        FATAL(free_fn != p->free_fn);
        p->count	+= refs;
        if (!p->count || r->refs==p)
          return p;
        RLIST_DEL(p, r->refc);
        RLIST_ADD(p, r->refs);
        return p;
      }

  p		= alloc0(sizeof *p);
  p->ref	= ref;
  p->count	= refs;
  p->free_fn	= free_fn;

  RLIST_ADD(p, r->refs);

  return p;
}

/* Increment reference counter
 * This is easy:
 * Just increment count
 */
static void
r_ref_inc(R, R_REF p)
{
  p->count++;
  FATAL(!p->count);
}

/* Decrement reference counter
 * In case count hits zero,
 * if is added to the collection list.
 */
static void
r_ref_dec(R, R_REF p)
{
  FATAL(!p->count);
  if (--p->count) return;

  if (r->refc==p) return;
  RLIST_DEL(p, r->refs);
  RLIST_ADD(p, r->refc);
}

static void
r_ref_init(R)
{
}

static void
r_ref_exit(R)
{
}

#elif	RADAU_PHASE==3

r->modadd(r, r_ref_init, r_ref_exit);

#endif

