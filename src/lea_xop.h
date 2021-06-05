#include "lea.h"
#include "lea_locl.h"

#ifdef COMPILE_XOP

#if !defined(_MSC_VER) && !defined(__XOP__)
#error "turn on XOP flag for lea_xop.c"
#endif

#include <emmintrin.h>
#include <immintrin.h>
#if defined(_MSC_VER)
#include <ammintrin.h>
#else
#include <x86intrin.h>
#endif

static void lea_encrypt_1block_xop(__m128i *ct, const __m128i *pt, const LEA_KEY *key);
static void ctr128_inc_xop(__m128i *counter);

#define XAR3_XOP(cur, pre, rk1, rk2)																								\
	cur = _mm_roti_epi32(_mm_add_epi32(_mm_xor_si128(pre, _mm_set1_epi32(rk1)), _mm_xor_si128(cur, _mm_set1_epi32(rk2))), 29);
#define XAR5_XOP(cur, pre, rk1, rk2)																								\
	cur = _mm_roti_epi32(_mm_add_epi32(_mm_xor_si128(pre, _mm_set1_epi32(rk1)), _mm_xor_si128(cur, _mm_set1_epi32(rk2))), 27);
#define XAR9_XOP(cur, pre, rk1, rk2)																								\
	cur = _mm_roti_epi32(_mm_add_epi32(_mm_xor_si128(pre, _mm_set1_epi32(rk1)), _mm_xor_si128(cur, _mm_set1_epi32(rk2))), 9);

#define XSR9_XOP(cur, pre, rk1, rk2)																								\
	cur = _mm_xor_si128(_mm_sub_epi32(_mm_roti_epi32(cur, 23), _mm_xor_si128(pre, _mm_set1_epi32(rk1))), _mm_set1_epi32(rk2));
#define XSR5_XOP(cur, pre, rk1, rk2)																								\
	cur = _mm_xor_si128(_mm_sub_epi32(_mm_roti_epi32(cur, 5), _mm_xor_si128(pre, _mm_set1_epi32(rk1))), _mm_set1_epi32(rk2));
#define XSR3_XOP(cur, pre, rk1, rk2)																								\
	cur = _mm_xor_si128(_mm_sub_epi32(_mm_roti_epi32(cur, 3), _mm_xor_si128(pre, _mm_set1_epi32(rk1))), _mm_set1_epi32(rk2));

#define SET_128ROUND_KEY(rk1, rk2, t, delta, rot)											\
	(t) = _mm_rot_epi32(_mm_add_epi32((t), _mm_loadu_si128((__m128i *)(delta))), rot);		\
	_mm_storeu_si128((__m128i *)(rk1), (t));												\
	_mm_storeu_si128((__m128i *)(rk2), _mm_shuffle_epi32((t), _MM_SHUFFLE(1, 3, 1, 2)));

#define SET_192ROUND_KEY(rk1, rk2, t1, t2, delta1, delta2, rot1, rot2)							\
	(t1) = _mm_rot_epi32(_mm_add_epi32((t1), _mm_loadu_si128((__m128i *)(delta1))), rot1);		\
	(t2) = _mm_rot_epi32(_mm_add_epi32((t2), _mm_loadu_si128((__m128i *)(delta2))), rot2);		\
	_mm_storeu_si128((__m128i *)(rk1), (t1));													\
	_mm_storeu_si128((__m128i *)(rk2), (t2));

#define SET_256ROUND_KEY_TYPE1(rk, t, delta, rot)												\
	(t) = _mm_rot_epi32(_mm_add_epi32((t), _mm_loadu_si128((__m128i *)(delta))), rot);			\
	_mm_storeu_si128((__m128i *)(rk), (t));

#define SET_256ROUND_KEY_TYPE2(rk, t, delta0, delta1, delta2, delta3, rot)											\
	(t) = _mm_rot_epi32(_mm_add_epi32((t), _mm_setr_epi32((delta0), (delta1), (delta2), (delta3))), rot);			\
	_mm_storeu_si128((__m128i *)(rk), (t));

#define LEA_ENC_ROUND(x, tmp, rot, rk0, rk1, rk2, rk3, rk4, rk5, key1, key2)		\
	key1 = _mm_setr_epi32((rk0), (rk2), (rk4), 0);									\
	key2 = _mm_setr_epi32((rk1), (rk3), (rk5), 0);									\
	tmp = _mm_xor_si128((x), key1);													\
	tmp = _mm_insert_epi32(tmp, 0, 3);												\
	(x) = _mm_shuffle_epi32((x), _MM_SHUFFLE(0, 3, 2, 1));							\
	(x) = _mm_xor_si128((x), key2);													\
	(x) = _mm_add_epi32((x), tmp);													\
	(x) = _mm_rot_epi32((x), rot);


