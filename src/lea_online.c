#include "lea.h"
#include "lea_locl.h"

/* encType info
 * ALG(1), MODE(2-3), SUB(4), INFO(5), ENC(6)
 * Info -> len16 : 1, no_iv : 2, keep_last_block : 4 
 */

int lea_online_init(LEA_ONLINE_CTX *ctx, unsigned int encType, const unsigned char * iv, const unsigned char *mk, int mk_len){
	LEA_KEY key;
	if (mk == NULL || ctx == NULL){
		return -1;
	}
	if (mk_len != 16 && mk_len != 24 && mk_len != 32){
		return -2;
	}

	// check no_iv
	if (!(encType & 0x20) && iv == NULL){
		return -1;
	}

	lea_set_key(&key, mk, mk_len);
	return lea_online_init_ex(ctx, encType, iv, &key);
}

int lea_online_init_ex(LEA_ONLINE_CTX *ctx, unsigned int encType, const unsigned char * iv, const LEA_KEY* key){
	if (key == NULL || ctx == NULL){
		return -1;
	}

	// check no_iv
	if (!(encType & 0x20) && iv == NULL){
		return -1;
	}

	if (encType == LEA_ECB_NOPAD_ENC || encType == LEA_ECB_NOPAD_DEC
		|| encType == LEA_ECB_PKCS5PAD_ENC || encType == LEA_ECB_PKCS5PAD_DEC
		|| encType == LEA_CBC_NOPAD_ENC || encType == LEA_CBC_NOPAD_DEC
		|| encType == LEA_CBC_PKCS5PAD_ENC || encType == LEA_CBC_PKCS5PAD_DEC
		|| encType == LEA_CTR_ENC || encType == LEA_CTR_DEC
		|| encType == LEA_OFB_ENC || encType == LEA_OFB_DEC
		|| encType == LEA_CFB128_ENC || encType == LEA_CFB128_DEC)
	{

		lea_memset(ctx, 0, sizeof(LEA_ONLINE_CTX));
		lea_memcpy(&ctx->key, key, sizeof(LEA_KEY));
		ctx->encType = encType;

		if (iv != NULL){
			lea_memcpy(ctx->iv, iv, 16);
		}
	}
	else{
		return -3;
	}

	return 0;
}

