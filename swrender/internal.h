/*-
 * Add by Kevin.Wen to support some function which doesn't exist on windows
 *
 * copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * common internal API header
 */

#ifndef AVUTIL_INTERNAL_H
#define AVUTIL_INTERNAL_H

#if !defined(DEBUG) && !defined(NDEBUG)
#    define NDEBUG
#endif

#include <limits.h>
#include <stddef.h>
#include <assert.h>
#include "attributes.h"

#ifndef attribute_align_arg
#if (!defined(__ICC) || __ICC > 1110) && AV_GCC_VERSION_AT_LEAST(4,2)
#    define attribute_align_arg __attribute__((force_align_arg_pointer))
#else
#    define attribute_align_arg
#endif
#endif

#ifndef attribute_used
#if AV_GCC_VERSION_AT_LEAST(3,1)
#    define attribute_used __attribute__((used))
#else
#    define attribute_used
#endif
#endif

#ifndef av_alias
#if HAVE_ATTRIBUTE_MAY_ALIAS && (!defined(__ICC) || __ICC > 1110) && AV_GCC_VERSION_AT_LEAST(3,3)
#   define av_alias __attribute__((may_alias))
#else
#   define av_alias
#endif
#endif

#ifndef INT16_MIN
#define INT16_MIN       (-0x7fff-1)
#endif

#ifndef INT16_MAX
#define INT16_MAX       0x7fff
#endif

#ifndef INT32_MIN
#define INT32_MIN       (-0x7fffffff-1)
#endif

#ifndef INT32_MAX
#define INT32_MAX       0x7fffffff
#endif

#ifndef UINT32_MAX
#define UINT32_MAX      0xffffffff
#endif

#ifndef INT64_MIN
#define INT64_MIN       (-0x7fffffffffffffffLL-1)
#endif

#ifndef INT64_C
#   if defined(__GNUC__)
#       define INT64_C(c)     (c ## LL)
#       define UINT64_C(c)    (c ## ULL)
#   else
#       define INT64_C(c)     (c ## i64)
#       define UINT64_C(c)    (c ## ui64)
#   endif
#endif

#ifndef INT64_MAX
#define INT64_MAX INT64_C(9223372036854775807)
#endif

#ifndef UINT64_MAX
#define UINT64_MAX UINT64_C(0xFFFFFFFFFFFFFFFF)
#endif

#ifndef INT_BIT
#    define INT_BIT (CHAR_BIT * sizeof(int))
#endif

#ifndef offsetof
#    define offsetof(T,F) ((unsigned int)((char *)&((T *)0)->F))
#endif

/* Use to export labels from asm. */
#define LABEL_MANGLE(a) EXTERN_PREFIX #a

// Use rip-relative addressing if compiling PIC code on x86-64.
#if ARCH_X86_64 && defined(PIC)
#    define LOCAL_MANGLE(a) #a "(%%rip)"
#else
#    define LOCAL_MANGLE(a) #a
#endif

#define MANGLE(a) EXTERN_PREFIX LOCAL_MANGLE(a)

/* debug stuff */

/* dprintf macros */
#ifdef DEBUG
#    define dprintf(pctx, ...) av_log(pctx, AV_LOG_DEBUG, __VA_ARGS__)
#else
#    define dprintf(pctx, ...)
#endif

#define av_abort()      do { av_log(NULL, AV_LOG_ERROR, "Abort at %s:%d\n", __FILE__, __LINE__); abort(); } while (0)

/* math */

#if ARCH_X86
#    define FASTDIV(a,b) \
    ({\
        int ret,dmy;\
        __asm__ volatile(\
            "mull %3"\
            :"=d"(ret),"=a"(dmy)\
            :"1"(a),"g"(ff_inverse[b])\
            );\
        ret;\
    })
#elif HAVE_ARMV6
static inline av_const int FASTDIV(int a, int b)
{
    int r, t;
    __asm__ volatile("cmp     %3, #2               \n\t"
                     "ldr     %1, [%4, %3, lsl #2] \n\t"
                     "lsrle   %0, %2, #1           \n\t"
                     "smmulgt %0, %1, %2           \n\t"
                     : "=&r"(r), "=&r"(t) : "r"(a), "r"(b), "r"(ff_inverse));
    return r;
}
#elif ARCH_ARM
static inline av_const int FASTDIV(int a, int b)
{
    int r, t;
    __asm__ volatile ("umull %1, %0, %2, %3"
                      : "=&r"(r), "=&r"(t) : "r"(a), "r"(ff_inverse[b]));
    return r;
}
#elif CONFIG_FASTDIV
#    define FASTDIV(a,b)   ((uint32_t)((((uint64_t)a)*ff_inverse[b])>>32))
#else
#    define FASTDIV(a,b)   ((a)/(b))
#endif

extern const uint8_t ff_sqrt_tab[256];

#if ARCH_X86
#define MASK_ABS(mask, level)\
            __asm__ volatile(\
                "cltd                   \n\t"\
                "xorl %1, %0            \n\t"\
                "subl %1, %0            \n\t"\
                : "+a" (level), "=&d" (mask)\
            );