static const unsigned int delta[8][36] = {
	{0xc3efe9db, 0x87dfd3b7, 0x0fbfa76f, 0x1f7f4ede, 0x3efe9dbc, 0x7dfd3b78, 0xfbfa76f0, 0xf7f4ede1,
	0xefe9dbc3, 0xdfd3b787, 0xbfa76f0f, 0x7f4ede1f, 0xfe9dbc3e, 0xfd3b787d, 0xfa76f0fb, 0xf4ede1f7,
	0xe9dbc3ef, 0xd3b787df, 0xa76f0fbf, 0x4ede1f7f, 0x9dbc3efe, 0x3b787dfd, 0x76f0fbfa, 0xede1f7f4,
	0xdbc3efe9, 0xb787dfd3, 0x6f0fbfa7, 0xde1f7f4e, 0xbc3efe9d, 0x787dfd3b, 0xf0fbfa76, 0xe1f7f4eD,
	0xc3efe9db,	0x87dfd3b7, 0x0fbfa76f, 0x1f7f4ede},
	{0x44626b02, 0x88c4d604, 0x1189ac09, 0x23135812, 0x4626b024, 0x8c4d6048, 0x189ac091, 0x31358122,
	0x626b0244, 0xc4d60488, 0x89ac0911, 0x13581223, 0x26b02446, 0x4d60488c, 0x9ac09118, 0x35812231,
	0x6b024462, 0xd60488c4, 0xac091189, 0x58122313, 0xb0244626, 0x60488c4d, 0xc091189a, 0x81223135,
	0x0244626b, 0x0488c4d6, 0x091189ac, 0x12231358, 0x244626b0, 0x488c4d60, 0x91189ac0, 0x22313581,
	0x44626b02, 0x88c4d604, 0x1189ac09, 0x23135812},
	{0x79e27c8a, 0xf3c4f914, 0xe789f229, 0xcf13e453, 0x9e27c8a7, 0x3c4f914f, 0x789f229e, 0xf13e453c,
	0xe27c8a79, 0xc4f914f3, 0x89f229e7, 0x13e453cf, 0x27c8a79e, 0x4f914f3c, 0x9f229e78, 0x3e453cf1,
	0x7c8a79e2, 0xf914f3c4, 0xf229e789, 0xe453cf13, 0xc8a79e27, 0x914f3c4f, 0x229e789f, 0x453cf13e,
	0x8a79e27c, 0x14f3c4f9, 0x29e789f2, 0x53cf13e4, 0xa79e27c8, 0x4f3c4f91, 0x9e789f22, 0x3cf13e45,
	0x79e27c8a, 0xf3c4f914, 0xe789f229, 0xcf13e453},
	{0x78df30ec, 0xf1be61d8, 0xe37cc3b1, 0xc6f98763, 0x8df30ec7, 0x1be61d8f, 0x37cc3b1e, 0x6f98763c,
	0xdf30ec78, 0xbe61d8f1, 0x7cc3b1e3, 0xf98763c6, 0xf30ec78d, 0xe61d8f1b, 0xcc3b1e37, 0x98763c6f,
	0x30ec78df, 0x61d8f1be, 0xc3b1e37c, 0x8763c6f9, 0x0ec78df3, 0x1d8f1be6, 0x3b1e37cc, 0x763c6f98,
	0xec78df30, 0xd8f1be61, 0xb1e37cc3, 0x63c6f987, 0xc78df30e, 0x8f1be61d, 0x1e37cc3b, 0x3c6f9876,
	0x78df30ec,	0xf1be61d8, 0xe37cc3b1, 0xc6f98763},
	{0x715ea49e, 0xe2bd493c, 0xc57a9279, 0x8af524f3, 0x15ea49e7, 0x2bd493ce, 0x57a9279c, 0xaf524f38,
	0x5ea49e71, 0xbd493ce2, 0x7a9279c5, 0xf524f38a, 0xea49e715, 0xd493ce2b, 0xa9279c57, 0x524f38af,
	0xa49e715e, 0x493ce2bd, 0x9279c57a, 0x24f38af5, 0x49e715ea, 0x93ce2bd4, 0x279c57a9, 0x4f38af52,
	0x9e715ea4, 0x3ce2bd49, 0x79c57a92, 0xf38af524, 0xe715ea49, 0xce2bd493, 0x9c57a927, 0x38af524f,
	0x715ea49e,	0xe2bd493c, 0xc57a9279, 0x8af524f3},
	{0xc785da0a, 0x8f0bb415, 0x1e17682b, 0x3c2ed056, 0x785da0ac, 0xf0bb4158, 0xe17682b1, 0xc2ed0563,
	0x85da0ac7, 0x0bb4158f, 0x17682b1e, 0x2ed0563c, 0x5da0ac78, 0xbb4158f0, 0x7682b1e1, 0xed0563c2,
	0xda0ac785, 0xb4158f0b, 0x682b1e17, 0xd0563c2e, 0xa0ac785d, 0x4158f0bb, 0x82b1e176, 0x0563c2ed,
	0x0ac785da, 0x158f0bb4, 0x2b1e1768, 0x563c2ed0, 0xac785da0, 0x58f0bb41, 0xb1e17682, 0x63c2ed05,
	0xc785da0a, 0x8f0bb415, 0x1e17682b, 0x3c2ed056},
	{0xe04ef22a, 0xc09de455, 0x813bc8ab, 0x02779157, 0x04ef22ae, 0x09de455c, 0x13bc8ab8, 0x27791570,
	0x4ef22ae0, 0x9de455c0, 0x3bc8ab81, 0x77915702, 0xef22ae04, 0xde455c09, 0xbc8ab813, 0x79157027,
	0xf22ae04e, 0xe455c09d, 0xc8ab813b, 0x91570277, 0x22ae04ef, 0x455c09de, 0x8ab813bc, 0x15702779,
	0x2ae04ef2, 0x55c09de4, 0xab813bc8, 0x57027791, 0xae04ef22, 0x5c09de45, 0xb813bc8a, 0x70277915,
	0xe04ef22a,	0xc09de455, 0x813bc8ab, 0x02779157},
	{0xe5c40957, 0xcb8812af, 0x9710255f, 0x2e204abf, 0x5c40957e, 0xb8812afc, 0x710255f9, 0xe204abf2,
	0xc40957e5, 0x8812afcb, 0x10255f97, 0x204abf2e, 0x40957e5c, 0x812afcb8, 0x0255f971, 0x04abf2e2,
	0x0957e5c4, 0x12afcb88, 0x255f9710, 0x4abf2e20, 0x957e5c40, 0x2afcb881, 0x55f97102, 0xabf2e204,
	0x57e5c409, 0xafcb8812, 0x5f971025, 0xbf2e204a, 0x7e5c4095, 0xfcb8812a, 0xf9710255, 0xf2e204ab,
	0xe5c40957,	0xcb8812af, 0x9710255f, 0x2e204abf}
};