int lea_online_update(LEA_ONLINE_CTX *ctx, unsigned char * out, unsigned char *in, int in_len){
	int more;
	int out_len = 0;
	int block_len;
	unsigned char * iv;
	LEA_KEY* key;
	if (in_len == 0){
		return 0;
	}
	if (in_len < 0){
		return -2;
	}
	if (out == NULL || in == NULL || ctx == NULL){
		return -1;
	}
	if (ctx->encType == 0){
		return -3;
	}
	if (ctx->nlast_block < 0 || ctx->nlast_block > 16){
		return -3;
	}
	

	iv = ctx->iv;
	key = &ctx->key;

	if (in_len + ctx->nlast_block <= 16){
		lea_memcpy(ctx->last_block + ctx->nlast_block, in, in_len);
		ctx->nlast_block += in_len;

		if (ctx->encType & 0x40 || ctx->nlast_block != 16){
			return 0;
		}

		in += in_len;
		in_len = 0;
	}

	//assert(ctx->nlast_block + in_len >= 16);

	if (ctx->nlast_block > 0 && ctx->nlast_block != 16){
		more = 16 - ctx->nlast_block;
		lea_memcpy(ctx->last_block + ctx->nlast_block, in, more);
		in += more;
		in_len -= more;
		ctx->nlast_block = 16;
	}
	
	//assert(ctx->nlast_block == 16 || ctx->nlast_block == 0);

	block_len = (in_len >> 4) << 4;
	//check keep_last
	if (ctx->encType & 0x40 && in_len == block_len){
		block_len -= 16;
	}

	switch (ctx->encType){
	case LEA_ECB_NOPAD_ENC:
	case LEA_ECB_PKCS5PAD_ENC:
		if (ctx->nlast_block){
			lea_ecb_enc(out, ctx->last_block, 16, key);
			out_len += 16;
			out += 16;
		}
		if (block_len){
			lea_ecb_enc(out, in, block_len, key);
		}
		break;
	case LEA_ECB_NOPAD_DEC:
	case LEA_ECB_PKCS5PAD_DEC:
		if (ctx->nlast_block){
			lea_ecb_dec(out, ctx->last_block, 16, key);
			out_len += 16;
			out += 16;
		}
		if (block_len){
			lea_ecb_dec(out, in, block_len, key);
		}
		break;
	case LEA_CBC_NOPAD_ENC:
	case LEA_CBC_PKCS5PAD_ENC:
		if (ctx->nlast_block){
			lea_cbc_enc(out, ctx->last_block, 16, iv, key);
			iv = out;
			out_len += 16;
			out += 16;
		}
		if (block_len){
			lea_cbc_enc(out, in, block_len, iv, key);
			iv = out + block_len - 16;
		}
		break;
	case LEA_CBC_NOPAD_DEC:
	case LEA_CBC_PKCS5PAD_DEC:
		if (ctx->nlast_block){
			lea_cbc_dec(out, ctx->last_block, 16, iv, key);
			lea_memcpy(iv, ctx->last_block, 16);
			out_len += 16;
			out += 16;
		}
		if (block_len){
			lea_cbc_dec(out, in, block_len, iv, key);
			iv = in + block_len - 16;
		}
		break;
	case LEA_CTR_ENC:
	case LEA_CTR_DEC:
		if (ctx->nlast_block){
			lea_ctr_enc(out, ctx->last_block, 16, iv, key);
			out_len += 16;
			out += 16;
		}
		if (block_len){
			lea_ctr_enc(out, in, block_len, iv, key);
		}
		break;

	case LEA_OFB_ENC:
	case LEA_OFB_DEC:
		if (ctx->nlast_block){
			lea_ofb_enc(out, ctx->last_block, 16, iv, key);
			out_len += 16;
			out += 16;
		}
		if (block_len){
			lea_ofb_enc(out, in, block_len, iv, key);
		}
		break;

	case LEA_CFB128_ENC:
		if (ctx->nlast_block){
			lea_cfb128_enc(out, ctx->last_block, 16, iv, key);
			iv = out;
			out_len += 16;
			out += 16;
		}
		if (block_len){
			lea_cfb128_enc(out, in, block_len, iv, key);
			iv = out + block_len - 16;
		}
		break;
	case LEA_CFB128_DEC:
		if (ctx->nlast_block){
			lea_cfb128_dec(out, ctx->last_block, 16, iv, key);
			lea_memcpy(iv, ctx->last_block, 16);
			out_len += 16;
			out += 16;
		}
		if (block_len){
			lea_cfb128_dec(out, in, block_len, iv, key);
			iv = in + block_len - 16;
		}
		break;
	default:
		return -3;
	}

	if (ctx->nlast_block){
		lea_memset(ctx->last_block, 0, 16);
		ctx->nlast_block = 0;
	}

	if (iv != ctx->iv){
		lea_memcpy(ctx->iv, iv, 16);
	}

	out_len += block_len;
	out += block_len;
	in += block_len;
	in_len -= block_len;

	//assert(in_len <= 16);

	if (in_len){
		lea_memcpy(ctx->last_block, in, in_len);
		ctx->nlast_block = in_len;
	}

	return out_len;
}

