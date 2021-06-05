static void ctr64_inc(unsigned char *counter) {
	unsigned int n=8;
	unsigned char c;

	counter += 8;
	do {
		--n;
		c = counter[n];
		++c;
		counter[n] = c;
		if (c) return;
	} while (n);
}	

int MAKE_FUNC(ccm_enc)(unsigned char *ct, unsigned char *T, const unsigned char *pt, unsigned int pt_len, unsigned int Tlen,
						 const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key)
{
	unsigned char ctr[16] = {0, }, tag[16] = {0, }, S0[16];
	unsigned int i, j, h;

	if (!T || !key){
		return 1;
	}
	
	if (pt_len > 0 && (!pt || !ct)){
		return 1;
	}

	if (Nlen > 0 && !N){
		return 1;
	}

	if (Nlen < 7 || Nlen > 13){
		return 1;
	}

	if (Alen > 0 && !A){
		return 1;
	}

	if (Tlen < 4 || Tlen > 16 || Tlen % 2 != 0){
		return 1;
	}

	/* Formatting of B0 */
	tag[0] = ((Alen != 0) ? 0x40 : 0) | (((Tlen - 2) >> 1) << 3) | (14 - Nlen);
	
	for(i = 0; i < Nlen; i++)
		tag[i + 1] = N[i];	/* Set N */
		
	tag[14] = pt_len >> 8, tag[15] = pt_len;
	if(Nlen < 13)
		tag[13] = pt_len >> 16;
	if(Nlen < 12)
		tag[12] = pt_len >> 24;

	lea_encrypt_1block(tag, tag, key);

	/* Formatting of the Associated Data */
	if(Alen < 0xff00)
		tag[0] ^= (Alen >> 8), tag[1] ^= Alen, i = 2;
	else
		tag[0] ^= 0xff, tag[1] ^= 0xfe, tag[2] ^= (Alen >> 24), tag[3] ^= (Alen >> 16), tag[4] ^= (Alen >> 8), tag[5] ^= Alen, i = 6;

	for(j = 0; j < Alen; i = 0)
	{
		for(; (i < 16) && (j < Alen); i++, j++)
			tag[i] ^= A[j];

		lea_encrypt_1block(tag, tag, key);
	}

	/* Calculate Yr */
	for(i = 0; i < pt_len;)
	{
		j = ((pt_len - i) > 0x10) ? 0x10 : (pt_len - i);

		for(h = 0; h < j; h++, i++)
			tag[h] ^= pt[i];
		lea_encrypt_1block(tag, tag, key);
	}
	
	/* Formatting of the counter blocks */
	ctr[0] = 14 - Nlen;

	for(i = 0; i < Nlen; i++)
		ctr[i + 1] = N[i];

	/* Calculate S0 */
	lea_encrypt_1block(S0, ctr, key);
	ctr64_inc(ctr);

	/* Fast encryption using SIMD */
	MAKE_FUNC(ctr_enc)(ct, pt, pt_len, ctr, key);

	for(i = 0; i < Tlen; i++)
		T[i] = tag[i] ^ S0[i];

	return 0;
}

int MAKE_FUNC(ccm_dec)(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *T, unsigned int Tlen,
						 const unsigned char *N, unsigned int Nlen, const unsigned char *A, unsigned int Alen, const LEA_KEY *key)
{
	unsigned char ctr[16] = {0, }, tag[16] = {0, }, S0[16];
	unsigned int i, j, h;

	if (!T || !key){
		return 1;
	}

	if (ct_len > 0 && (!pt || !ct)){
		return 1;
	}

	if (Nlen > 0 && !N){
		return 1;
	}

	if (Nlen < 7 || Nlen > 13){
		return 1;
	}

	if (Alen > 0 && !A){
		return 1;
	}

	if (Tlen < 4 || Tlen > 16 || Tlen % 2 != 0){
		return 1;
	}

	/* Formatting of B0 */
	tag[0] = ((Alen != 0) ? 0x40 : 0) | (((Tlen - 2) >> 1) << 3) | (14 - Nlen);
	
	for(i = 0; i < Nlen; i++)
		tag[i + 1] = N[i];	/* Set N */
	
	tag[14] = ct_len >> 8, tag[15] = ct_len;
	if(Nlen < 13)
		tag[13] = ct_len >> 16;
	if(Nlen < 12)
		tag[12] = ct_len >> 24;
	
	lea_encrypt_1block(tag, tag, key);

	/* Formatting of the Associated Data */
	if(Alen < 0xff00)
		tag[0] ^= (Alen >> 8), tag[1] ^= Alen, i = 2;
	else
		tag[0] ^= 0xff, tag[1] ^= 0xfe, tag[2] ^= (Alen >> 24), tag[3] ^= (Alen >> 16), tag[4] ^= (Alen >> 8), tag[5] ^= Alen, i = 6;

	for(j = 0; j < Alen; i = 0)
	{
		for(; (i < 16) && (j < Alen); i++, j++)
			tag[i] ^= A[j];

		lea_encrypt_1block(tag, tag, key);
	}

	/* Formatting of the counter blocks */
	ctr[0] = 14 - Nlen;

	for(i = 0; i < Nlen; i++)
		ctr[i + 1] = N[i];

	/* Calculate S0 */
	lea_encrypt_1block(S0, ctr, key);
	ctr64_inc(ctr);

	/* Fast encryption using SIMD */
	MAKE_FUNC(ctr_enc)(pt, ct, ct_len, ctr, key);

	/* Calculate Yr */
	for(i = 0; i < ct_len;)
	{
		j = ((ct_len - i) > 0x10) ? 0x10 : (ct_len - i);

		for(h = 0; h < j; h++, i++)
			tag[h] ^= pt[i];
		lea_encrypt_1block(tag, tag, key);
	}

	for(i = 0; i < Tlen; i++)
		tag[i] = tag[i] ^ S0[i];

	for(i = 0; i < Tlen; i++)
	{
		if(T[i] != tag[i])
			return -1;
	}

	return 0;
}
