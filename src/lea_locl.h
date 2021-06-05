#ifndef _LEA_LOCAL_H_
#define _LEA_LOCAL_H_

#include "lea.h"
#include "config.h"
#include <stdio.h>



#ifndef NO_SIMD
#include "cpu_info.h"

#if defined(__i386__) || defined(_M_IX86) || defined(_M_X64) || defined(__x86_64__)
#define ARCH_IA32
#endif

#if defined(__arm__) || defined(_M_ARM) || defined(_ARM) || defined(__arm) || defined(__aarch64__)
#define ARCH_ARM
#endif

#ifdef ARCH_IA32
#if (!defined(_MSC_VER) || _MSC_FULL_VER >= 180021114) && !defined(NO_AVX2)
#define COMPILE_AVX2
#endif

#if (!defined(_MSC_VER) || _MSC_FULL_VER >= 160040219) && !defined(NO_XOP)
#define COMPILE_XOP
#endif

#if (!defined(_MSC_VER) || _MSC_FULL_VER >= 150030729) && !defined(NO_PCLMUL)
#define COMPILE_PCLMUL
#endif

#if (!defined(_MSC_VER) || _MSC_VER >= 1250) && !defined(NO_SSE2)
#define COMPILE_SSE2
#endif 

#endif /* ARCH_IA32 */

#ifdef ARCH_ARM

#if (!defined(__GNUC__) || (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)) && !defined(NO_NEON)
#define COMPILE_NEON
#endif
#endif /* ARCH_ARM*/

extern info_ia32 g_info_ia32;
extern info_arm g_info_arm;

#endif /* NO_SIMD */

/*		#define USE_BUILT_IN	*/
#if (USE_BUILT_IN)
#if defined(_MSC_VER)
#include <stdlib.h>
#define ROR(W,i) _lrotr(W, i)
#define ROL(W,i) _lrotl(W, i)
#else	/*	#if defined(_MSC_VER)	*/
#define ROR(W,i) (((W) >> (i)) | ((W) << (32 - (i))))
#define ROL(W,i) (((W) << (i)) | ((W) >> (32 - (i))))
#endif	/*	#if defined(_MSC_VER)	*/
#include <string.h>
#define lea_memcpy		memcpy
#define lea_memset		memset
#define lea_memcmp		memcmp
#else	/*	#if (USE_BUILT_IN)	*/
#define ROR(W,i) (((W) >> (i)) | ((W) << (32 - (i))))
#define ROL(W,i) (((W) << (i)) | ((W) >> (32 - (i))))
void lea_memcpy(void *dst, void *src, int count);
void lea_memset(void *dst, int val, int count);
void lea_memcmp(void *src1, void *src2, int count);
#endif

/*		#define GCM_OPT_LEVEL	*/
/*	0 : 메모리를 추가적으로 사용하지 않습니다.(GCM 중 가장 느립니다.)
	1 : 4bit table을 사용합니다.(메모리를 추가로 약 0.3kb 더 사용합니다.		 0보다 약  5배 빠릅니다.)
	2 : 8bit table을 사용합니다.(메모리를 추가로 약 4.5kb 더 사용합니다.		 0보다 약 11배 빠릅니다.)
*/
#define GCM_OPT_LEVEL	2
//#define GCM_OP_LEVEL	1
//#define GCM_OP_LEVEL	0

//	endianess
#if IS_LITTLE_ENDIAN
//	little endian
#define ctow(w, c)	(*(w) = *((unsigned int *)(c)))
#define wtoc(c, w)	(*((unsigned int *)(c)) = *(w))
#define loadU32(v)	(v)

#else
//	big endian
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#if defined(__GNUC__) && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
#define loadU32(v)	__builtin_bswap32(v)
#define ctow(c,w)	(*(w) = __builtin_bswap32(*(unsigned int*)c))
#define wtoc(w,c)	(*(unsigned int*)(c) = __builtin_bswap32(*(w)))

#elif defined(__clang__) && defined(__has_builtin) && __has_builtin(__builtin_bswap32)

#define loadU32(v)	__builtin_bswap32(v)
#define ctow(c,w)	(*(w) = __builtin_bswap32(*(unsigned int*)c))
#define wtoc(w,c)	(*(unsigned int*)(c) = __builtin_bswap32(*(w)))

#else
	
