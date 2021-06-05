#include "lea.h"
#include "lea_locl.h"

void lea_set_key_fallback(LEA_KEY *key, const unsigned char *mk, unsigned int mk_len)
{
	init_simd();
	lea_set_key(key, mk, mk_len);
}

void lea_gcm_init_fallback(LEA_GCM_CTX *ctx, const unsigned char *mk, int mk_len){
	init_simd();
	lea_gcm_init(ctx, mk, mk_len);
}
void gcm_ghash_fallback(unsigned char *r, const unsigned char *x, unsigned int x_len, const unsigned char hTable[][16]){
	init_simd();
	gcm_ghash(r, x, x_len, hTable);
}

void lea_ecb_enc_fallback(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const LEA_KEY *key){
	init_simd();
	lea_ecb_enc(ct, pt, pt_len, key);
}

void lea_ecb_dec_fallback(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const LEA_KEY *key){
	init_simd();
	lea_ecb_dec(pt, ct, ct_len, key);
}
void lea_cbc_enc_fallback(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key){
	init_simd();
	lea_cbc_enc(ct, pt, pt_len, iv, key);
}
void lea_cbc_dec_fallback(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key){
	init_simd();
	lea_cbc_dec(pt, ct, ct_len, iv, key);
}
void lea_ctr_enc_fallback(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *ctr, const LEA_KEY *key){
	init_simd();
	lea_ctr_enc(ct, pt, pt_len, ctr, key);
}
void lea_ctr_dec_fallback(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *ctr, const LEA_KEY *key){
	init_simd();
	lea_ctr_dec(pt, ct, ct_len, ctr, key);
}
void lea_ofb_enc_fallback(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *iv, const LEA_KEY *key){
	init_simd();
	lea_ofb_enc(ct, pt, pt_len, iv, key);
}
void lea_ofb_dec_fallback(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *iv, const LEA_KEY *key){
	init_simd();
	lea_ofb_dec(pt, ct, ct_len, iv, key);
}
void lea_cfb128_enc_fallback(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key){
	init_simd();
	lea_cfb128_enc(ct, pt, pt_len, iv, key);
}
void lea_cfb128_dec_fallback(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key){
	init_simd();
	lea_cfb128_dec(pt, ct, ct_len, iv, key);
}
void lea_cmac_init_fallback(LEA_CMAC_CTX *ctx, const unsigned char *mk, int mk_len){
	init_simd();
	lea_cmac_init(ctx, mk, mk_len);

}
void lea_cmac_update_fallback(LEA_CMAC_CTX *ctx, const unsigned char *data, int data_len){
	init_simd();
	lea_cmac_update(ctx, data, data_len);
}
void lea_cmac_final_fallback(LEA_CMAC_CTX *ctx, unsigned char *cmac, int cmac_len){
	init_simd();
	lea_cmac_final(ctx, cmac, cmac_len);
}
int lea_ccm_enc_fallback(unsigned char *ct, unsigned char *T, const unsigned char *pt, unsigned int pt_len, unsigned int Tlen,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key){
	init_simd();
	return lea_ccm_enc(ct, T, pt, pt_len, Tlen, N, Nlen, A, Alen, key);

}
int lea_ccm_dec_fallback(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *T, unsigned int tag_bytes,
	const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key){
	init_simd();
	return lea_ccm_dec(pt, ct, ct_len, T, tag_bytes, N, Nlen, A, Alen, key);
}
void lea_gcm_set_ctr_fallback(LEA_GCM_CTX *ctx, const unsigned char *iv, int iv_len){
	init_simd();
	lea_gcm_set_ctr(ctx, iv, iv_len);
}
void lea_gcm_set_aad_fallback(LEA_GCM_CTX *ctx, const unsigned char *aad, int aad_len){
	init_simd();
	lea_gcm_set_aad(ctx, aad, aad_len);
}
void lea_gcm_enc_fallback(LEA_GCM_CTX *ctx, unsigned char *ct, const unsigned char *pt, int pt_len){
	init_simd();
	lea_gcm_enc(ctx, ct, pt, pt_len);
}
void lea_gcm_dec_fallback(LEA_GCM_CTX *ctx, unsigned char *pt, const unsigned char *ct, int ct_len){
	init_simd();
	lea_gcm_dec(ctx, pt, ct, ct_len);
}
int lea_gcm_final_fallback(LEA_GCM_CTX *ctx, unsigned char *tag, int tag_len){
	init_simd();
	return lea_gcm_final(ctx, tag, tag_len);
}
