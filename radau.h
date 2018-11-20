#ifndef	_INC_RADAU_H
#define	_INC_RADAU_H

#define	_STRINGIFY(X)	#X
#define	STRINGIFY(X)	_STRINGIFY(X)

#define	R_PORT		19162	/* 0x4adau	*/
#define	R		struct radau *r

#define	R_HD_IP_MIN	20
#define	R_HD_IP_MAX	60
#define	R_HD_UDP	8

#define	R_IP4_MIN_SZ	576
#define	R_IP6_MIN_SZ	1500
#define	R_IP_MAX_SZ	65535

struct radau
  {
    /* dynamics	*/
    struct rref		*refs, *refc;
    struct rring	*ring;

    /* states	*/
    int			tick;

    /* statics	*/
    int			sock;

    /* proto	*/

    /* settings	*/
    int			cycle;	/* ms	*/

    /* temps	*/
    char		packet[R_IP_MAX_SZ];
  };

#endif

