#include "lea.h"
#include "lea_locl.h"



static void lea_make_cmac_subkey(unsigned char *new_key, const unsigned char *old_key, unsigned int key_len)
{
	unsigned int i;

	for (i = 0; i < key_len; i++)
	{
		new_key[i] = old_key[i] << 1;
		if (i < key_len - 1 && old_key[i + 1] & 0x80)
			new_key[i] |= 1;
	}
	
	if (old_key[0] & 0x80)
		new_key[key_len - 1] ^= (key_len == 16 ? 0x87 : 0x1b);
}


void MAKE_FUNC(cmac_init)(LEA_CMAC_CTX *ctx, const unsigned char *mk, int mk_len)
{
	unsigned char zero[16];

	if (!ctx || !mk){
		return;
	}

	lea_set_key(&ctx->key, mk, mk_len);

	lea_memset(zero, 0, 16);
	lea_encrypt_1block(zero, zero, &ctx->key);
	lea_make_cmac_subkey(ctx->k1, zero, 16);
	lea_make_cmac_subkey(ctx->k2, ctx->k1, 16);

	lea_memset(ctx->tbl, 0, 16);
	lea_memset(ctx->last_block, 0, 16);

	ctx->nlast_block = 0;
}

void MAKE_FUNC(cmac_update)(LEA_CMAC_CTX *ctx, const unsigned char *data, int data_len)
{
	int empty;
	if (!ctx || !data){
		return;
	}
	if(data_len < 0)
		return;

	if(ctx->nlast_block)
	{
		empty = 16 - ctx->nlast_block;

		if(empty > data_len)
			empty = data_len;

		lea_memcpy(ctx->last_block + ctx->nlast_block, data, empty);

		data_len -= empty;
		data += empty;
		ctx->nlast_block += empty;

		if(!data_len)
			return;

		XOR8x16(ctx->tbl, ctx->tbl, ctx->last_block);
		lea_encrypt_1block(ctx->tbl, ctx->tbl, &ctx->key);
	}

	for(; data_len > 16; data_len -= 0x10, data += 0x10)
	{
		XOR8x16(ctx->tbl, ctx->tbl, data);

		lea_encrypt_1block(ctx->tbl, ctx->tbl, &ctx->key);
	}

	lea_memcpy(ctx->last_block, data, data_len);

	ctx->nlast_block = data_len;
}

void MAKE_FUNC(cmac_final)(LEA_CMAC_CTX *ctx, unsigned char *cmac, int cmac_len)
{
	if (!ctx || !cmac){
		return;
	}

	if (cmac_len <= 0){
		return;
	}

	if(ctx->nlast_block != 16)
	{
		ctx->last_block[ctx->nlast_block] = 0x80;
		lea_memset(ctx->last_block + ctx->nlast_block + 1, 0, 15 - ctx->nlast_block);

		for(ctx->nlast_block; ctx->nlast_block >= 0; ctx->nlast_block--)
			ctx->tbl[ctx->nlast_block] ^= ctx->last_block[ctx->nlast_block];

		XOR8x16(ctx->tbl, ctx->tbl, ctx->k2);
	}
	else
	{
		XOR8x16(ctx->tbl, ctx->tbl, ctx->last_block);
		XOR8x16(ctx->tbl, ctx->tbl, ctx->k1);
	}

	lea_encrypt_1block(ctx->tbl, ctx->tbl, &ctx->key);

	for(cmac_len--; cmac_len >= 0; cmac_len--)
		cmac[cmac_len] = ctx->tbl[cmac_len];

	ctx->nlast_block = 0;
	lea_memset(ctx->last_block, 0, 16);
	lea_memset(ctx->tbl, 0, 16);
}
