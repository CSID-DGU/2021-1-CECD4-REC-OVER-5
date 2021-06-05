#include "lea.h"
#include "lea_locl.h"

#ifndef NO_SIMD
info_ia32 g_info_ia32 = { 0, };
info_arm g_info_arm = { 0, };

lea_gcm_init_ptr g_gcm_init = lea_gcm_init_fallback;
gcm_ghash_ptr g_ghash = gcm_ghash_fallback;
lea_set_key_ptr g_set_key = lea_set_key_fallback;

lea_ecb_enc_ptr g_ecb_enc = lea_ecb_enc_fallback;
lea_ecb_dec_ptr g_ecb_dec = lea_ecb_dec_fallback;
lea_cbc_enc_ptr g_cbc_enc = lea_cbc_enc_fallback;
lea_cbc_dec_ptr g_cbc_dec = lea_cbc_dec_fallback;
lea_ctr_enc_ptr g_ctr_enc = lea_ctr_enc_fallback;
lea_ctr_dec_ptr g_ctr_dec = lea_ctr_dec_fallback;
lea_ofb_enc_ptr g_ofb_enc = lea_ofb_enc_fallback;
lea_ofb_dec_ptr g_ofb_dec = lea_ofb_dec_fallback;
lea_cfb128_enc_ptr g_cfb128_enc = lea_cfb128_enc_fallback;
lea_cfb128_dec_ptr g_cfb128_dec = lea_cfb128_dec_fallback;
lea_cmac_init_ptr g_cmac_init = lea_cmac_init_fallback;
lea_cmac_update_ptr g_cmac_update = lea_cmac_update_fallback;
lea_cmac_final_ptr g_cmac_final = lea_cmac_final_fallback;
lea_ccm_enc_ptr g_ccm_enc = lea_ccm_enc_fallback;
lea_ccm_dec_ptr g_ccm_dec = lea_ccm_dec_fallback;
lea_gcm_set_ctr_ptr g_gcm_set_ctr = lea_gcm_set_ctr_fallback;
lea_gcm_set_aad_ptr g_gcm_set_aad = lea_gcm_set_aad_fallback;
lea_gcm_enc_ptr g_gcm_enc = lea_gcm_enc_fallback;
lea_gcm_dec_ptr g_gcm_dec = lea_gcm_dec_fallback;
lea_gcm_final_ptr g_gcm_final = lea_gcm_final_fallback;

static const char * g_cszSIMD = "WARNING : init_simd() is not called.";
/* Auto Initialization */
#ifdef __cplusplus
#define INITIALIZER(f) \
	struct _intl_construct { _intl_construct(void(*fx)(void)){fx();}};\
	static void f(void); static _intl_construct _intl_construct_##f(f); \
	static void f(void)
