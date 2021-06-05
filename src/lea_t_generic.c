#include "lea.h"
#include "lea_locl.h"


#define MAX_BLK 1
#define SIMD_TYPE generic

#define lea_encrypt_1block lea_encrypt
#define lea_decrypt_1block lea_decrypt

#include "lea_ecb.h"
#include "lea_cbc.h"
#include "lea_ctr.h"
#include "lea_cfb.h"
#include "lea_ofb.h"

#include "lea_cmac.h"

#include "lea_ccm.h"
#include "lea_gcm.h"