static void lea_encrypt_1block_xop(__m128i *ct, const __m128i *pt, const LEA_KEY *key)
{
	__m128i x, tmp, rot, key1, key2;

	rot = _mm_setr_epi32(9, -5, -3, 0);
	x = _mm_loadu_si128((__m128i *)pt);

	LEA_ENC_ROUND(x, tmp, rot, key->rk[  0], key->rk[  1], key->rk[  2], key->rk[  3], key->rk[  4], key->rk[  5], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[  6], key->rk[  7], key->rk[  8], key->rk[  9], key->rk[ 10], key->rk[ 11], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[ 12], key->rk[ 13], key->rk[ 14], key->rk[ 15], key->rk[ 16], key->rk[ 17], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[ 18], key->rk[ 19], key->rk[ 20], key->rk[ 21], key->rk[ 22], key->rk[ 23], key1, key2);
	
	LEA_ENC_ROUND(x, tmp, rot, key->rk[ 24], key->rk[ 25], key->rk[ 26], key->rk[ 27], key->rk[ 28], key->rk[ 29], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[ 30], key->rk[ 31], key->rk[ 32], key->rk[ 33], key->rk[ 34], key->rk[ 35], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[ 36], key->rk[ 37], key->rk[ 38], key->rk[ 39], key->rk[ 40], key->rk[ 41], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[ 42], key->rk[ 43], key->rk[ 44], key->rk[ 45], key->rk[ 46], key->rk[ 47], key1, key2);
	
	LEA_ENC_ROUND(x, tmp, rot, key->rk[ 48], key->rk[ 49], key->rk[ 50], key->rk[ 51], key->rk[ 52], key->rk[ 53], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[ 54], key->rk[ 55], key->rk[ 56], key->rk[ 57], key->rk[ 58], key->rk[ 59], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[ 60], key->rk[ 61], key->rk[ 62], key->rk[ 63], key->rk[ 64], key->rk[ 65], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[ 66], key->rk[ 67], key->rk[ 68], key->rk[ 69], key->rk[ 70], key->rk[ 71], key1, key2);
	
	LEA_ENC_ROUND(x, tmp, rot, key->rk[ 72], key->rk[ 73], key->rk[ 74], key->rk[ 75], key->rk[ 76], key->rk[ 77], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[ 78], key->rk[ 79], key->rk[ 80], key->rk[ 81], key->rk[ 82], key->rk[ 83], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[ 84], key->rk[ 85], key->rk[ 86], key->rk[ 87], key->rk[ 88], key->rk[ 89], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[ 90], key->rk[ 91], key->rk[ 92], key->rk[ 93], key->rk[ 94], key->rk[ 95], key1, key2);
	
	LEA_ENC_ROUND(x, tmp, rot, key->rk[ 96], key->rk[ 97], key->rk[ 98], key->rk[ 99], key->rk[100], key->rk[101], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[102], key->rk[103], key->rk[104], key->rk[105], key->rk[106], key->rk[107], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[108], key->rk[109], key->rk[110], key->rk[111], key->rk[112], key->rk[113], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[114], key->rk[115], key->rk[116], key->rk[117], key->rk[118], key->rk[119], key1, key2);

	LEA_ENC_ROUND(x, tmp, rot, key->rk[120], key->rk[121], key->rk[122], key->rk[123], key->rk[124], key->rk[125], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[126], key->rk[127], key->rk[128], key->rk[129], key->rk[130], key->rk[131], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[132], key->rk[133], key->rk[134], key->rk[135], key->rk[136], key->rk[137], key1, key2);
	LEA_ENC_ROUND(x, tmp, rot, key->rk[138], key->rk[139], key->rk[140], key->rk[141], key->rk[142], key->rk[143], key1, key2);

	if(key->round > 24)
	{
		LEA_ENC_ROUND(x, tmp, rot, key->rk[144], key->rk[145], key->rk[146], key->rk[147], key->rk[148], key->rk[149], key1, key2);
		LEA_ENC_ROUND(x, tmp, rot, key->rk[150], key->rk[151], key->rk[152], key->rk[153], key->rk[154], key->rk[155], key1, key2);
		LEA_ENC_ROUND(x, tmp, rot, key->rk[156], key->rk[157], key->rk[158], key->rk[159], key->rk[160], key->rk[161], key1, key2);
		LEA_ENC_ROUND(x, tmp, rot, key->rk[162], key->rk[163], key->rk[164], key->rk[165], key->rk[166], key->rk[167], key1, key2);
	}

	if(key->round > 28)
	{
		LEA_ENC_ROUND(x, tmp, rot, key->rk[168], key->rk[169], key->rk[170], key->rk[171], key->rk[172], key->rk[173], key1, key2);
		LEA_ENC_ROUND(x, tmp, rot, key->rk[174], key->rk[175], key->rk[176], key->rk[177], key->rk[178], key->rk[179], key1, key2);
		LEA_ENC_ROUND(x, tmp, rot, key->rk[180], key->rk[181], key->rk[182], key->rk[183], key->rk[184], key->rk[185], key1, key2);
		LEA_ENC_ROUND(x, tmp, rot, key->rk[186], key->rk[187], key->rk[188], key->rk[189], key->rk[190], key->rk[191], key1, key2);
	}

	*ct = x;
}

