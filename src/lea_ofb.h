void MAKE_FUNC(ofb_enc)(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *iv, const LEA_KEY *key)
{

	//unsigned char iv_tmp[16];
	unsigned int numBlock1 = pt_len >> 4, i;

	if (!iv || !key){
		return;
	}

	if (pt_len > 0 && (!ct || !pt)){
		return;
	}

#if defined(USE_OWN_FUNC)
	_lea_ofb_enc(ct,pt,pt_len,iv,key);
	if(numBlock1 > 0 && pt_len & 0xf){
	}
#else
	//CPY8x16(iv_tmp, iv);

	for(i = 0; i < numBlock1; i++, pt += 0x10, ct += 0x10)
	{
		lea_encrypt_1block(iv, iv, key);

		XOR8x16(ct, pt, iv);
	}
#endif
	if((numBlock1 << 4) != pt_len)
	{
		lea_encrypt_1block(iv, iv, key);

		for(i = 0; i < pt_len - (numBlock1 << 4); i++)
			ct[i] = iv[i] ^ pt[i];
	}

}

void MAKE_FUNC(ofb_dec)(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, unsigned char *iv, const LEA_KEY *key)
{
	MAKE_FUNC(ofb_enc)(pt, ct, ct_len, iv, key);
}
