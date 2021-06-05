#include "lea.h"
#include "lea_locl.h"

void MAKE_FUNC(gcm_set_ctr)(LEA_GCM_CTX *ctx, const unsigned char *iv, int iv_len)
{
	int tmp_iv_len = iv_len;
	
	if (!ctx || !iv){
		return;
	}
	if (iv_len < 0){
		return;
	}

	ctx->ct_len = 0;

	if(iv_len == 12)
	{
		memcpy(ctx->ctr, iv, 12);
		ctx->ctr[15] = 1;
	}
	else
	{
		for(; iv_len >= 16; iv_len -= 16, iv += 16)
			gcm_ghash(ctx->ctr, iv, 16, (const unsigned char(*)[16])ctx->h);

		if(iv_len)
			gcm_ghash(ctx->ctr, iv, iv_len, (const unsigned char(*)[16])ctx->h);

		tmp_iv_len <<= 3;
		ctx->yn[12] = (tmp_iv_len >> 24) & 0xff;
		ctx->yn[13] = (tmp_iv_len >> 16) & 0xff;
		ctx->yn[14] = (tmp_iv_len >>  8) & 0xff;
		ctx->yn[15] = (tmp_iv_len      ) & 0xff;
		gcm_ghash(ctx->ctr, ctx->yn, 16, (const unsigned char(*)[16])ctx->h);
		lea_memset(ctx->yn, 0, 16);
	}

	lea_encrypt_1block(ctx->ek0, ctx->ctr, &ctx->key);

	ctr128_inc_aligned(ctx->ctr);
}

void MAKE_FUNC(gcm_set_aad)(LEA_GCM_CTX *ctx, const unsigned char *aad, int aad_len)
{
	if (!ctx){
		return;
	}
	if (aad_len <= 0){
		return;
	}
	if (!aad){
		return;
	}
	ctx->aad_len = aad_len;

	gcm_ghash(ctx->tbl, aad, aad_len, (const unsigned char (*)[16])ctx->h);
}

void MAKE_FUNC(gcm_enc)(LEA_GCM_CTX *ctx, unsigned char *ct, const unsigned char *pt, int pt_len)
{
	int remain, i;
	
	if (!ctx || !ct || !pt){
		return;
	}
	if (pt_len < 0){
		return;
	}

	ctx->is_encrypt = 1;
	ctx->ct_len += pt_len;

	if (!pt_len)
		return;

	if(ctx->yn_used)
	{
		remain = 16 - ctx->yn_used;

		if(remain > pt_len)
			remain = pt_len;

		for(i = 0; i < remain; i++)
			ctx->yn[ctx->yn_used + i] ^= pt[i];

		lea_memcpy(ct, ctx->yn + ctx->yn_used, remain);

		pt_len -= remain;
		pt += remain;
		ct += remain;
		ctx->yn_used = (ctx->yn_used + remain) & 0xf;

		if(!ctx->yn_used)
			gcm_ghash(ctx->tbl, ctx->yn, 16, (const unsigned char (*)[16])ctx->h);

		if(!pt_len)
			return;
	}

	i = pt_len & 0xfffffff0;

	MAKE_FUNC(ctr_enc)(ct, pt, i, ctx->ctr, &ctx->key);

	if(i)
		gcm_ghash(ctx->tbl, ct, i, (const unsigned char (*)[16])ctx->h);

	pt_len &= 0xf;

	if(!pt_len)
		return;

	pt += i;
	ct += i;

	lea_encrypt_1block(ctx->yn, ctx->ctr, &ctx->key);
	ctr128_inc_aligned(ctx->ctr);
	ctx->yn_used = pt_len;

	for(pt_len--; pt_len >= 0; pt_len--)
		ct[pt_len] = ctx->yn[pt_len] = ctx->yn[pt_len] ^ pt[pt_len];
}

