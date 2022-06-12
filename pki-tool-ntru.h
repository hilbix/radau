/* PKI based on the NTRU library which is quantum resistant.
 * Note that the ntru library is GPLv3!
 */

#include <libntru/ntru.h>

#define	PKI_PARAMS	EES1087EP2	/* 256 bit security IEEE 1361.1	*/

typedef struct PKI
  {
    void		*user;
    char		*err;
    NtruEncKeyPair	kp;
    NtruRandContext	rnd;
    NtruRandGen		rng;
  } *PKI;

typedef struct PKIdst
  {
    PKI			pki;
    NtruEncPubKey	pub;
  } *PKIdst;

#define	PKIOK(FN)	do { ok = ok && PKIok(P, (FN), #FN); } while (0)
#define	PKIERR(S,...)	do { ok = ok && PKIerr(P, S, __VA_ARGS__); } while (0)

static int
PKIerr(PKI P, const char *s, ...)
{
  va_list	list;

  MFREE(P->err);
  va_start(list, s);
  P->err	= Mprintf("%s: %s", s, list);
  va_end(list);
  return 0;
}

static int
PKIok(PKI P, int ko, const char *fn)
{
  const char	*s;

  switch (ko)
    {
    case 0:				return 1;
    default:				s="unknown error"; break;
    case NTRU_ERR_OUT_OF_MEMORY:	s="out of memory"; break;
    case NTRU_ERR_PRNG:			s="PRNG error"; break;
    case NTRU_ERR_MSG_TOO_LONG:		s="message too long"; break;
    case NTRU_ERR_INVALID_MAX_LEN:	s="invalid maxlen"; break;
    case NTRU_ERR_DM0_VIOLATION:	s="DM0 violation"; break;
    case NTRU_ERR_NO_ZERO_PAD:		s="no zero padding"; break;
    case NTRU_ERR_INVALID_ENCODING:	s="invalid encoding"; break;
    case NTRU_ERR_NULL_ARG:		s="NULL argument"; break;
    case NTRU_ERR_UNKNOWN_PARAM_SET:	s="unkown parameter set"; break;
    case NTRU_ERR_INVALID_PARAM:	s="invalid parameter"; break;
    case NTRU_ERR_INVALID_KEY:		s="invalid key"; break;
    }
  return PKIerr(P, "%s: %s", s, fn);
}

static void
PKI_free(PKI P)
{
  P->rng.release(&P->rnd);
  MFREE(P->err);
  memset(P, 0, sizeof *P);
}

static PKI
PKI_init(PKI P, void *user)
{
  NtruRandGen rng	= NTRU_RNG_DEFAULT;
  int	ok = 1;

  memset(P, 0, sizeof *P);
  P->user	= user;
  P->rng	= rng;
  PKIOK(ntru_rand_init(&P->rnd, &P->rng));

  return ok ? P : 0;
}

static PKI
PKI_generate_identity(PKI P)
{
  int	ok = 1;
  PKIOK(ntru_gen_key_pair(&PKI_PARAMS, &P->kp, &P->rnd));
  return ok ? P : 0;
}

static PKI
PKI_put_key(PKI P, int (*writer)(void *user, const void *ptr, size_t))
{
#if 0
  writer(P->user, kp
#endif
}

static PKI
PKI_get_key(PKI P)
{
}

static PKIdst
PKI_pub_read(PKI P, const void *ptr, int len)
{
  PKIdst	D;
  int		ok=1;
  uint16_t	publen;

  MALLOC(D,1)->pki	= P;
  publen	= ntru_pub_len(&PKI_PARAMS);
  if (len == publen)
    {
      publen = ntru_import_pub(ptr, &D->pub);
      if (len == publen)
        return D;
      PKIERR("loaded public key length %llu is not %d", (unsigned long long)publen, len);
    }
  else
    PKIERR("given public key length %d is not %llu", len, (unsigned long long)publen);
  return MFREE(D);
}

static PKIdst
PKI_pub_write(PKdst R, const void *ptr, size_t *len)
{
  PKI	P = R->pki;
  int	ok=1;

  return ok ? R : 0;
}