static void lea_encrypt_1block(unsigned char *ct, const unsigned char *pt, const LEA_KEY *key){
	__m128i pt_simd, ct_simd;

	pt_simd = _mm_loadu_si128((__m128i *)pt);

	lea_encrypt_1block_xop(&ct_simd, &pt_simd, key);

	_mm_storeu_si128((__m128i *)ct, ct_simd);
}

static void lea_encrypt_4block(unsigned char *ct, const unsigned char *pt, const LEA_KEY *key)
{
	__m128i x0, x1, x2, x3;
	__m128i tmp0, tmp1, tmp2, tmp3;
	
	x0 = _mm_loadu_si128((__m128i *)(pt     ));
	x1 = _mm_loadu_si128((__m128i *)(pt + 16));
	x2 = _mm_loadu_si128((__m128i *)(pt + 32));
	x3 = _mm_loadu_si128((__m128i *)(pt + 48));

	tmp0 = _mm_unpacklo_epi32(x0, x1);
	tmp1 = _mm_unpacklo_epi32(x2, x3);
	tmp2 = _mm_unpackhi_epi32(x0, x1);
	tmp3 = _mm_unpackhi_epi32(x2, x3);

	x0 = _mm_unpacklo_epi64(tmp0, tmp1);
	x1 = _mm_unpackhi_epi64(tmp0, tmp1);
	x2 = _mm_unpacklo_epi64(tmp2, tmp3);
	x3 = _mm_unpackhi_epi64(tmp2, tmp3);

	XAR3_XOP(x3, x2, key->rk[  4], key->rk[  5]);
	XAR5_XOP(x2, x1, key->rk[  2], key->rk[  3]);
	XAR9_XOP(x1, x0, key->rk[  0], key->rk[  1]);
	XAR3_XOP(x0, x3, key->rk[ 10], key->rk[ 11]);
	XAR5_XOP(x3, x2, key->rk[  8], key->rk[  9]);
	XAR9_XOP(x2, x1, key->rk[  6], key->rk[  7]);
	XAR3_XOP(x1, x0, key->rk[ 16], key->rk[ 17]);
	XAR5_XOP(x0, x3, key->rk[ 14], key->rk[ 15]);
	XAR9_XOP(x3, x2, key->rk[ 12], key->rk[ 13]);
	XAR3_XOP(x2, x1, key->rk[ 22], key->rk[ 23]);
	XAR5_XOP(x1, x0, key->rk[ 20], key->rk[ 21]);
	XAR9_XOP(x0, x3, key->rk[ 18], key->rk[ 19]);

	XAR3_XOP(x3, x2, key->rk[ 28], key->rk[ 29]);
	XAR5_XOP(x2, x1, key->rk[ 26], key->rk[ 27]);
	XAR9_XOP(x1, x0, key->rk[ 24], key->rk[ 25]);
	XAR3_XOP(x0, x3, key->rk[ 34], key->rk[ 35]);
	XAR5_XOP(x3, x2, key->rk[ 32], key->rk[ 33]);
	XAR9_XOP(x2, x1, key->rk[ 30], key->rk[ 31]);
	XAR3_XOP(x1, x0, key->rk[ 40], key->rk[ 41]);
	XAR5_XOP(x0, x3, key->rk[ 38], key->rk[ 39]);
	XAR9_XOP(x3, x2, key->rk[ 36], key->rk[ 37]);
	XAR3_XOP(x2, x1, key->rk[ 46], key->rk[ 47]);
	XAR5_XOP(x1, x0, key->rk[ 44], key->rk[ 45]);
	XAR9_XOP(x0, x3, key->rk[ 42], key->rk[ 43]);

	XAR3_XOP(x3, x2, key->rk[ 52], key->rk[ 53]);
	XAR5_XOP(x2, x1, key->rk[ 50], key->rk[ 51]);
	XAR9_XOP(x1, x0, key->rk[ 48], key->rk[ 49]);
	XAR3_XOP(x0, x3, key->rk[ 58], key->rk[ 59]);
	XAR5_XOP(x3, x2, key->rk[ 56], key->rk[ 57]);
	XAR9_XOP(x2, x1, key->rk[ 54], key->rk[ 55]);
	XAR3_XOP(x1, x0, key->rk[ 64], key->rk[ 65]);
	XAR5_XOP(x0, x3, key->rk[ 62], key->rk[ 63]);
	XAR9_XOP(x3, x2, key->rk[ 60], key->rk[ 61]);
	XAR3_XOP(x2, x1, key->rk[ 70], key->rk[ 71]);
	XAR5_XOP(x1, x0, key->rk[ 68], key->rk[ 69]);
	XAR9_XOP(x0, x3, key->rk[ 66], key->rk[ 67]);

	XAR3_XOP(x3, x2, key->rk[ 76], key->rk[ 77]);
	XAR5_XOP(x2, x1, key->rk[ 74], key->rk[ 75]);
	XAR9_XOP(x1, x0, key->rk[ 72], key->rk[ 73]);
	XAR3_XOP(x0, x3, key->rk[ 82], key->rk[ 83]);
	XAR5_XOP(x3, x2, key->rk[ 80], key->rk[ 81]);
	XAR9_XOP(x2, x1, key->rk[ 78], key->rk[ 79]);
	XAR3_XOP(x1, x0, key->rk[ 88], key->rk[ 89]);
	XAR5_XOP(x0, x3, key->rk[ 86], key->rk[ 87]);
	XAR9_XOP(x3, x2, key->rk[ 84], key->rk[ 85]);
	XAR3_XOP(x2, x1, key->rk[ 94], key->rk[ 95]);
	XAR5_XOP(x1, x0, key->rk[ 92], key->rk[ 93]);
	XAR9_XOP(x0, x3, key->rk[ 90], key->rk[ 91]);

	XAR3_XOP(x3, x2, key->rk[100], key->rk[101]);
	XAR5_XOP(x2, x1, key->rk[ 98], key->rk[ 99]);
	XAR9_XOP(x1, x0, key->rk[ 96], key->rk[ 97]);
	XAR3_XOP(x0, x3, key->rk[106], key->rk[107]);
	XAR5_XOP(x3, x2, key->rk[104], key->rk[105]);
	XAR9_XOP(x2, x1, key->rk[102], key->rk[103]);
	XAR3_XOP(x1, x0, key->rk[112], key->rk[113]);
	XAR5_XOP(x0, x3, key->rk[110], key->rk[111]);
	XAR9_XOP(x3, x2, key->rk[108], key->rk[109]);
	XAR3_XOP(x2, x1, key->rk[118], key->rk[119]);
	XAR5_XOP(x1, x0, key->rk[116], key->rk[117]);
	XAR9_XOP(x0, x3, key->rk[114], key->rk[115]);

	XAR3_XOP(x3, x2, key->rk[124], key->rk[125]);
	XAR5_XOP(x2, x1, key->rk[122], key->rk[123]);
	XAR9_XOP(x1, x0, key->rk[120], key->rk[121]);
	XAR3_XOP(x0, x3, key->rk[130], key->rk[131]);
	XAR5_XOP(x3, x2, key->rk[128], key->rk[129]);
	XAR9_XOP(x2, x1, key->rk[126], key->rk[127]);
	XAR3_XOP(x1, x0, key->rk[136], key->rk[137]);
	XAR5_XOP(x0, x3, key->rk[134], key->rk[135]);
	XAR9_XOP(x3, x2, key->rk[132], key->rk[133]);
	XAR3_XOP(x2, x1, key->rk[142], key->rk[143]);
	XAR5_XOP(x1, x0, key->rk[140], key->rk[141]);
	XAR9_XOP(x0, x3, key->rk[138], key->rk[139]);

	if(key->round > 24)
	{
		XAR3_XOP(x3, x2, key->rk[148], key->rk[149]);
		XAR5_XOP(x2, x1, key->rk[146], key->rk[147]);
		XAR9_XOP(x1, x0, key->rk[144], key->rk[145]);
		XAR3_XOP(x0, x3, key->rk[154], key->rk[155]);
		XAR5_XOP(x3, x2, key->rk[152], key->rk[153]);
		XAR9_XOP(x2, x1, key->rk[150], key->rk[151]);
		XAR3_XOP(x1, x0, key->rk[160], key->rk[161]);
		XAR5_XOP(x0, x3, key->rk[158], key->rk[159]);
		XAR9_XOP(x3, x2, key->rk[156], key->rk[157]);
		XAR3_XOP(x2, x1, key->rk[166], key->rk[167]);
		XAR5_XOP(x1, x0, key->rk[164], key->rk[165]);
		XAR9_XOP(x0, x3, key->rk[162], key->rk[163]);
	}

	if(key->round > 28)
	{
		XAR3_XOP(x3, x2, key->rk[172], key->rk[173]);
		XAR5_XOP(x2, x1, key->rk[170], key->rk[171]);
		XAR9_XOP(x1, x0, key->rk[168], key->rk[169]);
		XAR3_XOP(x0, x3, key->rk[178], key->rk[179]);
		XAR5_XOP(x3, x2, key->rk[176], key->rk[177]);
		XAR9_XOP(x2, x1, key->rk[174], key->rk[175]);
		XAR3_XOP(x1, x0, key->rk[184], key->rk[185]);
		XAR5_XOP(x0, x3, key->rk[182], key->rk[183]);
		XAR9_XOP(x3, x2, key->rk[180], key->rk[181]);
		XAR3_XOP(x2, x1, key->rk[190], key->rk[191]);
		XAR5_XOP(x1, x0, key->rk[188], key->rk[189]);
		XAR9_XOP(x0, x3, key->rk[186], key->rk[187]);
	}

	tmp0 = _mm_unpacklo_epi32(x0, x1);
	tmp1 = _mm_unpacklo_epi32(x2, x3);
	tmp2 = _mm_unpackhi_epi32(x0, x1);
	tmp3 = _mm_unpackhi_epi32(x2, x3);

	x0 = _mm_unpacklo_epi64(tmp0, tmp1);
	x1 = _mm_unpackhi_epi64(tmp0, tmp1);
	x2 = _mm_unpacklo_epi64(tmp2, tmp3);
	x3 = _mm_unpackhi_epi64(tmp2, tmp3);

	_mm_storeu_si128((__m128i *)(ct     ), x0);
	_mm_storeu_si128((__m128i *)(ct + 16), x1);
	_mm_storeu_si128((__m128i *)(ct + 32), x2);
	_mm_storeu_si128((__m128i *)(ct + 48), x3);
}