void MAKE_FUNC(gcm_dec)(LEA_GCM_CTX *ctx, unsigned char *pt, const unsigned char *ct, int ct_len)
{
	int remain, i;	

	if (!ctx || !pt || !ct){
		return;
	}
	if (ct_len < 0){
		return;
	}

	ctx->is_encrypt = 0;
	ctx->ct_len += ct_len;

	if (!ct_len)
		return;

	if(ctx->yn_used)
	{
		remain = 16 - ctx->yn_used;

		if(remain > ct_len)
			remain = ct_len;

		for(i = 0; i < remain; i++)
			pt[i] = ctx->yn[ctx->yn_used + i] ^ ct[i];

		lea_memcpy(ctx->yn + ctx->yn_used, ct, remain);

		ct_len -= remain;
		pt += remain;
		ct += remain;
		ctx->yn_used = (ctx->yn_used + remain) & 0xf;

		if(!ctx->yn_used)
			gcm_ghash(ctx->tbl, ctx->yn, 16, (const unsigned char (*)[16])ctx->h);

		if(!ct_len)
			return;
	}

	i = ct_len & 0xfffffff0;

	MAKE_FUNC(ctr_dec)(pt, ct, i, ctx->ctr, &ctx->key);

	if(i)
		gcm_ghash(ctx->tbl, ct, i, (const unsigned char (*)[16])ctx->h);

	ct_len &= 0xf;

	if(!ct_len)
		return;

	pt += i;
	ct += i;

	lea_encrypt_1block(ctx->yn, ctx->ctr, &ctx->key);
	ctr128_inc_aligned(ctx->ctr);
	ctx->yn_used = ct_len;

	for (ct_len--; ct_len >= 0; ct_len--){
		pt[ct_len] = ctx->yn[ct_len] ^ ct[ct_len];
		ctx->yn[ct_len] = ct[ct_len];
	}
}


int MAKE_FUNC(gcm_final)(LEA_GCM_CTX *ctx, unsigned char *tag, int tag_len)
{
	unsigned char tmp[16];

	if (!ctx || !tag){
		memset(ctx, 0, sizeof(LEA_GCM_CTX));
		return -1;
	}
	if (tag_len < 4){
		memset(ctx, 0, sizeof(LEA_GCM_CTX));
		return -1;
	}

	if(ctx->yn_used)
		gcm_ghash(ctx->tbl, ctx->yn, ctx->yn_used, (const unsigned char (*)[16])ctx->h);

	lea_memset(tmp, 0, 16);

	ctx->aad_len <<= 3;
	ctx->ct_len <<= 3;

	tmp[4] = ctx->aad_len >> 24;
	tmp[5] = ctx->aad_len >> 16;
	tmp[6] = ctx->aad_len >> 8;
	tmp[7] = ctx->aad_len;

	tmp[12] = ctx->ct_len >> 24;
	tmp[13] = ctx->ct_len >> 16;
	tmp[14] = ctx->ct_len >> 8;
	tmp[15] = ctx->ct_len;

	gcm_ghash(ctx->tbl, tmp, 16, (const unsigned char (*)[16])ctx->h);

	XOR8x16(ctx->tbl, ctx->tbl, ctx->ek0);

	if(ctx->is_encrypt)
		lea_memcpy(tag, ctx->tbl, tag_len);
	else
	{
		for(tag_len--; tag_len >= 0; tag_len--)
		{
			if (ctx->tbl[tag_len] != tag[tag_len]){
				lea_memset(ctx->ctr, 0, 16);
				lea_memset(ctx->ek0, 0, 16);
				lea_memset(ctx->tbl, 0, 16);
				lea_memset(ctx->yn, 0, 16);
				ctx->yn_used = 0;			
				
				return -1;

			}
				
		}
	}
	ctx->ct_len = 0;

	lea_memset(ctx->ctr, 0, 16);
	lea_memset(ctx->ek0, 0, 16);
	lea_memset(ctx->tbl, 0, 16);
	lea_memset(ctx->yn, 0, 16);
	ctx->yn_used = 0;

	return 0;
}

