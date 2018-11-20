/* Ring management	*/

#define	RRING_ADD(p, head)		\
        (p)->last	= (p);		\
        if (head)			\
          {				\
            (p)->next	= (head);	\
            (p)->last	= (head)->last;	\
            FATAL((p)->last->next!=(head));	\
            (p)->last->next	= (p);	\
            (head)->last	= (p);	\
          }				\
        else				\
          {				\
            (p)->next	= (p);		\
            (p)->last	= (p);		\
          }				\
        (head)	= (p);			\

#define	RRING_DEL(p, head)		\
        (p)->next->last	= (p)->last;	\
        (p)->last->next	= (p)->next;	\
        if ((head) == (p))		\
          {				\
            (head) = (p)->next;		\
            if ((head) == (p))		\
              (head) = 0;		\
          }				\
        (p)->next	= 0;		\
        (p)->last	= 0;		\

struct rring
  {
    R_ELEM		head;
    int			(*cmp)(R, const void *, const void *);
  };

typedef	struct rring	*R_RING;

/* Move the compare function to the ring?	*/
static void *
r_ring_add(R, R_RING ring, void *data, R_REF ref)
{
  R_ELEM	p;

  FATAL(!ref);
  /* This could be speed up with hashing, but for this we need some additional hashing function	*/
  if ((p=ring->head)!=0)
    do
      {
        if (!ring->cmp(r, data, p->data))
          {
            r_ref_inc(r, ref);
            return p->data;
          }
      } while ((p=p->next)!=ring->head);
  /* Allocate new one	*/

  p		= alloc0(sizeof *p);
  p->data	= data;

  RRING_ADD(p, ring->head);

  return p;
}

static int
r_ring_len(R_RING ring)
{
  R_ELEM	p;
  int		len;

  if ((p=ring->head)==0)
    return 0;

  for (len=1; (p = p->next) != ring->head; len++);
  return len;
}