static void lea_decrypt_4block(unsigned char *pt, const unsigned char *ct, const LEA_KEY *key)
{
	__m128i x0, x1, x2, x3;
	__m128i tmp0, tmp1, tmp2, tmp3;
	
	x0 = _mm_loadu_si128((__m128i *)(ct     ));
	x1 = _mm_loadu_si128((__m128i *)(ct + 16));
	x2 = _mm_loadu_si128((__m128i *)(ct + 32));
	x3 = _mm_loadu_si128((__m128i *)(ct + 48));

	tmp0 = _mm_unpacklo_epi32(x0, x1);
	tmp1 = _mm_unpacklo_epi32(x2, x3);
	tmp2 = _mm_unpackhi_epi32(x0, x1);
	tmp3 = _mm_unpackhi_epi32(x2, x3);

	x0 = _mm_unpacklo_epi64(tmp0, tmp1);
	x1 = _mm_unpackhi_epi64(tmp0, tmp1);
	x2 = _mm_unpacklo_epi64(tmp2, tmp3);
	x3 = _mm_unpackhi_epi64(tmp2, tmp3);

	if(key->round > 28)
	{
		XSR9_XOP(x0, x3, key->rk[186], key->rk[187]);
		XSR5_XOP(x1, x0, key->rk[188], key->rk[189]);
		XSR3_XOP(x2, x1, key->rk[190], key->rk[191]);
		XSR9_XOP(x3, x2, key->rk[180], key->rk[181]);
		XSR5_XOP(x0, x3, key->rk[182], key->rk[183]);
		XSR3_XOP(x1, x0, key->rk[184], key->rk[185]);
		XSR9_XOP(x2, x1, key->rk[174], key->rk[175]);
		XSR5_XOP(x3, x2, key->rk[176], key->rk[177]);
		XSR3_XOP(x0, x3, key->rk[178], key->rk[179]);
		XSR9_XOP(x1, x0, key->rk[168], key->rk[169]);
		XSR5_XOP(x2, x1, key->rk[170], key->rk[171]);
		XSR3_XOP(x3, x2, key->rk[172], key->rk[173]);
	}

	if(key->round > 24)
	{
		XSR9_XOP(x0, x3, key->rk[162], key->rk[163]);
		XSR5_XOP(x1, x0, key->rk[164], key->rk[165]);
		XSR3_XOP(x2, x1, key->rk[166], key->rk[167]);
		XSR9_XOP(x3, x2, key->rk[156], key->rk[157]);
		XSR5_XOP(x0, x3, key->rk[158], key->rk[159]);
		XSR3_XOP(x1, x0, key->rk[160], key->rk[161]);
		XSR9_XOP(x2, x1, key->rk[150], key->rk[151]);
		XSR5_XOP(x3, x2, key->rk[152], key->rk[153]);
		XSR3_XOP(x0, x3, key->rk[154], key->rk[155]);
		XSR9_XOP(x1, x0, key->rk[144], key->rk[145]);
		XSR5_XOP(x2, x1, key->rk[146], key->rk[147]);
		XSR3_XOP(x3, x2, key->rk[148], key->rk[149]);
	}

	XSR9_XOP(x0, x3, key->rk[138], key->rk[139]);
	XSR5_XOP(x1, x0, key->rk[140], key->rk[141]);
	XSR3_XOP(x2, x1, key->rk[142], key->rk[143]);
	XSR9_XOP(x3, x2, key->rk[132], key->rk[133]);
	XSR5_XOP(x0, x3, key->rk[134], key->rk[135]);
	XSR3_XOP(x1, x0, key->rk[136], key->rk[137]);
	XSR9_XOP(x2, x1, key->rk[126], key->rk[127]);
	XSR5_XOP(x3, x2, key->rk[128], key->rk[129]);
	XSR3_XOP(x0, x3, key->rk[130], key->rk[131]);
	XSR9_XOP(x1, x0, key->rk[120], key->rk[121]);
	XSR5_XOP(x2, x1, key->rk[122], key->rk[123]);
	XSR3_XOP(x3, x2, key->rk[124], key->rk[125]);

	XSR9_XOP(x0, x3, key->rk[114], key->rk[115]);
	XSR5_XOP(x1, x0, key->rk[116], key->rk[117]);
	XSR3_XOP(x2, x1, key->rk[118], key->rk[119]);
	XSR9_XOP(x3, x2, key->rk[108], key->rk[109]);
	XSR5_XOP(x0, x3, key->rk[110], key->rk[111]);
	XSR3_XOP(x1, x0, key->rk[112], key->rk[113]);
	XSR9_XOP(x2, x1, key->rk[102], key->rk[103]);
	XSR5_XOP(x3, x2, key->rk[104], key->rk[105]);
	XSR3_XOP(x0, x3, key->rk[106], key->rk[107]);
	XSR9_XOP(x1, x0, key->rk[ 96], key->rk[ 97]);
	XSR5_XOP(x2, x1, key->rk[ 98], key->rk[ 99]);
	XSR3_XOP(x3, x2, key->rk[100], key->rk[101]);

	XSR9_XOP(x0, x3, key->rk[ 90], key->rk[ 91]);
	XSR5_XOP(x1, x0, key->rk[ 92], key->rk[ 93]);
	XSR3_XOP(x2, x1, key->rk[ 94], key->rk[ 95]);
	XSR9_XOP(x3, x2, key->rk[ 84], key->rk[ 85]);
	XSR5_XOP(x0, x3, key->rk[ 86], key->rk[ 87]);
	XSR3_XOP(x1, x0, key->rk[ 88], key->rk[ 89]);
	XSR9_XOP(x2, x1, key->rk[ 78], key->rk[ 79]);
	XSR5_XOP(x3, x2, key->rk[ 80], key->rk[ 81]);
	XSR3_XOP(x0, x3, key->rk[ 82], key->rk[ 83]);
	XSR9_XOP(x1, x0, key->rk[ 72], key->rk[ 73]);
	XSR5_XOP(x2, x1, key->rk[ 74], key->rk[ 75]);
	XSR3_XOP(x3, x2, key->rk[ 76], key->rk[ 77]);

	XSR9_XOP(x0, x3, key->rk[ 66], key->rk[ 67]);
	XSR5_XOP(x1, x0, key->rk[ 68], key->rk[ 69]);
	XSR3_XOP(x2, x1, key->rk[ 70], key->rk[ 71]);
	XSR9_XOP(x3, x2, key->rk[ 60], key->rk[ 61]);
	XSR5_XOP(x0, x3, key->rk[ 62], key->rk[ 63]);
	XSR3_XOP(x1, x0, key->rk[ 64], key->rk[ 65]);
	XSR9_XOP(x2, x1, key->rk[ 54], key->rk[ 55]);
	XSR5_XOP(x3, x2, key->rk[ 56], key->rk[ 57]);
	XSR3_XOP(x0, x3, key->rk[ 58], key->rk[ 59]);
	XSR9_XOP(x1, x0, key->rk[ 48], key->rk[ 49]);
	XSR5_XOP(x2, x1, key->rk[ 50], key->rk[ 51]);
	XSR3_XOP(x3, x2, key->rk[ 52], key->rk[ 53]);

	XSR9_XOP(x0, x3, key->rk[ 42], key->rk[ 43]);
	XSR5_XOP(x1, x0, key->rk[ 44], key->rk[ 45]);
	XSR3_XOP(x2, x1, key->rk[ 46], key->rk[ 47]);
	XSR9_XOP(x3, x2, key->rk[ 36], key->rk[ 37]);
	XSR5_XOP(x0, x3, key->rk[ 38], key->rk[ 39]);
	XSR3_XOP(x1, x0, key->rk[ 40], key->rk[ 41]);
	XSR9_XOP(x2, x1, key->rk[ 30], key->rk[ 31]);
	XSR5_XOP(x3, x2, key->rk[ 32], key->rk[ 33]);
	XSR3_XOP(x0, x3, key->rk[ 34], key->rk[ 35]);
	XSR9_XOP(x1, x0, key->rk[ 24], key->rk[ 25]);
	XSR5_XOP(x2, x1, key->rk[ 26], key->rk[ 27]);
	XSR3_XOP(x3, x2, key->rk[ 28], key->rk[ 29]);

	XSR9_XOP(x0, x3, key->rk[ 18], key->rk[ 19]);
	XSR5_XOP(x1, x0, key->rk[ 20], key->rk[ 21]);
	XSR3_XOP(x2, x1, key->rk[ 22], key->rk[ 23]);
	XSR9_XOP(x3, x2, key->rk[ 12], key->rk[ 13]);
	XSR5_XOP(x0, x3, key->rk[ 14], key->rk[ 15]);
	XSR3_XOP(x1, x0, key->rk[ 16], key->rk[ 17]);
	XSR9_XOP(x2, x1, key->rk[  6], key->rk[  7]);
	XSR5_XOP(x3, x2, key->rk[  8], key->rk[  9]);
	XSR3_XOP(x0, x3, key->rk[ 10], key->rk[ 11]);
	XSR9_XOP(x1, x0, key->rk[  0], key->rk[  1]);
	XSR5_XOP(x2, x1, key->rk[  2], key->rk[  3]);
	XSR3_XOP(x3, x2, key->rk[  4], key->rk[  5]);

	tmp0 = _mm_unpacklo_epi32(x0, x1);
	tmp1 = _mm_unpacklo_epi32(x2, x3);
	tmp2 = _mm_unpackhi_epi32(x0, x1);
	tmp3 = _mm_unpackhi_epi32(x2, x3);

	x0 = _mm_unpacklo_epi64(tmp0, tmp1);
	x1 = _mm_unpackhi_epi64(tmp0, tmp1);
	x2 = _mm_unpacklo_epi64(tmp2, tmp3);
	x3 = _mm_unpackhi_epi64(tmp2, tmp3);

	_mm_storeu_si128((__m128i *)(pt     ), x0);
	_mm_storeu_si128((__m128i *)(pt + 16), x1);
	_mm_storeu_si128((__m128i *)(pt + 32), x2);
	_mm_storeu_si128((__m128i *)(pt + 48), x3);
}

