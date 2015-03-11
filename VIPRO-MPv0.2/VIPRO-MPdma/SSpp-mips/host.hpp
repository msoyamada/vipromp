
#ifndef HOST_H
#define HOST_H

/* make sure host compiler supports ANSI-C */
#ifndef __STDC__ /* an ansi C compiler is required */
#error The SimpleScalar simulators must be compiled with an ANSI C compiler.
#endif /* __STDC__ */

/* enable inlining here, if supported by host compiler */
#undef INLINE
#if defined(__GNUC__)
#define INLINE		inline
#else
#define INLINE
#endif

/* bind together two symbols, at preprocess time */
#ifdef __GNUC__
/* this works on all GNU GCC targets (that I've seen...) */
#define SYMCAT(X,Y)	X##Y
#define ANSI_SYMCAT
#else /* !__GNUC__ */
#ifdef OLD_SYMCAT
#define SYMCAT(X,Y)	X/**/Y
#else /* !OLD_SYMCAT */
#define SYMCAT(X,Y)	X##Y
#define ANSI_SYMCAT
#endif /* OLD_SYMCAT */
#endif /* __GNUC__ */

/* host-dependent canonical type definitions */
typedef int bool_t;			/* generic boolean type */
typedef unsigned char byte_t;		/* byte - 8 bits */
typedef signed char sbyte_t;
typedef unsigned short half_t;		/* half - 16 bits */
typedef signed short shalf_t;
typedef unsigned int word_t;		/* word - 32 bits */
typedef signed int sword_t;
typedef float sfloat_t;			/* single-precision float - 32 bits */
typedef double dfloat_t;		/* double-precision float - 64 bits */

/* quadword defs, note: not all targets support quadword types */
#if defined(__GNUC__) || defined(__SUNPRO_C) || defined(__CC_C89) || defined(__CC_XLC)
#define HOST_HAS_QUAD
#if !defined(__FreeBSD__)
//typedef unsigned long long quad_t;	/* quad - 64 bits */
typedef signed long long squad_t;
#else /* __FreeBSD__ */
#define quad_t		unsigned long long
#define squad_t		signed long long
#endif /* __FreeBSD__ */
#ifdef ANSI_SYMCAT
#define ULL(N)		N##ULL		/* quad_t constant */
#define LL(N)		N##LL		/* squad_t constant */
#else /* OLD_SYMCAT */
#define ULL(N)		N/**/ULL		/* quad_t constant */
#define LL(N)		N/**/LL		/* squad_t constant */
#endif
#elif defined(__alpha)
#define HOST_HAS_QUAD
typedef unsigned long quad_t;		/* quad - 64 bits */
typedef signed long squad_t;
#ifdef ANSI_SYMCAT
#define ULL(N)		N##UL		/* quad_t constant */
#define LL(N)		N##L		/* squad_t constant */
#else /* OLD_SYMCAT */
#define ULL(N)		N/**/UL		/* quad_t constant */
#define LL(N)		N/**/L		/* squad_t constant */
#endif
#elif defined(_MSC_VER)
#define HOST_HAS_QUAD
typedef unsigned __int64 quad_t;	/* quad - 64 bits */
typedef signed __int64 squad_t;
#define ULL(N)		((quad_t)(N))
#define LL(N)		((squad_t)(N))
#else /* !__GNUC__ && !__alpha */
#undef HOST_HAS_QUAD
#endif

/* statistical counter types, use largest counter type available */
#ifdef HOST_HAS_QUAD
typedef squad_t counter_t;
typedef squad_t tick_t;			/* NOTE: unsigned breaks caches */
#else /* !HOST_HAS_QUAD */
typedef dfloat_t counter_t;
typedef dfloat_t tick_t;
#endif /* HOST_HAS_QUAD */

#endif /* HOST_H */