#elif defined(_MSC_VER)
#define CCALL __cdecl
#pragma section(".CRT$XCU",read)
#define INITIALIZER(f) \
	static void __cdecl f(void); \
	__declspec(allocate(".CRT$XCU")) void (__cdecl*f##_)(void) = f; \
	static void __cdecl f(void)
#elif defined(__GNUC__)

#define CCALL
#define INITIALIZER(f) \
	static void f(void) __attribute__((constructor)); \
	static void f(void)
#endif

INITIALIZER(autoinit_simd)
{
	init_simd();
}

const char * get_simd_type(){
	/* Avoid for initialization codes to be optimized out. */
#if !defined(__cplusplus) && defined(_MSC_VER)
	void(*f)() = autoinit_simd_;
#elif defined(__GNUC__)
	void(*f)() = autoinit_simd;
#else
	void(*f)() = init_simd;
#endif
	if (g_ecb_enc == lea_ecb_enc_fallback){
		//Fallback autoinit_simd
		f();
		//assert(FALSE);
	}

	return g_cszSIMD;
}

void init_simd(){
#ifdef ARCH_IA32
	get_ia32_cpuinfo(&g_info_ia32, 1);

	if (!g_info_ia32.sse2){
		g_info_ia32.pclmul = 0;
		g_info_ia32.xop = 0;
		g_info_ia32.avx2 = 0;
	}

	if (0){}
#ifdef COMPILE_PCLMUL
	else if (g_info_ia32.pclmul){
		g_gcm_init = lea_gcm_init_pclmul;
		g_ghash = gcm_ghash_pclmul;
	}
#endif
	else{
		g_gcm_init = lea_gcm_init_generic;
		g_ghash = gcm_ghash_generic;
	}

	if (0){}
#ifdef COMPILE_SSE2
#ifdef COMPILE_AVX2
#ifdef COMPILE_XOP
	else if (g_info_ia32.avx2 && g_info_ia32.xop){
		if (g_info_ia32.pclmul){
			g_cszSIMD = "avx2,xop,pclmul";
		}
		else{
			g_cszSIMD = "avx2,xop";
		}

		g_set_key = lea_set_key_xop;
		
		g_ecb_enc = lea_ecb_enc_avx2_xop;
		g_ecb_dec = lea_ecb_dec_avx2_xop;
		g_cbc_enc = lea_cbc_enc_avx2_xop;
		g_cbc_dec = lea_cbc_dec_avx2_xop;
		g_ctr_enc = lea_ctr_enc_avx2_xop;
		g_ctr_dec = lea_ctr_dec_avx2_xop;
		g_ofb_enc = lea_ofb_enc_avx2_xop;
		g_ofb_dec = lea_ofb_dec_avx2_xop;
		g_cfb128_enc = lea_cfb128_enc_avx2_xop;
		g_cfb128_dec = lea_cfb128_dec_avx2_xop;
		g_cmac_init = lea_cmac_init_avx2_xop;
		g_cmac_update = lea_cmac_update_avx2_xop;
		g_cmac_final = lea_cmac_final_avx2_xop;
		g_ccm_enc = lea_ccm_enc_avx2_xop;
		g_ccm_dec = lea_ccm_dec_avx2_xop;
		g_gcm_set_ctr = lea_gcm_set_ctr_avx2_xop;
		g_gcm_set_aad = lea_gcm_set_aad_avx2_xop;
		g_gcm_enc = lea_gcm_enc_avx2_xop;
		g_gcm_dec = lea_gcm_dec_avx2_xop;
		g_gcm_final = lea_gcm_final_avx2_xop;
	}
#endif //XOP
	else if (g_info_ia32.avx2){
		if (0){}
#ifdef COMPILE_PCLMUL
		else if (g_info_ia32.pclmul){
			g_cszSIMD = "avx2,sse2,pclmul";
		}
#endif //PCLMUL
		else{
			g_cszSIMD = "avx2,sse2";
		}
		g_set_key = lea_set_key_avx2_sse2;

		g_ecb_enc = lea_ecb_enc_avx2_sse2;
		g_ecb_dec = lea_ecb_dec_avx2_sse2;
		g_cbc_enc = lea_cbc_enc_avx2_sse2;
		g_cbc_dec = lea_cbc_dec_avx2_sse2;
		g_ctr_enc = lea_ctr_enc_avx2_sse2;
		g_ctr_dec = lea_ctr_dec_avx2_sse2;
		g_ofb_enc = lea_ofb_enc_avx2_sse2;
		g_ofb_dec = lea_ofb_dec_avx2_sse2;
		g_cfb128_enc = lea_cfb128_enc_avx2_sse2;
		g_cfb128_dec = lea_cfb128_dec_avx2_sse2;
		g_cmac_init = lea_cmac_init_avx2_sse2;
		g_cmac_update = lea_cmac_update_avx2_sse2;
		g_cmac_final = lea_cmac_final_avx2_sse2;
		g_ccm_enc = lea_ccm_enc_avx2_sse2;
		g_ccm_dec = lea_ccm_dec_avx2_sse2;
		g_gcm_set_ctr = lea_gcm_set_ctr_avx2_sse2;
		g_gcm_set_aad = lea_gcm_set_aad_avx2_sse2;
		g_gcm_enc = lea_gcm_enc_avx2_sse2;
		g_gcm_dec = lea_gcm_dec_avx2_sse2;
		g_gcm_final = lea_gcm_final_avx2_sse2;
	}
#endif //AVX2
#ifdef COMPILE_XOP
	else if(g_info_ia32.xop){
		if (0){}
#ifdef COMPILE_PCLMUL
		else if (g_info_ia32.pclmul){
			g_cszSIMD = "xop,pclmul";
		}
#endif //PCLMUL
		else{
			g_cszSIMD = "xop";
		}
		g_set_key = lea_set_key_xop;

		g_ecb_enc = lea_ecb_enc_xop;
		g_ecb_dec = lea_ecb_dec_xop;
		g_cbc_enc = lea_cbc_enc_xop;
		g_cbc_dec = lea_cbc_dec_xop;
		g_ctr_enc = lea_ctr_enc_xop;
		g_ctr_dec = lea_ctr_dec_xop;
		g_ofb_enc = lea_ofb_enc_xop;
		g_ofb_dec = lea_ofb_dec_xop;
		g_cfb128_enc = lea_cfb128_enc_xop;
		g_cfb128_dec = lea_cfb128_dec_xop;
		g_cmac_init = lea_cmac_init_xop;
		g_cmac_update = lea_cmac_update_xop;
		g_cmac_final = lea_cmac_final_xop;
		g_ccm_enc = lea_ccm_enc_xop;
		g_ccm_dec = lea_ccm_dec_xop;
		g_gcm_set_ctr = lea_gcm_set_ctr_xop;
		g_gcm_set_aad = lea_gcm_set_aad_xop;
		g_gcm_enc = lea_gcm_enc_xop;
		g_gcm_dec = lea_gcm_dec_xop;
		g_gcm_final = lea_gcm_final_xop;
	}
#endif
	else if (g_info_ia32.sse2){
		if (0){}
#ifdef COMPILE_PCLMUL
		else if (g_info_ia32.pclmul){
			g_cszSIMD = "sse2,pclmul";
		}
#endif
		else{
			g_cszSIMD = "sse2";
		}
		g_set_key = lea_set_key_sse2;

		g_ecb_enc = lea_ecb_enc_sse2;
		g_ecb_dec = lea_ecb_dec_sse2;
		g_cbc_enc = lea_cbc_enc_sse2;
		g_cbc_dec = lea_cbc_dec_sse2;
		g_ctr_enc = lea_ctr_enc_sse2;
		g_ctr_dec = lea_ctr_dec_sse2;
		g_ofb_enc = lea_ofb_enc_sse2;
		g_ofb_dec = lea_ofb_dec_sse2;
		g_cfb128_enc = lea_cfb128_enc_sse2;
		g_cfb128_dec = lea_cfb128_dec_sse2;
		g_cmac_init = lea_cmac_init_sse2;
		g_cmac_update = lea_cmac_update_sse2;
		g_cmac_final = lea_cmac_final_sse2;
		g_ccm_enc = lea_ccm_enc_sse2;
		g_ccm_dec = lea_ccm_dec_sse2;
		g_gcm_set_ctr = lea_gcm_set_ctr_sse2;
		g_gcm_set_aad = lea_gcm_set_aad_sse2;
		g_gcm_enc = lea_gcm_enc_sse2;
		g_gcm_dec = lea_gcm_dec_sse2;
		g_gcm_final = lea_gcm_final_sse2;
	}
#endif
#elif defined(ARCH_ARM)
	get_arm_cpuinfo(&g_info_arm);
	if(0){}
#ifdef COMPILE_NEON
	else if (g_info_arm.neon){
		g_cszSIMD = "neon";

		g_set_key = lea_set_key_generic;
		g_gcm_init = lea_gcm_init_generic;
		g_ghash = gcm_ghash_generic;

		g_ecb_enc = lea_ecb_enc_neon;
		g_ecb_dec = lea_ecb_dec_neon;
		g_cbc_enc = lea_cbc_enc_neon;
		g_cbc_dec = lea_cbc_dec_neon;
		g_ctr_enc = lea_ctr_enc_neon;
		g_ctr_dec = lea_ctr_dec_neon;
		g_ofb_enc = lea_ofb_enc_neon;
		g_ofb_dec = lea_ofb_dec_neon;
		g_cfb128_enc = lea_cfb128_enc_neon;
		g_cfb128_dec = lea_cfb128_dec_neon;
		g_cmac_init = lea_cmac_init_neon;
		g_cmac_update = lea_cmac_update_neon;
		g_cmac_final = lea_cmac_final_neon;
		g_ccm_enc = lea_ccm_enc_neon;
		g_ccm_dec = lea_ccm_dec_neon;
		g_gcm_set_ctr = lea_gcm_set_ctr_neon;
		g_gcm_set_aad = lea_gcm_set_aad_neon;
		g_gcm_enc = lea_gcm_enc_neon;
		g_gcm_dec = lea_gcm_dec_neon;
		g_gcm_final = lea_gcm_final_neon;
	}
#endif
#endif
	else{
		g_cszSIMD = "generic";
		g_set_key = lea_set_key_generic;

		g_ecb_enc = lea_ecb_enc_generic;
		g_ecb_dec = lea_ecb_dec_generic;
		g_cbc_enc = lea_cbc_enc_generic;
		g_cbc_dec = lea_cbc_dec_generic;
		g_ctr_enc = lea_ctr_enc_generic;
		g_ctr_dec = lea_ctr_dec_generic;
		g_ofb_enc = lea_ofb_enc_generic;
		g_ofb_dec = lea_ofb_dec_generic;
		g_cfb128_enc = lea_cfb128_enc_generic;
		g_cfb128_dec = lea_cfb128_dec_generic;
		g_cmac_init = lea_cmac_init_generic;
		g_cmac_update = lea_cmac_update_generic;
		g_cmac_final = lea_cmac_final_generic;
		g_ccm_enc = lea_ccm_enc_generic;
		g_ccm_dec = lea_ccm_dec_generic;
		g_gcm_set_ctr = lea_gcm_set_ctr_generic;
		g_gcm_set_aad = lea_gcm_set_aad_generic;
		g_gcm_enc = lea_gcm_enc_generic;
		g_gcm_dec = lea_gcm_dec_generic;
		g_gcm_final = lea_gcm_final_generic;
	}
}





void lea_set_key(LEA_KEY *key, const unsigned char *mk, unsigned int mk_len)
{
#if defined(ARCH_IA32)
	g_set_key(key, mk, mk_len);
#else
	lea_set_key_generic(key, mk, mk_len);
#endif
}

void lea_gcm_init(LEA_GCM_CTX *ctx, const unsigned char *mk, int mk_len){
#if defined(ARCH_IA32)
	g_gcm_init(ctx, mk, mk_len);
#else
	lea_gcm_init_generic(ctx, mk, mk_len);
#endif
}
void gcm_ghash(unsigned char *r, const unsigned char *x, unsigned int x_len, const unsigned char hTable[][16]){
#if defined(ARCH_IA32)
	g_ghash(r, x, x_len, hTable);
#else
	gcm_ghash_generic(r, x, x_len, hTable);
#endif
}

void lea_ecb_enc(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const LEA_KEY *key){
	g_ecb_enc(ct, pt, pt_len, key);
}

void lea_ecb_dec(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const LEA_KEY *key){
	g_ecb_dec(pt, ct, ct_len, key);
}
void lea_cbc_enc(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key){
	g_cbc_enc(ct, pt, pt_len, iv, key);
}
void lea_cbc_dec(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key){
	g_cbc_dec(pt, ct, ct_len, iv, key);
}
void lea_ctr_enc(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *ctr, const LEA_KEY *key){
	g_ctr_enc(ct, pt, pt_len, ctr, key);
}
void lea_ctr_dec(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *ctr, const LEA_KEY *key){
	g_ctr_dec(pt, ct, ct_len, ctr, key);
}
void lea_ofb_enc(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *iv, const LEA_KEY *key){
	g_ofb_enc(ct, pt, pt_len, iv, key);
}
void lea_ofb_dec(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *iv, const LEA_KEY *key){
	g_ofb_dec(pt, ct, ct_len, iv, key);
}
void lea_cfb128_enc(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key){
	g_cfb128_enc(ct, pt, pt_len, iv, key);
}
void lea_cfb128_dec(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key){
	g_cfb128_dec(pt, ct, ct_len, iv, key);
}
void lea_cmac_init(LEA_CMAC_CTX *ctx, const unsigned char *mk, int mk_len){
	g_cmac_init(ctx, mk, mk_len);

}
void lea_cmac_update(LEA_CMAC_CTX *ctx, const unsigned char *data, int data_len){
	g_cmac_update(ctx, data, data_len);
}
void lea_cmac_final(LEA_CMAC_CTX *ctx, unsigned char *cmac, int cmac_len){
	g_cmac_final(ctx, cmac, cmac_len);
}
int lea_ccm_enc(unsigned char *ct, unsigned char *T, const unsigned char *pt, unsigned int pt_len, unsigned int Tlen,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key){
	return g_ccm_enc(ct, T, pt, pt_len, Tlen, N, Nlen, A, Alen, key);

}
int lea_ccm_dec(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *T, unsigned int tag_bytes,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key){
	return g_ccm_dec(pt, ct, ct_len, T, tag_bytes, N, Nlen, A, Alen, key);
}
void lea_gcm_set_ctr(LEA_GCM_CTX *ctx, const unsigned char *iv, int iv_len){
	g_gcm_set_ctr(ctx, iv, iv_len);
}
void lea_gcm_set_aad(LEA_GCM_CTX *ctx, const unsigned char *aad, int aad_len){
	g_gcm_set_aad(ctx, aad, aad_len);
}
void lea_gcm_enc(LEA_GCM_CTX *ctx, unsigned char *ct, const unsigned char *pt, int pt_len){
	g_gcm_enc(ctx, ct, pt, pt_len);
}
void lea_gcm_dec(LEA_GCM_CTX *ctx, unsigned char *pt, const unsigned char *ct, int ct_len){
	g_gcm_dec(ctx, pt, ct, ct_len);
}
int lea_gcm_final(LEA_GCM_CTX *ctx, unsigned char *tag, int tag_len){
	return g_gcm_final(ctx, tag, tag_len);
}

#else
void init_simd(){
}

const char * get_simd_type(){
	return "generic";
}

#endif

#if !(USE_BUILT_IN)

void lea_memcpy(void *dst, void *src, int count)
{
	char *pDst = (char *)dst;
	char *pSrc = (char *)src;
	
	for(count--; count >= 0; count--)
		pDst[count] = pSrc[count];
}

void lea_memset(void *dst, int val, int count)
{
	char *pDst = (char *)dst;

	for(count--; count >= 0; count--)
		pDst[count] = val;
}

void lea_memcmp(void *src1, void *src2, int count)
{
	char *pSrc1 = (char *)src1;
	char *pSrc2 = (char *)src2;
	int i;

	for(i = 0; i < count; i++)
		if(pSrc1[i] != pSrc2[i])
			return i;

	return 0;
}

#endif
