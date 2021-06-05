#include "lea.h"
#include "lea_locl.h"

#ifdef COMPILE_NEON

#if !defined(__ARM_NEON__)
#error "turn on NEON flag for lea_t_neon.c"
#endif


#include <arm_neon.h>


#define MAX_BLK 4
#define SIMD_TYPE neon

#define lea_encrypt_1block lea_encrypt
#define lea_decrypt_1block lea_decrypt

#include "lea_neon.h"

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