static void _lea_cbc_enc(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key){
	unsigned int nBlock1 = pt_len >> 4, i;
	__m128i pt_simd, ct_simd, iv_simd;
	iv_simd = _mm_loadu_si128((__m128i *)iv);

	for (i = 0; i < nBlock1; i++, pt += 16, ct += 16)
	{
		pt_simd = _mm_loadu_si128((__m128i *)pt);

		ct_simd = _mm_xor_si128(iv_simd, pt_simd);

		lea_encrypt_1block_xop(&ct_simd, &ct_simd, key);

		iv_simd = ct_simd;

		_mm_storeu_si128((__m128i *)ct, ct_simd);
	}
}

static void _lea_ctr_enc(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, unsigned char *ctr, const LEA_KEY *key){
	unsigned char block[64];
	unsigned int numBlock4 = pt_len >> 6;
	unsigned int numBlock1 = (pt_len & 0x3f) >> 4, i;
	__m128i pt_simd, ct_simd, block_simd, ctr_simd;

	for (i = 0; i < numBlock4; i++, pt += 0x40, ct += 0x40)
	{
		CPY8x16(block, ctr);
		ctr128_inc_aligned(ctr);

		CPY8x16(block + 0x10, ctr);
		ctr128_inc_aligned(ctr);

		CPY8x16(block + 0x20, ctr);
		ctr128_inc_aligned(ctr);

		CPY8x16(block + 0x30, ctr);
		ctr128_inc_aligned(ctr);

		lea_encrypt_4block(block, block, key);

		XOR8x64(ct, block, pt);
	}

	ctr_simd = _mm_loadu_si128((__m128i *)ctr);

	for (i = 0; i < numBlock1; i++, pt += 0x10, ct += 0x10)
	{
		lea_encrypt_1block_xop(&block_simd, &ctr_simd, key);

		block_simd = _mm_xor_si128(block_simd, _mm_loadu_si128((__m128i *)pt));
		_mm_storeu_si128((__m128i *)ct, block_simd);

		ctr128_inc_xop(&ctr_simd);
	}

	if ((numBlock4 * 64 + numBlock1 * 16) < pt_len)
	{
		lea_encrypt_1block_xop(&block_simd, &ctr_simd, key);

		block_simd = _mm_xor_si128(block_simd, _mm_loadu_si128((__m128i *)pt));

		_mm_storeu_si128((__m128i *)block, block_simd);

		for (i = 0; i < (pt_len & 0xf); i++)
			ct[i] = block[i];
	}

	_mm_storeu_si128((__m128i *)ctr, ctr_simd);
}