#define loadU32(v)	((unsigned int)((((unsigned char*)(&v))[3]<<24)|(((unsigned char*)(&v))[2]<<16)|(((unsigned char*)(&v))[1]<<8)|(((unsigned char*)(&v))[0])))
#define ctow(c, w)	(*(w) = (((c)[3] << 24) | ((c)[2] << 16) | ((c)[1] << 8) | ((c)[0])))
#define wtoc(w, c)	((c)[0] = *(w), (c)[1] = (*(w) >> 8), (c)[2] = (*(w) >> 16), (c)[3] = (*(w) >> 24))
#endif

#endif



#define lea_assert(cond)	((cond) ? 0 : (return -1;))



/*****		cryptographic functions
*****/
void lea_encrypt(unsigned char *ct, const unsigned char *pt, const LEA_KEY *key);
void lea_decrypt(unsigned char *pt, const unsigned char *ct, const LEA_KEY *key);

/****** lea templates
******/
#ifndef NO_SIMD
#define MAKE_FUNC(NAME) MAKE_FUNC_G(NAME,SIMD_TYPE)
#define PASTER(x,y,z) x ## _ ## y ## _ ## z
#define MAKE_FUNC_G(NAME, SIMD) PASTER(lea,NAME,SIMD)
#else
#define PASTER(x,y) x ## _ ## y
#define MAKE_FUNC(NAME) PASTER(lea,NAME)
#endif

void lea_gcm_init(LEA_GCM_CTX *ctx, const unsigned char *mk, int mk_len);
void gcm_ghash(unsigned char *r, const unsigned char *x, unsigned int x_len, const unsigned char hTable[][16]);


#ifndef NO_SIMD
void lea_set_key_xop(LEA_KEY *key, const unsigned char *mk, unsigned int mk_len);
void lea_set_key_generic(LEA_KEY *key, const unsigned char *mk, unsigned int mk_len);
void lea_set_key_fallback(LEA_KEY *key, const unsigned char *mk, unsigned int mk_len);


typedef void(*lea_gcm_init_ptr)(LEA_GCM_CTX *ctx, const unsigned char *mk, int mk_len);
typedef void(*gcm_ghash_ptr)(unsigned char *r, const unsigned char *x, unsigned int x_len, const unsigned char hTable[][16]);
typedef void(*lea_set_key_ptr)(LEA_KEY *key, const unsigned char *mk, unsigned int mk_len);

