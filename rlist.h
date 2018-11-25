/* List management	*/

#define	RLIST_ADD(p, head)		\
        (p)->last	= 0;		\
        (p)->next	= (head);	\
        if ((p)->next)			\
          {				\
            FATAL((p)->next->last);	\
            (p)->next->last	= (p);	\
          }				\
        (head)	= (p);			\

#define	RLIST_DEL(p, head)		\
        if ((head) == (p))		\
          (head) = (p)->next;		\
        if ((p)->next)			\
          (p)->next->last = (p)->last;	\
        (p)->next	= 0;		\
        (p)->last	= 0;		\

typedef struct relem	*R_ELEM;

struct rlist
  {
    R_ELEM	head;
  };

struct relem
  {
    R_ELEM	next, last;
    R_REF	ref;
    void	*data;
  };