static void _lea_cfb128_enc(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key){
	__m128i pt_simd, iv_simd, tmp_simd;
	unsigned int nBlock1 = pt_len >> 4, i;

	iv_simd = _mm_loadu_si128((__m128i *)iv);

	for (i = 0; i < nBlock1; i++, pt += 0x10, ct += 0x10)
	{
		pt_simd = _mm_loadu_si128((__m128i *)pt);

		lea_encrypt_1block_xop(&iv_simd, &iv_simd, key);

		iv_simd = _mm_xor_si128(iv_simd, pt_simd);

		_mm_storeu_si128((__m128i *)ct, iv_simd);
	}
}

static void _lea_cfb128_dec(unsigned char *pt, const unsigned char *ct, unsigned int ct_len, const unsigned char *iv, const LEA_KEY *key){
	__m128i pt_simd, iv_simd;
	unsigned int nBlock1 = ct_len >> 4, i;

	iv_simd = _mm_loadu_si128((__m128i *)iv);

	for (i = 0; i < nBlock1; i++, pt += 0x10, ct += 0x10)
	{
		lea_encrypt_1block_xop(&pt_simd, &iv_simd, key);

		iv_simd = _mm_loadu_si128((__m128i *)ct);

		pt_simd = _mm_xor_si128(pt_simd, iv_simd);

		_mm_storeu_si128((__m128i *)pt, pt_simd);
	}
}

