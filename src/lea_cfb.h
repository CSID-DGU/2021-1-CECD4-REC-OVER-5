void MAKE_FUNC(cfb128_enc)(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key)
{


	const unsigned char *pIv = iv;
	unsigned char block[16];
	unsigned int nBlock1 = pt_len >> 4, i;

	if (!iv || !key){
		return;
	}
	if (pt_len > 0 && (!pt||!ct)){
		return;
	}
	
#if defined(USE_OWN_FUNC)
	
	_lea_cfb128_enc(ct, pt, pt_len, iv, key);
	if (nBlock1 > 0 && pt_len & 0xf){
		ct += nBlock1 * 16;
		pt += nBlock1 * 16;
		pIv = pt - 16;
	}
#else
	for(i = 0; i < nBlock1; i++, pt += 0x10, ct += 0x10)
	{
		lea_encrypt_1block(block, pIv, key);

		XOR8x16(ct, block, pt);

		pIv = ct;
	}
#endif
	if (pt_len & 0xf){
		lea_encrypt_1block(block, pIv, key);
		for (i = 0; i < (pt_len & 0xf); i++){
			ct[i] = block[i] ^ pt[i];
		}
	}


}

void MAKE_FUNC(cfb128_dec)(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key)
{

	const unsigned char *pIv = iv;
	unsigned char block[16];
	unsigned int nBlock1 = ct_len >> 4, i;
	
	if (!iv || !key){
		return;
	}
	if (ct_len > 0 && (!pt || !ct)){
		return;
	}

	if (ct_len & 0xf){
		i = nBlock1 * 16;

		if (nBlock1 > 0){
			pIv = ct + i - 16;
		}

		lea_encrypt_1block(block, pIv, key);
		for (; i < ct_len; i++){
			pt[i] = block[i & 0xf] ^ ct[i];
		}
	}
	if(ct_len < 0x10){
		return;
	}
#if defined(USE_OWN_FUNC)
	_lea_cfb128_dec(pt, ct, ct_len, iv, key);
#else
	for(i = 1, pt += ((nBlock1 - 1) * 16), ct += ((nBlock1 - 1) * 16); i < nBlock1; i++, pt -= 16, ct -= 16)
	{
		pIv = ct - 16;

		lea_encrypt_1block(block, pIv, key);

		XOR8x16(pt, block, ct);
	}

	lea_encrypt_1block(block, iv, key);

	XOR8x16(pt, block, ct);
#endif
}