#else
#define MASK_ABS(mask, level)\
            mask= level>>31;\
            level= (level^mask)-mask;
#endif

#if HAVE_CMOV
#define COPY3_IF_LT(x,y,a,b,c,d)\
__asm__ volatile (\
    "cmpl %0, %3        \n\t"\
    "cmovl %3, %0       \n\t"\
    "cmovl %4, %1       \n\t"\
    "cmovl %5, %2       \n\t"\
    : "+&r" (x), "+&r" (a), "+r" (c)\
    : "r" (y), "r" (b), "r" (d)\
);
#else
#define COPY3_IF_LT(x,y,a,b,c,d)\
if((y)<(x)){\
     (x)=(y);\
     (a)=(b);\
     (c)=(d);\
}
#endif

#define FF_ALLOC_OR_GOTO(ctx, p, size, label)\
{\
    p = av_malloc(size);\
    if(p==NULL && (size)!=0){\
        av_log(ctx, AV_LOG_ERROR, "Cannot allocate memory.\n");\
        goto label;\
    }\
}

#if defined(__ICC) || defined(__SUNPRO_C)
    #define DECLARE_ALIGNED(n,t,v)      t v __attribute__ ((aligned (n)))
    #define DECLARE_ASM_CONST(n,t,v)    const t __attribute__ ((aligned (n))) v
#elif defined(__GNUC__)
    #define DECLARE_ALIGNED(n,t,v)      t v __attribute__ ((aligned (n)))
    #define DECLARE_ASM_CONST(n,t,v)    static const t v attribute_used __attribute__ ((aligned (n)))
#elif defined(_MSC_VER)
    #define DECLARE_ALIGNED(n,t,v)      __declspec(align(n)) t v
    #define DECLARE_ASM_CONST(n,t,v)    __declspec(align(n)) static const t v
#elif HAVE_INLINE_ASM
    #error The asm code needs alignment, but we do not know how to do it for this compiler.
#else
    #define DECLARE_ALIGNED(n,t,v)      t v
    #define DECLARE_ASM_CONST(n,t,v)    static const t v
#endif

#define FF_ALLOCZ_OR_GOTO(ctx, p, size, label)\
{\
    p = av_mallocz(size);\
    if (p == NULL && (size) != 0) {\
        av_log(ctx, AV_LOG_ERROR, "Cannot allocate memory.\n");\
        goto label;\
    }\
}

//#include "libm.h"

/*#if !HAVE_LLRINT
static long long llrint(double x)
{
	char* buf;
	int i, desc, sign;
	buf = _fcvt(x, 0, &desc, &sign);

	i = atoi(buf);
	if(sign == 1)
	{
		i = i*(-1);
	}

	return (i);
}
#endif /* HAVE_LLRINT */

#if !HAVE_LRINT
static av_always_inline av_const long int lrint(double x)
{
	char* buf;
	int i, desc, sign;
	buf = _fcvt(x, 0, &desc, &sign);

	i = atoi(buf);
	if(sign == 1)
	{
		i = i*(-1);
	}

	return (i);
}
#endif /* HAVE_LRINT */


static av_always_inline av_const long int isinf(float x)//Kevin.Wen:fix it
{
	return 0;
}

static av_always_inline av_const int isnan(double val)//Kevin.Wen:fix it
{
	volatile double d = val;
	return d!=d;
}

#if !HAVE_ROUND
static av_always_inline av_const double round(double x)
{
    return (x > 0) ? floor(x + 0.5) : ceil(x - 0.5);
}
#endif /* HAVE_ROUND */

#if !HAVE_ROUNDF
static av_always_inline av_const float roundf(float x)
{
    return (x > 0) ? floor(x + 0.5) : ceil(x - 0.5);
}
#endif /* HAVE_ROUNDF */

#if !HAVE_TRUNCF
static av_always_inline av_const float truncf(float x)
{
    return (x > 0) ? floor(x) : ceil(x);
}
#endif /* HAVE_TRUNCF */

/**
 * Returns NULL if CONFIG_SMALL is true, otherwise the argument
 * without modification. Used to disable the definition of strings
 * (for example AVCodec long_names).
 */
#if CONFIG_SMALL
#   define NULL_IF_CONFIG_SMALL(x) NULL
#else
#   define NULL_IF_CONFIG_SMALL(x) x
#endif

#if HAVE_SYMVER_ASM_LABEL
#   define FF_SYMVER(type, name, args, ver)                     \
    type ff_##name args __asm__ (EXTERN_PREFIX #name "@" ver);  \
    type ff_##name args
#elif HAVE_SYMVER_GNU_ASM
#   define FF_SYMVER(type, name, args, ver)                             \
    __asm__ (".symver ff_" #name "," EXTERN_PREFIX #name "@" ver);      \
    type ff_##name args;                                                \
    type ff_##name args
#endif

#endif /* AVUTIL_INTERNAL_H */
