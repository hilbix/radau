/* LIBS:
 *
 * PKI based on the signal-protocol library.
 * Note that the signal-protocol is GPLv3!
 *
 * WHY? Because it looks like a mature implementation.
 * On contrast OpenSSL is confusing:
 *
 * man RSA_generate_key_ex
 * pseudo-random number generator must be seeded prior to calling RSA_generate_key_ex()
 *
 * https://wiki.openssl.org/index.php/Random_Numbers
 * OpenSSL will attempt to seed the random number generator automatically upon instantiation by calling RAND_poll.
 * If the generator is not initialized and RAND_bytes is called, then the generator will also call RAND_poll
 */

#include <signal/signal_protocol.h>
#include "libsignal-protocol-c/tests/test_common_openssl.c"

#define	LOCK	LOCK_dkfjsdkfjwiejfiwjijefwef

typedef struct PKI
  {
    void		*user;
    char		*err;
    signal_context	*ctx;
  } *PKI;

#define	PKIOK(FN)	ok = ok && PKIerr(P, !!(FN), "fail: %s", #FN)
#define	PKIOOM(FN)	ok = ok && PKIerr(P, !!(FN), "out of memory: %s", #FN)

static int
PKIerr(PKI P, int ok, const char *s, ...)
{
  va_list	list;

  if (ok)
    return ok;

  MFREE(P->err);
  va_start(list, s);
  P->err	= Mvprintf(s, list);
  va_end(list);
  return ok;
}

static void
PKI_free(PKI P)
{
  MFREE(P->err);
  memset(P, 0, sizeof *P);
}

static void
PKI_lock(void *user)
{
  pthread_mutex_lock(&PKI_mutex);
}

static void
PKI_unlock(void *user)
{
  pthread_mutex_unlock(&PKI_mutex);
}

static PKI
PKI_init(PKI P, void *user)
{
  int	ok = 1;

  memset(P, 0, sizeof *P);

  signal_crypto_provider provider =
    { .user_data = P
    , .random_func = test_random_generator
    , .hmac_sha256_init_func = test_hmac_sha256_init
    , .hmac_sha256_update_func = test_hmac_sha256_update
    , .hmac_sha256_final_func = test_hmac_sha256_final
    , .hmac_sha256_cleanup_func = test_hmac_sha256_cleanup
    , .sha512_digest_init_func = test_sha512_digest_init
    , .sha512_digest_update_func = test_sha512_digest_update
    , .sha512_digest_final_func = test_sha512_digest_final
    , .sha512_digest_cleanup_func = test_sha512_digest_cleanup
    , .encrypt_func = test_encrypt
    , .decrypt_func = test_decrypt
    };

  signal_context_create(&P->ctx, P);
  signal_context_set_crypto_provider(P->ctx, &provider);
  signal_context_set_locking_functions(P->ctx, PKI_lock, PKI_unlock);

  P->user	= user;
  PKIOK(RAND_poll());
  return ok ? P : 0;
}

static PKI
PKI_generate_identity(PKI P)
{
  
}

#if 0
static PKI
PKI_generate_key_openssl(PKI P)
{
  BIGNUM	*e;
  RSA		*r;
  BIO		*b;
  int		ok = 1;

  PKIOOM(e = BN_new());
  PKIOOM(r = RSA_new());
  PKIOOM(b = BIO_new(BIO_s_mem()));

  /* Following step seems irresponsible and not meaningful at all.
   * Can somebody please enlighten me why this reasonable?
   * "It just works this way" is a non-explanation, this exposes a critical problem in the brains of the implementors!
   * Proof:
   * Math basics dictate, that you explain each step.  This includes why it is needed and why it is meaningful.
   * Crypto is pure math.  But the facts dictate, that people are usually no Mathematicians.
   * Therefor crypto must not require users to know anything about cryptology to use it properly!
   * This leads to the fundamental requirement for all crypto implementations:
   * Be easy to use but make it almost impossible to use it the wrong way.
   * If this requirement is not met, the implementation just is toxic waste.
   * Why do you unnecessarily implement toxic waste?
   */
  PKIOK(BN_set_word(e, RSA_F4));		/* What is RSA_F4?  Vaginal Secretion?  And what happened to RSA_F0 to F3?	*/

  /* Undocumented: can e be safely freed afterwards?  */
  PKIOK(RSA_generate_key_ex(r, 4096, e, NULL));	/* WARNING: this uses an undocumented return value	*/

  b.

  BIO_free(b);
  RSA_free(r);
  BN_free(e);
  return ok ? P : 0;
}
#endif