static void _lea_ofb_enc(unsigned char *ct, const unsigned char *pt, unsigned int pt_len, const unsigned char *iv, const LEA_KEY *key){
	__m128i pt_simd, ct_simd, iv_simd;
	unsigned int numBlock1 = pt_len >> 4, i;

	iv_simd = _mm_loadu_si128((__m128i *)iv);

	for (i = 0; i < numBlock1; i++, pt += 0x10, ct += 0x10)
	{
		lea_encrypt_1block_xop(&iv_simd, &iv_simd, key);

		pt_simd = _mm_loadu_si128((__m128i *)pt);

		ct_simd = _mm_xor_si128(pt_simd, iv_simd);

		_mm_storeu_si128((__m128i *)ct, ct_simd);
	}
}

static void ctr128_inc_xop(__m128i *counter)
{
	__m128i one = _mm_setr_epi32(0, 0, 0, 0x01000000);
	int t;

	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 15))
		return;
	one = _mm_srli_epi32(one, 8);

	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 14))
		return;
	one = _mm_srli_epi32(one, 8);

	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 13))
		return;
	one = _mm_srli_epi32(one, 8);

	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 12))
		return;
	one = _mm_srli_epi32(one, 8);

	one = _mm_shuffle_epi32(one, _MM_SHUFFLE(0, 3, 2, 1));
	one = _mm_roti_epi32(one, 8);



	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 11))
		return;
	one = _mm_srli_epi32(one, 8);

	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 10))
		return;
	one = _mm_srli_epi32(one, 8);

	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 9))
		return;
	one = _mm_srli_epi32(one, 8);

	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 8))
		return;
	one = _mm_srli_epi32(one, 8);

	one = _mm_shuffle_epi32(one, _MM_SHUFFLE(0, 3, 2, 1));
	one = _mm_roti_epi32(one, 8);



	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 7))
		return;
	one = _mm_srli_epi32(one, 8);

	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 6))
		return;
	one = _mm_srli_epi32(one, 8);

	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 5))
		return;
	one = _mm_srli_epi32(one, 8);

	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 4))
		return;
	one = _mm_srli_epi32(one, 8);

	one = _mm_shuffle_epi32(one, _MM_SHUFFLE(0, 3, 2, 1));
	one = _mm_roti_epi32(one, 8);



	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 3))
		return;
	one = _mm_srli_epi32(one, 8);

	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 2))
		return;
	one = _mm_srli_epi32(one, 8);

	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 1))
		return;
	one = _mm_srli_epi32(one, 8);

	*counter = _mm_add_epi8(*counter, one);
	if (_mm_extract_epi8(*counter, 0))
		return;
}

#endif	//	(__XOP__)