typedef void(*lea_ecb_enc_ptr)(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const LEA_KEY *key);
typedef void(*lea_ecb_dec_ptr)(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const LEA_KEY *key);
typedef void(*lea_cbc_enc_ptr)(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
typedef void(*lea_cbc_dec_ptr)(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
typedef void(*lea_ctr_enc_ptr)(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *ctr, const LEA_KEY *key);
typedef void(*lea_ctr_dec_ptr)(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *ctr, const LEA_KEY *key);
typedef void(*lea_ofb_enc_ptr)(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *iv, const LEA_KEY *key);
typedef void(*lea_ofb_dec_ptr)(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *iv, const LEA_KEY *key);
typedef void(*lea_cfb128_enc_ptr)(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
typedef void(*lea_cfb128_dec_ptr)(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
typedef void(*lea_cmac_init_ptr)(LEA_CMAC_CTX *ctx, const unsigned char *mk, int mk_len);
typedef void(*lea_cmac_update_ptr)(LEA_CMAC_CTX *ctx, const unsigned char *data, int data_len);
typedef void(*lea_cmac_final_ptr)(LEA_CMAC_CTX *ctx, unsigned char *cmac, int cmac_len);
typedef int(*lea_ccm_enc_ptr)(unsigned char *ct, unsigned char *T, const unsigned char *pt, unsigned int pt_len, unsigned int Tlen,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
typedef int(*lea_ccm_dec_ptr)(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *T, unsigned int tag_bytes,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
typedef void(*lea_gcm_set_ctr_ptr)(LEA_GCM_CTX *ctx, const unsigned char *iv, int iv_len);
typedef void(*lea_gcm_set_aad_ptr)(LEA_GCM_CTX *ctx, const unsigned char *aad, int aad_len);
typedef void(*lea_gcm_enc_ptr)(LEA_GCM_CTX *ctx, unsigned char *ct, const unsigned char *pt, int pt_len);
typedef void(*lea_gcm_dec_ptr)(LEA_GCM_CTX *ctx, unsigned char *pt, const unsigned char *ct, int ct_len);
typedef int(*lea_gcm_final_ptr)(LEA_GCM_CTX *ctx, unsigned char *tag, int tag_len);

/* avx2_sse2 */
void lea_set_key_avx2_sse2(LEA_KEY *key, const unsigned char *mk, unsigned int mk_len);
void lea_ecb_enc_avx2_sse2(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const LEA_KEY *key);
void lea_ecb_dec_avx2_sse2(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const LEA_KEY *key);
void lea_cbc_enc_avx2_sse2(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cbc_dec_avx2_sse2(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
void lea_ctr_enc_avx2_sse2(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *ctr, const LEA_KEY *key);
void lea_ctr_dec_avx2_sse2(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *ctr, const LEA_KEY *key);
void lea_ofb_enc_avx2_sse2(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *iv, const LEA_KEY *key);
void lea_ofb_dec_avx2_sse2(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *iv, const LEA_KEY *key);
void lea_cfb128_enc_avx2_sse2(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cfb128_dec_avx2_sse2(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cmac_init_avx2_sse2(LEA_CMAC_CTX *ctx, const unsigned char *mk, int mk_len);
void lea_cmac_update_avx2_sse2(LEA_CMAC_CTX *ctx, const unsigned char *data, int data_len);
void lea_cmac_final_avx2_sse2(LEA_CMAC_CTX *ctx, unsigned char *cmac, int cmac_len);
int lea_ccm_enc_avx2_sse2(unsigned char *ct, unsigned char *T, const unsigned char *pt, unsigned int pt_len, unsigned int Tlen,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
int lea_ccm_dec_avx2_sse2(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *T, unsigned int tag_bytes,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
void lea_gcm_set_ctr_avx2_sse2(LEA_GCM_CTX *ctx, const unsigned char *iv, int iv_len);
void lea_gcm_set_aad_avx2_sse2(LEA_GCM_CTX *ctx, const unsigned char *aad, int aad_len);
void lea_gcm_enc_avx2_sse2(LEA_GCM_CTX *ctx, unsigned char *ct, const unsigned char *pt, int pt_len);
void lea_gcm_dec_avx2_sse2(LEA_GCM_CTX *ctx, unsigned char *pt, const unsigned char *ct, int ct_len);
int lea_gcm_final_avx2_sse2(LEA_GCM_CTX *ctx, unsigned char *tag, int tag_len);

/* avx2_xop */
void lea_ecb_enc_avx2_xop(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const LEA_KEY *key);
void lea_ecb_dec_avx2_xop(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const LEA_KEY *key);
void lea_cbc_enc_avx2_xop(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cbc_dec_avx2_xop(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
void lea_ctr_enc_avx2_xop(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *ctr, const LEA_KEY *key);
void lea_ctr_dec_avx2_xop(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *ctr, const LEA_KEY *key);
void lea_ofb_enc_avx2_xop(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *iv, const LEA_KEY *key);
void lea_ofb_dec_avx2_xop(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *iv, const LEA_KEY *key);
void lea_cfb128_enc_avx2_xop(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cfb128_dec_avx2_xop(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cmac_init_avx2_xop(LEA_CMAC_CTX *ctx, const unsigned char *mk, int mk_len);
void lea_cmac_update_avx2_xop(LEA_CMAC_CTX *ctx, const unsigned char *data, int data_len);
void lea_cmac_final_avx2_xop(LEA_CMAC_CTX *ctx, unsigned char *cmac, int cmac_len);
int lea_ccm_enc_avx2_xop(unsigned char *ct, unsigned char *T, const unsigned char *pt, unsigned int pt_len, unsigned int Tlen,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
int lea_ccm_dec_avx2_xop(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *T, unsigned int tag_bytes,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
void lea_gcm_set_ctr_avx2_xop(LEA_GCM_CTX *ctx, const unsigned char *iv, int iv_len);
void lea_gcm_set_aad_avx2_xop(LEA_GCM_CTX *ctx, const unsigned char *aad, int aad_len);
void lea_gcm_enc_avx2_xop(LEA_GCM_CTX *ctx, unsigned char *ct, const unsigned char *pt, int pt_len);
void lea_gcm_dec_avx2_xop(LEA_GCM_CTX *ctx, unsigned char *pt, const unsigned char *ct, int ct_len);
int lea_gcm_final_avx2_xop(LEA_GCM_CTX *ctx, unsigned char *tag, int tag_len);

/* sse2 */
void lea_set_key_sse2(LEA_KEY *key, const unsigned char *mk, unsigned int mk_len);
void lea_ecb_enc_sse2(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const LEA_KEY *key);
void lea_ecb_dec_sse2(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const LEA_KEY *key);
void lea_cbc_enc_sse2(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cbc_dec_sse2(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
void lea_ctr_enc_sse2(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *ctr, const LEA_KEY *key);
void lea_ctr_dec_sse2(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *ctr, const LEA_KEY *key);
void lea_ofb_enc_sse2(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *iv, const LEA_KEY *key);
void lea_ofb_dec_sse2(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *iv, const LEA_KEY *key);
void lea_cfb128_enc_sse2(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cfb128_dec_sse2(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cmac_init_sse2(LEA_CMAC_CTX *ctx, const unsigned char *mk, int mk_len);
void lea_cmac_update_sse2(LEA_CMAC_CTX *ctx, const unsigned char *data, int data_len);
void lea_cmac_final_sse2(LEA_CMAC_CTX *ctx, unsigned char *cmac, int cmac_len);
int lea_ccm_enc_sse2(unsigned char *ct, unsigned char *T, const unsigned char *pt, unsigned int pt_len, unsigned int Tlen,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
int lea_ccm_dec_sse2(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *T, unsigned int tag_bytes,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
void lea_gcm_set_ctr_sse2(LEA_GCM_CTX *ctx, const unsigned char *iv, int iv_len);
void lea_gcm_set_aad_sse2(LEA_GCM_CTX *ctx, const unsigned char *aad, int aad_len);
void lea_gcm_enc_sse2(LEA_GCM_CTX *ctx, unsigned char *ct, const unsigned char *pt, int pt_len);
void lea_gcm_dec_sse2(LEA_GCM_CTX *ctx, unsigned char *pt, const unsigned char *ct, int ct_len);
int lea_gcm_final_sse2(LEA_GCM_CTX *ctx, unsigned char *tag, int tag_len);

/* xop*/
void lea_ecb_enc_xop(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const LEA_KEY *key);
void lea_ecb_dec_xop(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const LEA_KEY *key);
void lea_cbc_enc_xop(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cbc_dec_xop(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
void lea_ctr_enc_xop(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *ctr, const LEA_KEY *key);
void lea_ctr_dec_xop(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *ctr, const LEA_KEY *key);
void lea_ofb_enc_xop(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *iv, const LEA_KEY *key);
void lea_ofb_dec_xop(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *iv, const LEA_KEY *key);
void lea_cfb128_enc_xop(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cfb128_dec_xop(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cmac_init_xop(LEA_CMAC_CTX *ctx, const unsigned char *mk, int mk_len);
void lea_cmac_update_xop(LEA_CMAC_CTX *ctx, const unsigned char *data, int data_len);
void lea_cmac_final_xop(LEA_CMAC_CTX *ctx, unsigned char *cmac, int cmac_len);
int lea_ccm_enc_xop(unsigned char *ct, unsigned char *T, const unsigned char *pt, unsigned int pt_len, unsigned int Tlen,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
int lea_ccm_dec_xop(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *T, unsigned int tag_bytes,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
void lea_gcm_set_ctr_xop(LEA_GCM_CTX *ctx, const unsigned char *iv, int iv_len);
void lea_gcm_set_aad_xop(LEA_GCM_CTX *ctx, const unsigned char *aad, int aad_len);
void lea_gcm_enc_xop(LEA_GCM_CTX *ctx, unsigned char *ct, const unsigned char *pt, int pt_len);
void lea_gcm_dec_xop(LEA_GCM_CTX *ctx, unsigned char *pt, const unsigned char *ct, int ct_len);
int lea_gcm_final_xop(LEA_GCM_CTX *ctx, unsigned char *tag, int tag_len);

/* neon*/
void lea_ecb_enc_neon(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const LEA_KEY *key);
void lea_ecb_dec_neon(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const LEA_KEY *key);
void lea_cbc_enc_neon(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cbc_dec_neon(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
void lea_ctr_enc_neon(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *ctr, const LEA_KEY *key);
void lea_ctr_dec_neon(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *ctr, const LEA_KEY *key);
void lea_ofb_enc_neon(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *iv, const LEA_KEY *key);
void lea_ofb_dec_neon(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *iv, const LEA_KEY *key);
void lea_cfb128_enc_neon(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cfb128_dec_neon(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cmac_init_neon(LEA_CMAC_CTX *ctx, const unsigned char *mk, int mk_len);
void lea_cmac_update_neon(LEA_CMAC_CTX *ctx, const unsigned char *data, int data_len);
void lea_cmac_final_neon(LEA_CMAC_CTX *ctx, unsigned char *cmac, int cmac_len);
int lea_ccm_enc_neon(unsigned char *ct, unsigned char *T, const unsigned char *pt, unsigned int pt_len, unsigned int Tlen,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
int lea_ccm_dec_neon(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *T, unsigned int tag_bytes,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
void lea_gcm_set_ctr_neon(LEA_GCM_CTX *ctx, const unsigned char *iv, int iv_len);
void lea_gcm_set_aad_neon(LEA_GCM_CTX *ctx, const unsigned char *aad, int aad_len);
void lea_gcm_enc_neon(LEA_GCM_CTX *ctx, unsigned char *ct, const unsigned char *pt, int pt_len);
void lea_gcm_dec_neon(LEA_GCM_CTX *ctx, unsigned char *pt, const unsigned char *ct, int ct_len);
int lea_gcm_final_neon(LEA_GCM_CTX *ctx, unsigned char *tag, int tag_len);

/* generic*/
void lea_ecb_enc_generic(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const LEA_KEY *key);
void lea_ecb_dec_generic(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const LEA_KEY *key);
void lea_cbc_enc_generic(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cbc_dec_generic(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
void lea_ctr_enc_generic(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *ctr, const LEA_KEY *key);
void lea_ctr_dec_generic(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *ctr, const LEA_KEY *key);
void lea_ofb_enc_generic(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *iv, const LEA_KEY *key);
void lea_ofb_dec_generic(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *iv, const LEA_KEY *key);
void lea_cfb128_enc_generic(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cfb128_dec_generic(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cmac_init_generic(LEA_CMAC_CTX *ctx, const unsigned char *mk, int mk_len);
void lea_cmac_update_generic(LEA_CMAC_CTX *ctx, const unsigned char *data, int data_len);
void lea_cmac_final_generic(LEA_CMAC_CTX *ctx, unsigned char *cmac, int cmac_len);
int lea_ccm_enc_generic(unsigned char *ct, unsigned char *T, const unsigned char *pt, unsigned int pt_len, unsigned int Tlen,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
int lea_ccm_dec_generic(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *T, unsigned int tag_bytes,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
void lea_gcm_set_ctr_generic(LEA_GCM_CTX *ctx, const unsigned char *iv, int iv_len);
void lea_gcm_set_aad_generic(LEA_GCM_CTX *ctx, const unsigned char *aad, int aad_len);
void lea_gcm_enc_generic(LEA_GCM_CTX *ctx, unsigned char *ct, const unsigned char *pt, int pt_len);
void lea_gcm_dec_generic(LEA_GCM_CTX *ctx, unsigned char *pt, const unsigned char *ct, int ct_len);
int lea_gcm_final_generic(LEA_GCM_CTX *ctx, unsigned char *tag, int tag_len);

/* gcm func */
void lea_gcm_init_generic(LEA_GCM_CTX *ctx, const unsigned char *mk, int mk_len);
void gcm_ghash_generic(unsigned char *r, const unsigned char *x, unsigned int x_len, const unsigned char hTable[][16]);

void lea_gcm_init_pclmul(LEA_GCM_CTX *ctx, const unsigned char *mk, int mk_len);
void gcm_ghash_pclmul(unsigned char *r, const unsigned char *x, unsigned int x_len, const unsigned char hTable[][16]);

void lea_gcm_init_fallback(LEA_GCM_CTX *ctx, const unsigned char *mk, int mk_len);
void gcm_ghash_fallback(unsigned char *r, const unsigned char *x, unsigned int x_len, const unsigned char hTable[][16]);

/* fallback */
void lea_ecb_enc_fallback(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const LEA_KEY *key);
void lea_ecb_dec_fallback(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const LEA_KEY *key);
void lea_cbc_enc_fallback(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cbc_dec_fallback(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
void lea_ctr_enc_fallback(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *ctr, const LEA_KEY *key);
void lea_ctr_dec_fallback(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *ctr, const LEA_KEY *key);
void lea_ofb_enc_fallback(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *iv, const LEA_KEY *key);
void lea_ofb_dec_fallback(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *iv, const LEA_KEY *key);
void lea_cfb128_enc_fallback(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cfb128_dec_fallback(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key);
void lea_cmac_init_fallback(LEA_CMAC_CTX *ctx, const unsigned char *mk, int mk_len);
void lea_cmac_update_fallback(LEA_CMAC_CTX *ctx, const unsigned char *data, int data_len);
void lea_cmac_final_fallback(LEA_CMAC_CTX *ctx, unsigned char *cmac, int cmac_len);
int lea_ccm_enc_fallback(unsigned char *ct, unsigned char *T, const unsigned char *pt, unsigned int pt_len, unsigned int Tlen,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
int lea_ccm_dec_fallback(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *T, unsigned int tag_bytes,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key);
void lea_gcm_init_fallback(LEA_GCM_CTX *ctx, const unsigned char *mk, int mk_len);
void lea_gcm_set_ctr_fallback(LEA_GCM_CTX *ctx, const unsigned char *iv, int iv_len);
void lea_gcm_set_aad_fallback(LEA_GCM_CTX *ctx, const unsigned char *aad, int aad_len);
void lea_gcm_enc_fallback(LEA_GCM_CTX *ctx, unsigned char *ct, const unsigned char *pt, int pt_len);
void lea_gcm_dec_fallback(LEA_GCM_CTX *ctx, unsigned char *pt, const unsigned char *ct, int ct_len);
int lea_gcm_final_fallback(LEA_GCM_CTX *ctx, unsigned char *tag, int tag_len);

#else

#define lea_set_key_generic lea_set_key


#define lea_ecb_enc_generic lea_ecb_enc
#define lea_ecb_dec_generic lea_ecb_dec
#define lea_cbc_enc_generic lea_cbc_enc
#define lea_cbc_dec_generic lea_cbc_dec
#define lea_ctr_enc_generic lea_ctr_enc
#define lea_ctr_dec_generic lea_ctr_dec
#define lea_ofb_enc_generic lea_ofb_enc
#define lea_ofb_dec_generic lea_ofb_dec
#define lea_cfb128_enc_generic lea_cfb128_enc
#define lea_cfb128_dec_generic lea_cfb128_dec
#define lea_cmac_init_generic lea_cmac_init
#define lea_cmac_update_generic lea_cmac_update
#define lea_cmac_final_generic lea_cmac_final
#define lea_ccm_enc_generic lea_ccm_enc
#define lea_ccm_dec_generic lea_ccm_dec
#define lea_gcm_set_ctr_generic lea_gcm_set_ctr
#define lea_gcm_set_aad_generic lea_gcm_set_aad
#define lea_gcm_enc_generic lea_gcm_enc
#define lea_gcm_dec_generic lea_gcm_dec
#define lea_gcm_final_generic lea_gcm_final

#define lea_gcm_init_generic lea_gcm_init
#define gcm_ghash_generic gcm_ghash
#endif


/*****		not built-in functions
*****/
#if defined(_M_X64) || defined(__x86_64__)
#define XOR8x16(r, a, b)																		\
	*((unsigned long long *)(r)      ) = *((unsigned long long *)(a)      ) ^ *((unsigned long long *)(b)      ),	\
	*((unsigned long long *)(r) + 0x1) = *((unsigned long long *)(a) + 0x1) ^ *((unsigned long long *)(b) + 0x1)
#elif defined(__i386__) || defined(_M_IX86)
#define XOR8x16(r, a, b)																		\
	*((unsigned int *)(r)      ) = *((unsigned int *)(a)      ) ^ *((unsigned int *)(b)      ),	\
	*((unsigned int *)(r) + 0x1) = *((unsigned int *)(a) + 0x1) ^ *((unsigned int *)(b) + 0x1),	\
	*((unsigned int *)(r) + 0x2) = *((unsigned int *)(a) + 0x2) ^ *((unsigned int *)(b) + 0x2),	\
	*((unsigned int *)(r) + 0x3) = *((unsigned int *)(a) + 0x3) ^ *((unsigned int *)(b) + 0x3)
#else
#define XOR8x16(r, a, b)				\
	*((r)      ) = *((a)      ) ^ *((b)      ),	\
	*((r) + 0x1) = *((a) + 0x1) ^ *((b) + 0x1),	\
	*((r) + 0x2) = *((a) + 0x2) ^ *((b) + 0x2),	\
	*((r) + 0x3) = *((a) + 0x3) ^ *((b) + 0x3),	\
	*((r) + 0x4) = *((a) + 0x4) ^ *((b) + 0x4),	\
	*((r) + 0x5) = *((a) + 0x5) ^ *((b) + 0x5),	\
	*((r) + 0x6) = *((a) + 0x6) ^ *((b) + 0x6),	\
	*((r) + 0x7) = *((a) + 0x7) ^ *((b) + 0x7),	\
	*((r) + 0x8) = *((a) + 0x8) ^ *((b) + 0x8),	\
	*((r) + 0x9) = *((a) + 0x9) ^ *((b) + 0x9),	\
	*((r) + 0xa) = *((a) + 0xa) ^ *((b) + 0xa),	\
	*((r) + 0xb) = *((a) + 0xb) ^ *((b) + 0xb),	\
	*((r) + 0xc) = *((a) + 0xc) ^ *((b) + 0xc),	\
	*((r) + 0xd) = *((a) + 0xd) ^ *((b) + 0xd),	\
	*((r) + 0xe) = *((a) + 0xe) ^ *((b) + 0xe),	\
	*((r) + 0xf) = *((a) + 0xf) ^ *((b) + 0xf)
#endif

#define XOR8x64(r, a, b)				\
	XOR8x16((r)       , (a)       , (b)       ),	\
	XOR8x16((r) + 0x10, (a) + 0x10, (b) + 0x10),	\
	XOR8x16((r) + 0x20, (a) + 0x20, (b) + 0x20),	\
	XOR8x16((r) + 0x30, (a) + 0x30, (b) + 0x30)

#define XOR8x128(r, a, b)				\
	XOR8x64((r)       , (a)       , (b)       ),	\
	XOR8x64((r) + 0x40, (a) + 0x40, (b) + 0x40)

#define XOR8x16r(r, a, b)				\
	*((r) + 0xf) = *((a) + 0xf) ^ *((b) + 0xf),	\
	*((r) + 0xe) = *((a) + 0xe) ^ *((b) + 0xe),	\
	*((r) + 0xd) = *((a) + 0xd) ^ *((b) + 0xd),	\
	*((r) + 0xc) = *((a) + 0xc) ^ *((b) + 0xc),	\
	*((r) + 0xb) = *((a) + 0xb) ^ *((b) + 0xb),	\
	*((r) + 0xa) = *((a) + 0xa) ^ *((b) + 0xa),	\
	*((r) + 0x9) = *((a) + 0x9) ^ *((b) + 0x9),	\
	*((r) + 0x8) = *((a) + 0x8) ^ *((b) + 0x8),	\
	*((r) + 0x7) = *((a) + 0x7) ^ *((b) + 0x7),	\
	*((r) + 0x6) = *((a) + 0x6) ^ *((b) + 0x6),	\
	*((r) + 0x5) = *((a) + 0x5) ^ *((b) + 0x5),	\
	*((r) + 0x4) = *((a) + 0x4) ^ *((b) + 0x4),	\
	*((r) + 0x3) = *((a) + 0x3) ^ *((b) + 0x3),	\
	*((r) + 0x2) = *((a) + 0x2) ^ *((b) + 0x2),	\
	*((r) + 0x1) = *((a) + 0x1) ^ *((b) + 0x1),	\
	*((r)      ) = *((a)      ) ^ *((b)      )

#define XOR8x64r(r, a, b)				\
	XOR8x16r((r) + 0x30, (a) + 0x30, (b) + 0x30),	\
	XOR8x16r((r) + 0x20, (a) + 0x20, (b) + 0x20),	\
	XOR8x16r((r) + 0x10, (a) + 0x10, (b) + 0x10),	\
	XOR8x16r((r)       , (a)       , (b)       )

#define XOR8x128r(r, a, b)				\
	XOR8x64r((r) + 0x40, (a) + 0x40, (b) + 0x40),	\
	XOR8x64r((r)       , (a)       , (b)       )

#define	XOR32x4(d, a, b)							\
	*((d)    ) = *((a)    ) ^ *((b)    ),			\
	*((d) + 1) = *((a) + 1) ^ *((b) + 1),			\
	*((d) + 2) = *((a) + 2) ^ *((b) + 2),			\
	*((d) + 3) = *((a) + 3) ^ *((b) + 3)

#define CPY8x12(d, s)											\
	*((unsigned int *)(d)) = *((unsigned int *)(s)),			\
	*((unsigned int *)(d) + 1) = *((unsigned int *)(s) + 1),	\
	*((unsigned int *)(d) + 2) = *((unsigned int *)(s) + 2)

#define CPY8x16(d, s)											\
	*((unsigned int *)(d)) = *((unsigned int *)(s)),			\
	*((unsigned int *)(d) + 1) = *((unsigned int *)(s) + 1),	\
	*((unsigned int *)(d) + 2) = *((unsigned int *)(s) + 2),	\
	*((unsigned int *)(d) + 3) = *((unsigned int *)(s) + 3)

#define ZERO8x16(a)																				\
	(*((unsigned int *)(a)) = 0, *((unsigned int *)(a) + 1) = 0, *((unsigned int *)(a) + 2) = 0, *((unsigned int *)(a) + 3) = 0);

#define RSHIFT32x4(r, a, bit)								\
	(r)[3] = ((a)[3] >> (bit)) | ((a)[2] << (32 - (bit))),	\
	(r)[2] = ((a)[2] >> (bit)) | ((a)[1] << (32 - (bit))),	\
	(r)[1] = ((a)[1] >> (bit)) | ((a)[0] << (32 - (bit))),	\
	(r)[0] = ((a)[0] >> (bit))

#define RSHIFT8x16_1(v)								\
	(v)[15] = ((v)[15] >> 1) | ((v)[14] << 7),		\
	(v)[14] = ((v)[14] >> 1) | ((v)[13] << 7),		\
	(v)[13] = ((v)[13] >> 1) | ((v)[12] << 7),		\
	(v)[12] = ((v)[12] >> 1) | ((v)[11] << 7),		\
	(v)[11] = ((v)[11] >> 1) | ((v)[10] << 7),		\
	(v)[10] = ((v)[10] >> 1) | ((v)[ 9] << 7),		\
	(v)[ 9] = ((v)[ 9] >> 1) | ((v)[ 8] << 7),		\
	(v)[ 8] = ((v)[ 8] >> 1) | ((v)[ 7] << 7),		\
	(v)[ 7] = ((v)[ 7] >> 1) | ((v)[ 6] << 7),		\
	(v)[ 6] = ((v)[ 6] >> 1) | ((v)[ 5] << 7),		\
	(v)[ 5] = ((v)[ 5] >> 1) | ((v)[ 4] << 7),		\
	(v)[ 4] = ((v)[ 4] >> 1) | ((v)[ 3] << 7),		\
	(v)[ 3] = ((v)[ 3] >> 1) | ((v)[ 2] << 7),		\
	(v)[ 2] = ((v)[ 2] >> 1) | ((v)[ 1] << 7),		\
	(v)[ 1] = ((v)[ 1] >> 1) | ((v)[ 0] << 7),		\
	(v)[ 0] = ((v)[ 0] >> 1)

#define RSHIFT8x16_4(v)								\
	(v)[15] = ((v)[15] >> 4) | ((v)[14] << 4),		\
	(v)[14] = ((v)[14] >> 4) | ((v)[13] << 4),		\
	(v)[13] = ((v)[13] >> 4) | ((v)[12] << 4),		\
	(v)[12] = ((v)[12] >> 4) | ((v)[11] << 4),		\
	(v)[11] = ((v)[11] >> 4) | ((v)[10] << 4),		\
	(v)[10] = ((v)[10] >> 4) | ((v)[ 9] << 4),		\
	(v)[ 9] = ((v)[ 9] >> 4) | ((v)[ 8] << 4),		\
	(v)[ 8] = ((v)[ 8] >> 4) | ((v)[ 7] << 4),		\
	(v)[ 7] = ((v)[ 7] >> 4) | ((v)[ 6] << 4),		\
	(v)[ 6] = ((v)[ 6] >> 4) | ((v)[ 5] << 4),		\
	(v)[ 5] = ((v)[ 5] >> 4) | ((v)[ 4] << 4),		\
	(v)[ 4] = ((v)[ 4] >> 4) | ((v)[ 3] << 4),		\
	(v)[ 3] = ((v)[ 3] >> 4) | ((v)[ 2] << 4),		\
	(v)[ 2] = ((v)[ 2] >> 4) | ((v)[ 1] << 4),		\
	(v)[ 1] = ((v)[ 1] >> 4) | ((v)[ 0] << 4),		\
	(v)[ 0] = ((v)[ 0] >> 4)

#define RSHIFT8x16_8(v)							\
	(v)[15] = (v)[14],		\
	(v)[14] = (v)[13],		\
	(v)[13] = (v)[12],		\
	(v)[12] = (v)[11],		\
	(v)[11] = (v)[10],		\
	(v)[10] = (v)[ 9],		\
	(v)[ 9] = (v)[ 8],		\
	(v)[ 8] = (v)[ 7],		\
	(v)[ 7] = (v)[ 6],		\
	(v)[ 6] = (v)[ 5],		\
	(v)[ 5] = (v)[ 4],		\
	(v)[ 4] = (v)[ 3],		\
	(v)[ 3] = (v)[ 2],		\
	(v)[ 2] = (v)[ 1],		\
	(v)[ 1] = (v)[ 0],		\
	(v)[ 0] = 0



#endif	//_LEA_LOCAL_H_
