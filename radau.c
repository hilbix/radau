/* RADAU - Unidirectional connection mesh using UDP.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#include "radau.h"

/* Just run a single radau	*/
int
main(int argc, const char * const *argv)
{
  struct radau	radau;
  return R_main(&radau, argv+1);
}

