#include "lea.h"
#include "lea_locl.h"


#if defined(COMPILE_AVX2) && defined(COMPILE_SSE2)

#ifndef __AVX2__
#error "turn on AVX2 flag for lea_t_avx2_sse2.c"
#endif

#if !defined(_M_X64) && (_M_IX86_FP != 2) && !defined(__SSE2__)
#error "turn on SSE2 flag for lea_t_avx2_sse2.c"
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
#define SIMD_TYPE avx2_sse2

#define lea_encrypt_1block lea_encrypt
#define lea_decrypt_1block lea_decrypt

#include "lea_avx2.h"
#include "lea_sse2.h"

#include "lea_key.h"

#include "lea_ecb.h"
#include "lea_cbc.h"
#include "lea_ctr.h"
#include "lea_cfb.h"
#include "lea_ofb.h"

#include "lea_cmac.h"

#include "lea_ccm.h"
#include "lea_gcm.h"

#endif
