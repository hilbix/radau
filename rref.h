/* Reference counters	*/

typedef	struct ref	*R_REF;

struct ref
  {
  };

static R_REF
r_ref(R, void *ref, int refs, void (*free_fn)(R, void *))
{
  
}
