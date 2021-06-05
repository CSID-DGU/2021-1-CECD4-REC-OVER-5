#include "lea.h"
#include "lea_locl.h"

#ifdef COMPILE_PCLMUL
#include <smmintrin.h>
#include <wmmintrin.h>


const unsigned int gfmul_table[64] = {
	0x00000000, 0x00000087, 0x0000010E, 0x00000189, 0x0000021C, 0x0000029B, 0x00000312, 0x00000395,
	0x00000438, 0x000004BF, 0x00000536, 0x000005B1, 0x00000624, 0x000006A3, 0x0000072A, 0x000007AD,
	0x00000870, 0x000008F7, 0x0000097E, 0x000009F9, 0x00000A6C, 0x00000AEB, 0x00000B62, 0x00000BE5,
	0x00000C48, 0x00000CCF, 0x00000D46, 0x00000DC1, 0x00000E54, 0x00000ED3, 0x00000F5A, 0x00000FDD,
	0x00001067, 0x000010E0, 0x00001169, 0x000011EE, 0x0000127B, 0x000012FC, 0x00001375, 0x000013F2,
	0x0000145F, 0x000014D8, 0x00001551, 0x000015D6, 0x00001643, 0x000016C4, 0x0000174D, 0x000017CA,
	0x00001817, 0x00001890, 0x00001919, 0x0000199E, 0x00001A0B, 0x00001A8C, 0x00001B05, 0x00001B82,
	0x00001C2F, 0x00001CA8, 0x00001D21, 0x00001DA6, 0x00001E33, 0x00001EB4, 0x00001F3D, 0x00001FBA
};



static __m128i reflect_xmm(__m128i X)
{
	__m128i tmp1,tmp2;
	__m128i AND_MASK = _mm_set_epi32(0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f);
	__m128i LOWER_MASK = _mm_set_epi32(0x0f070b03, 0x0d050901, 0x0e060a02, 0x0c040800);
	__m128i HIGHER_MASK = _mm_set_epi32(0xf070b030, 0xd0509010, 0xe060a020, 0xc0408000);
	__m128i BSWAP_MASK = _mm_set_epi8(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);

	tmp2 = _mm_srli_epi16(X, 4);
	tmp1 = _mm_and_si128(X, AND_MASK);
	tmp2 = _mm_and_si128(tmp2, AND_MASK);
	tmp1 = _mm_shuffle_epi8(HIGHER_MASK ,tmp1);
	tmp2 = _mm_shuffle_epi8(LOWER_MASK ,tmp2);
	tmp1 = _mm_xor_si128(tmp1, tmp2);

	return _mm_shuffle_epi8(tmp1, BSWAP_MASK);
}

static __m128i gcm_gfmul(__m128i a, __m128i b)
{
	__m128i high, low, tmp0, tmp1, ret;
	__m128i shuffle = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
	int lookup_val;

	a = _mm_shuffle_epi8(a, shuffle);
	b = _mm_shuffle_epi8(b, shuffle);

	//	multiplication
	low = _mm_clmulepi64_si128(a, b, 0x00);
	high = _mm_clmulepi64_si128(a, b, 0x11);
	tmp0 = _mm_clmulepi64_si128(a, b, 0x01);
	tmp1 = _mm_clmulepi64_si128(a, b, 0x10);

	tmp0 = _mm_xor_si128(tmp0, tmp1);

	tmp1 = _mm_srli_si128(tmp0, 8);
	high = _mm_xor_si128(high, tmp1);
	tmp1 = _mm_slli_si128(tmp0, 8);
	low = _mm_xor_si128(low, tmp1);

	//	reduction
	ret = _mm_xor_si128(high, low);

	tmp0 = _mm_srli_epi64(high, 1);
	lookup_val = _mm_extract_epi8(tmp0, 15);
	tmp0 = _mm_setr_epi32(gfmul_table[lookup_val], 0, 0, 0);
	ret = _mm_xor_si128(ret, tmp0);

	tmp0 = _mm_slli_epi64(high, 1);
	tmp1 = _mm_srli_epi64(high, 63);
	tmp1 = _mm_slli_si128(tmp1, 8);
	tmp1 = _mm_xor_si128(tmp0, tmp1);
	ret = _mm_xor_si128(ret, tmp1);

	tmp0 = _mm_slli_epi64(high, 2);
	tmp1 = _mm_srli_epi64(high, 62);
	tmp1 = _mm_slli_si128(tmp1, 8);
	tmp1 = _mm_xor_si128(tmp0, tmp1);
	ret = _mm_xor_si128(ret, tmp1);

	tmp0 = _mm_slli_epi64(high, 7);
	tmp1 = _mm_srli_epi64(high, 57);
	tmp1 = _mm_slli_si128(tmp1, 8);
	tmp1 = _mm_xor_si128(tmp0, tmp1);
	ret = _mm_xor_si128(ret, tmp1);

	ret = _mm_shuffle_epi8(ret, shuffle);

	return ret;
}


void gcm_ghash_pclmul(unsigned char *r, const unsigned char *x, unsigned int x_len, const unsigned char h[][16])
{
	__m128i tmp_simd, x_simd, h_simd;
	unsigned char tmp[16] = {0, };
	int i;

	tmp_simd = _mm_loadu_si128((__m128i *)r);
	tmp_simd = reflect_xmm(tmp_simd);
	h_simd = _mm_loadu_si128((__m128i *)h);
	h_simd = reflect_xmm(h_simd);

	for(; x_len >= 16; x_len -= 16, x += 16)
	{
		x_simd = _mm_loadu_si128((__m128i *)x);
		x_simd = reflect_xmm(x_simd);

		tmp_simd = _mm_xor_si128(tmp_simd, x_simd);

		tmp_simd = gcm_gfmul(tmp_simd, h_simd);
	}

	if(x_len)
	{
		lea_memcpy(tmp, x, x_len);
		x_simd = _mm_loadu_si128((__m128i *)tmp);
		x_simd = reflect_xmm(x_simd);

		tmp_simd = _mm_xor_si128(tmp_simd, x_simd);

		tmp_simd = gcm_gfmul(tmp_simd, h_simd);
	}

	tmp_simd = reflect_xmm(tmp_simd);

	_mm_storeu_si128((__m128i *)r, tmp_simd);
}

void lea_gcm_init_pclmul(LEA_GCM_CTX *ctx, const unsigned char *mk, int mk_len)
{
	unsigned char zero[16] = {0, };

	lea_memset(ctx, 0, sizeof(LEA_GCM_CTX));

	lea_set_key(&ctx->key, mk, mk_len);
	lea_encrypt(ctx->h[0], zero, &ctx->key);
}

#endif