int lea_online_final(LEA_ONLINE_CTX *ctx, unsigned char * out){
	int out_len = 0;
	int i, more;
	LEA_KEY *key;
	unsigned char *iv;

	if (ctx == NULL || out == NULL){
		return -1;
	}
	if (ctx->encType == 0){
		return -3;
	}
	if (ctx->nlast_block < 0 || ctx->nlast_block > 16){
		return -3;
	}

	// Check len16 FLAG
	if (ctx->encType & 0x10 && ctx->nlast_block % 16 > 0){
		return -2;
	}

	// Check keep_last Flag
	if (ctx->encType & 0x40 && ctx->nlast_block != 16){
		return -2;
	}
	if (!(ctx->encType & 0x40) && ctx->nlast_block == 16){
		return -3;
	}

	// Fill padding
	if (ctx->encType == LEA_ECB_PKCS5PAD_ENC || ctx->encType == LEA_CBC_PKCS5PAD_ENC){
		more = 16 - ctx->nlast_block;
		while (ctx->nlast_block < 16){
			ctx->last_block[ctx->nlast_block] = more;
			ctx->nlast_block++;
		}
	}

	if (ctx->nlast_block == 0){
		lea_memset(ctx, 0, sizeof(LEA_ONLINE_CTX));
		return 0;
	}

	key = &ctx->key;
	iv = ctx->iv;
	switch (ctx->encType){
	case LEA_ECB_NOPAD_ENC:
	case LEA_ECB_PKCS5PAD_ENC:
		lea_ecb_enc(out, ctx->last_block, 16, key);
		out_len = 16;
		break;
	case LEA_ECB_NOPAD_DEC:
		lea_ecb_dec(out, ctx->last_block, 16, key);
		out_len = 16;
		break;
	case LEA_ECB_PKCS5PAD_DEC:
		lea_ecb_dec(ctx->last_block, ctx->last_block, 16, key);
		i = ctx->last_block[15];

		if (i > 0x16){
			return -4;
		}

		while (i > 0 && ctx->nlast_block > 0){
			ctx->nlast_block--;
			if (ctx->last_block[ctx->nlast_block] != ctx->last_block[15]){
				return -4;
			}
			i--;
		}

		lea_memcpy(out, ctx->last_block, ctx->nlast_block);
		out_len = ctx->nlast_block;
		break;
	case LEA_CBC_NOPAD_ENC:
	case LEA_CBC_PKCS5PAD_ENC:
		lea_cbc_enc(out, ctx->last_block, 16, iv, key);
		out_len = 16;
		break;
	case LEA_CBC_NOPAD_DEC:
		lea_cbc_dec(out, ctx->last_block, 16, iv, key);
		out_len = 16;
		break;
	case LEA_CBC_PKCS5PAD_DEC:
		lea_cbc_dec(ctx->last_block, ctx->last_block, 16, iv, key);
		i = ctx->last_block[15];

		if (i > 0x16){
			return -4;
		}

		while (i > 0 && ctx->nlast_block > 0){
			ctx->nlast_block--;
			if (ctx->last_block[ctx->nlast_block] != ctx->last_block[15]){
				return -4;
			}
			i--;
		}

		lea_memcpy(out, ctx->last_block, ctx->nlast_block);
		out_len = ctx->nlast_block;
		break;
	case LEA_CTR_ENC:
	case LEA_CTR_DEC:
		lea_ctr_enc(ctx->last_block, ctx->last_block, 16, iv, key);
		lea_memcpy(out, ctx->last_block, ctx->nlast_block);
		out_len = ctx->nlast_block;
		break;
	case LEA_OFB_ENC:
	case LEA_OFB_DEC:
		lea_ofb_enc(ctx->last_block, ctx->last_block, 16, iv, key);
		lea_memcpy(out, ctx->last_block, ctx->nlast_block);
		out_len = ctx->nlast_block;
		break;

	case LEA_CFB128_ENC:
		lea_cfb128_enc(ctx->last_block, ctx->last_block, 16, iv, key);
		lea_memcpy(out, ctx->last_block, ctx->nlast_block);
		out_len = ctx->nlast_block;
		break;
	case LEA_CFB128_DEC:
		lea_cfb128_dec(ctx->last_block, ctx->last_block, 16, iv, key);
		lea_memcpy(out, ctx->last_block, ctx->nlast_block);
		out_len = ctx->nlast_block;
		break;
	default:
		return -3;
	}


	// Check padding
	if (ctx->encType == LEA_ECB_PKCS5PAD_DEC || ctx->encType == LEA_CBC_PKCS5PAD_DEC){

	}
	

	//finish
	lea_memset(ctx, 0, sizeof(LEA_ONLINE_CTX));

	return out_len;
}
