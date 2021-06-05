#include "lea.h"
#include "lea_locl.h"


#if defined(COMPILE_AVX2) && defined(COMPILE_XOP)

#ifndef __AVX2__
#error "turn on AVX2 flag for lea_t_avx2_xop.c"
#endif

#if !defined(_MSC_VER) && !defined(__XOP__)
#error "turn on XOP flag for lea_t_avx2_xop.c"
#endif

#include <xmmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>
#if defined(_MSC_VER)
#include <ammintrin.h>
#else
#include <x86intrin.h>
#endif


#define MAX_BLK 8
#define SIMD_TYPE avx2_xop
#define USE_OWN_FUNC
#define lea_decrypt_1block lea_decrypt

static void ctr128_inc_aligned(unsigned char *counter);

#include "lea_avx2.h"
#include "lea_xop.h"

#include "lea_ecb.h"
#include "lea_cbc.h"
#include "lea_ctr.h"
#include "lea_cfb.h"
#include "lea_ofb.h"

#include "lea_cmac.h"

#include "lea_ccm.h"
#include "lea_gcm.h"

#endif
