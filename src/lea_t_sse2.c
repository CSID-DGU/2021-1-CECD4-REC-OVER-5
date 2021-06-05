#include "lea.h"
#include "lea_locl.h"

#ifdef COMPILE_SSE2
#if !defined(_M_X64) && (_M_IX86_FP != 2) && !defined(__SSE2__)
#error "turn on SSE2 flag for lea_t_sse2.c"
#endif

#include <emmintrin.h>


#define MAX_BLK 4
#define SIMD_TYPE sse2

#define lea_encrypt_1block lea_encrypt
#define lea_decrypt_1block lea_decrypt

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
