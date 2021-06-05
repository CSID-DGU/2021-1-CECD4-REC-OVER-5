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
	{ 0xc3efe9db, 0x87dfd3b7, 0x0fbfa76f, 0x1f7f4ede, 0x3efe9dbc, 0x7dfd3b78, 0xfbfa76f0, 0xf7f4ede1,
	0xefe9dbc3, 0xdfd3b787, 0xbfa76f0f, 0x7f4ede1f, 0xfe9dbc3e, 0xfd3b787d, 0xfa76f0fb, 0xf4ede1f7,
	0xe9dbc3ef, 0xd3b787df, 0xa76f0fbf, 0x4ede1f7f, 0x9dbc3efe, 0x3b787dfd, 0x76f0fbfa, 0xede1f7f4,
	0xdbc3efe9, 0xb787dfd3, 0x6f0fbfa7, 0xde1f7f4e, 0xbc3efe9d, 0x787dfd3b, 0xf0fbfa76, 0xe1f7f4eD,
	0xc3efe9db, 0x87dfd3b7, 0x0fbfa76f, 0x1f7f4ede },
	{ 0x44626b02, 0x88c4d604, 0x1189ac09, 0x23135812, 0x4626b024, 0x8c4d6048, 0x189ac091, 0x31358122,
	0x626b0244, 0xc4d60488, 0x89ac0911, 0x13581223, 0x26b02446, 0x4d60488c, 0x9ac09118, 0x35812231,
	0x6b024462, 0xd60488c4, 0xac091189, 0x58122313, 0xb0244626, 0x60488c4d, 0xc091189a, 0x81223135,
	0x0244626b, 0x0488c4d6, 0x091189ac, 0x12231358, 0x244626b0, 0x488c4d60, 0x91189ac0, 0x22313581,
	0x44626b02, 0x88c4d604, 0x1189ac09, 0x23135812 },
	{ 0x79e27c8a, 0xf3c4f914, 0xe789f229, 0xcf13e453, 0x9e27c8a7, 0x3c4f914f, 0x789f229e, 0xf13e453c,
	0xe27c8a79, 0xc4f914f3, 0x89f229e7, 0x13e453cf, 0x27c8a79e, 0x4f914f3c, 0x9f229e78, 0x3e453cf1,
	0x7c8a79e2, 0xf914f3c4, 0xf229e789, 0xe453cf13, 0xc8a79e27, 0x914f3c4f, 0x229e789f, 0x453cf13e,
	0x8a79e27c, 0x14f3c4f9, 0x29e789f2, 0x53cf13e4, 0xa79e27c8, 0x4f3c4f91, 0x9e789f22, 0x3cf13e45,
	0x79e27c8a, 0xf3c4f914, 0xe789f229, 0xcf13e453 },
	{ 0x78df30ec, 0xf1be61d8, 0xe37cc3b1, 0xc6f98763, 0x8df30ec7, 0x1be61d8f, 0x37cc3b1e, 0x6f98763c,
	0xdf30ec78, 0xbe61d8f1, 0x7cc3b1e3, 0xf98763c6, 0xf30ec78d, 0xe61d8f1b, 0xcc3b1e37, 0x98763c6f,
	0x30ec78df, 0x61d8f1be, 0xc3b1e37c, 0x8763c6f9, 0x0ec78df3, 0x1d8f1be6, 0x3b1e37cc, 0x763c6f98,
	0xec78df30, 0xd8f1be61, 0xb1e37cc3, 0x63c6f987, 0xc78df30e, 0x8f1be61d, 0x1e37cc3b, 0x3c6f9876,
	0x78df30ec, 0xf1be61d8, 0xe37cc3b1, 0xc6f98763 },
	{ 0x715ea49e, 0xe2bd493c, 0xc57a9279, 0x8af524f3, 0x15ea49e7, 0x2bd493ce, 0x57a9279c, 0xaf524f38,
	0x5ea49e71, 0xbd493ce2, 0x7a9279c5, 0xf524f38a, 0xea49e715, 0xd493ce2b, 0xa9279c57, 0x524f38af,
	0xa49e715e, 0x493ce2bd, 0x9279c57a, 0x24f38af5, 0x49e715ea, 0x93ce2bd4, 0x279c57a9, 0x4f38af52,
	0x9e715ea4, 0x3ce2bd49, 0x79c57a92, 0xf38af524, 0xe715ea49, 0xce2bd493, 0x9c57a927, 0x38af524f,
	0x715ea49e, 0xe2bd493c, 0xc57a9279, 0x8af524f3 },
	{ 0xc785da0a, 0x8f0bb415, 0x1e17682b, 0x3c2ed056, 0x785da0ac, 0xf0bb4158, 0xe17682b1, 0xc2ed0563,
	0x85da0ac7, 0x0bb4158f, 0x17682b1e, 0x2ed0563c, 0x5da0ac78, 0xbb4158f0, 0x7682b1e1, 0xed0563c2,
	0xda0ac785, 0xb4158f0b, 0x682b1e17, 0xd0563c2e, 0xa0ac785d, 0x4158f0bb, 0x82b1e176, 0x0563c2ed,
	0x0ac785da, 0x158f0bb4, 0x2b1e1768, 0x563c2ed0, 0xac785da0, 0x58f0bb41, 0xb1e17682, 0x63c2ed05,
	0xc785da0a, 0x8f0bb415, 0x1e17682b, 0x3c2ed056 },
	{ 0xe04ef22a, 0xc09de455, 0x813bc8ab, 0x02779157, 0x04ef22ae, 0x09de455c, 0x13bc8ab8, 0x27791570,
	0x4ef22ae0, 0x9de455c0, 0x3bc8ab81, 0x77915702, 0xef22ae04, 0xde455c09, 0xbc8ab813, 0x79157027,
	0xf22ae04e, 0xe455c09d, 0xc8ab813b, 0x91570277, 0x22ae04ef, 0x455c09de, 0x8ab813bc, 0x15702779,
	0x2ae04ef2, 0x55c09de4, 0xab813bc8, 0x57027791, 0xae04ef22, 0x5c09de45, 0xb813bc8a, 0x70277915,
	0xe04ef22a, 0xc09de455, 0x813bc8ab, 0x02779157 },
	{ 0xe5c40957, 0xcb8812af, 0x9710255f, 0x2e204abf, 0x5c40957e, 0xb8812afc, 0x710255f9, 0xe204abf2,
	0xc40957e5, 0x8812afcb, 0x10255f97, 0x204abf2e, 0x40957e5c, 0x812afcb8, 0x0255f971, 0x04abf2e2,
	0x0957e5c4, 0x12afcb88, 0x255f9710, 0x4abf2e20, 0x957e5c40, 0x2afcb881, 0x55f97102, 0xabf2e204,
	0x57e5c409, 0xafcb8812, 0x5f971025, 0xbf2e204a, 0x7e5c4095, 0xfcb8812a, 0xf9710255, 0xf2e204ab,
	0xe5c40957, 0xcb8812af, 0x9710255f, 0x2e204abf }
};

void lea_set_key_xop(LEA_KEY *key, const unsigned char *mk, unsigned int mk_len)
{
	__m128i t1, t2, rot1, rot2, rot3;

	key->round = (mk_len >> 1) + 16;

	switch (mk_len)
	{
	case 16:
		t1 = _mm_loadu_si128((__m128i *)mk);
		rot1 = _mm_setr_epi32(1, 3, 6, 11);

		SET_128ROUND_KEY(key->rk, key->rk + 2, t1, delta[0], rot1);
		SET_128ROUND_KEY(key->rk + 6, key->rk + 8, t1, delta[1] + 1, rot1);
		SET_128ROUND_KEY(key->rk + 12, key->rk + 14, t1, delta[2] + 2, rot1);
		SET_128ROUND_KEY(key->rk + 18, key->rk + 20, t1, delta[3] + 3, rot1);

		SET_128ROUND_KEY(key->rk + 24, key->rk + 26, t1, delta[0] + 4, rot1);
		SET_128ROUND_KEY(key->rk + 30, key->rk + 32, t1, delta[1] + 5, rot1);
		SET_128ROUND_KEY(key->rk + 36, key->rk + 38, t1, delta[2] + 6, rot1);
		SET_128ROUND_KEY(key->rk + 42, key->rk + 44, t1, delta[3] + 7, rot1);

		SET_128ROUND_KEY(key->rk + 48, key->rk + 50, t1, delta[0] + 8, rot1);
		SET_128ROUND_KEY(key->rk + 54, key->rk + 56, t1, delta[1] + 9, rot1);
		SET_128ROUND_KEY(key->rk + 60, key->rk + 62, t1, delta[2] + 10, rot1);
		SET_128ROUND_KEY(key->rk + 66, key->rk + 68, t1, delta[3] + 11, rot1);

		SET_128ROUND_KEY(key->rk + 72, key->rk + 74, t1, delta[0] + 12, rot1);
		SET_128ROUND_KEY(key->rk + 78, key->rk + 80, t1, delta[1] + 13, rot1);
		SET_128ROUND_KEY(key->rk + 84, key->rk + 86, t1, delta[2] + 14, rot1);
		SET_128ROUND_KEY(key->rk + 90, key->rk + 92, t1, delta[3] + 15, rot1);

		SET_128ROUND_KEY(key->rk + 96, key->rk + 98, t1, delta[0] + 16, rot1);
		SET_128ROUND_KEY(key->rk + 102, key->rk + 104, t1, delta[1] + 17, rot1);
		SET_128ROUND_KEY(key->rk + 108, key->rk + 110, t1, delta[2] + 18, rot1);
		SET_128ROUND_KEY(key->rk + 114, key->rk + 116, t1, delta[3] + 19, rot1);

		SET_128ROUND_KEY(key->rk + 120, key->rk + 122, t1, delta[0] + 20, rot1);
		SET_128ROUND_KEY(key->rk + 126, key->rk + 128, t1, delta[1] + 21, rot1);
		SET_128ROUND_KEY(key->rk + 132, key->rk + 134, t1, delta[2] + 22, rot1);
		SET_128ROUND_KEY(key->rk + 138, key->rk + 140, t1, delta[3] + 23, rot1);
		break;

	case 24:
		t1 = _mm_loadu_si128((__m128i *)mk);
		t2 = _mm_loadu_si128((__m128i *)(mk + 8));
		rot1 = _mm_setr_epi32(1, 3, 6, 11);
		rot2 = _mm_setr_epi32(6, 11, 13, 17);

		SET_192ROUND_KEY(key->rk, key->rk + 2, t1, t2, delta[0], delta[0] + 2, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 6, key->rk + 8, t1, t2, delta[1] + 1, delta[1] + 3, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 12, key->rk + 14, t1, t2, delta[2] + 2, delta[2] + 4, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 18, key->rk + 20, t1, t2, delta[3] + 3, delta[3] + 5, rot1, rot2);

		SET_192ROUND_KEY(key->rk + 24, key->rk + 26, t1, t2, delta[4] + 4, delta[4] + 6, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 30, key->rk + 32, t1, t2, delta[5] + 5, delta[5] + 7, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 36, key->rk + 38, t1, t2, delta[0] + 6, delta[0] + 8, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 42, key->rk + 44, t1, t2, delta[1] + 7, delta[1] + 9, rot1, rot2);

		SET_192ROUND_KEY(key->rk + 48, key->rk + 50, t1, t2, delta[2] + 8, delta[2] + 10, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 54, key->rk + 56, t1, t2, delta[3] + 9, delta[3] + 11, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 60, key->rk + 62, t1, t2, delta[4] + 10, delta[4] + 12, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 66, key->rk + 68, t1, t2, delta[5] + 11, delta[5] + 13, rot1, rot2);

		SET_192ROUND_KEY(key->rk + 72, key->rk + 74, t1, t2, delta[0] + 12, delta[0] + 14, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 78, key->rk + 80, t1, t2, delta[1] + 13, delta[1] + 15, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 84, key->rk + 86, t1, t2, delta[2] + 14, delta[2] + 16, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 90, key->rk + 92, t1, t2, delta[3] + 15, delta[3] + 17, rot1, rot2);

		SET_192ROUND_KEY(key->rk + 96, key->rk + 98, t1, t2, delta[4] + 16, delta[4] + 18, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 102, key->rk + 104, t1, t2, delta[5] + 17, delta[5] + 19, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 108, key->rk + 110, t1, t2, delta[0] + 18, delta[0] + 20, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 114, key->rk + 116, t1, t2, delta[1] + 19, delta[1] + 21, rot1, rot2);

		SET_192ROUND_KEY(key->rk + 120, key->rk + 122, t1, t2, delta[2] + 20, delta[2] + 22, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 126, key->rk + 128, t1, t2, delta[3] + 21, delta[3] + 23, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 132, key->rk + 134, t1, t2, delta[4] + 22, delta[4] + 24, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 138, key->rk + 140, t1, t2, delta[5] + 23, delta[5] + 25, rot1, rot2);

		SET_192ROUND_KEY(key->rk + 144, key->rk + 146, t1, t2, delta[0] + 24, delta[0] + 26, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 150, key->rk + 152, t1, t2, delta[1] + 25, delta[1] + 27, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 156, key->rk + 158, t1, t2, delta[2] + 26, delta[2] + 28, rot1, rot2);
		SET_192ROUND_KEY(key->rk + 162, key->rk + 164, t1, t2, delta[3] + 27, delta[3] + 29, rot1, rot2);
		break;

	case 32:
		rot1 = _mm_setr_epi32(1, 3, 6, 11);
		rot2 = _mm_setr_epi32(6, 11, 13, 17);
		rot3 = _mm_setr_epi32(13, 17, 1, 3);

		t1 = _mm_loadu_si128((__m128i *)mk);
		SET_256ROUND_KEY_TYPE1(key->rk, t1, delta[0], rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 8, t1, delta[1] + 3, rot2);
		SET_256ROUND_KEY_TYPE2(key->rk + 16, t1, delta[2][6], delta[2][7], delta[3][3], delta[3][4], rot3);

		SET_256ROUND_KEY_TYPE1(key->rk + 24, t1, delta[4] + 4, rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 32, t1, delta[5] + 7, rot2);
		SET_256ROUND_KEY_TYPE2(key->rk + 40, t1, delta[6][10], delta[6][11], delta[7][7], delta[7][8], rot3);

		SET_256ROUND_KEY_TYPE1(key->rk + 48, t1, delta[0] + 8, rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 56, t1, delta[1] + 11, rot2);
		SET_256ROUND_KEY_TYPE2(key->rk + 64, t1, delta[2][14], delta[2][15], delta[3][11], delta[3][12], rot3);

		SET_256ROUND_KEY_TYPE1(key->rk + 72, t1, delta[4] + 12, rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 80, t1, delta[5] + 15, rot2);
		SET_256ROUND_KEY_TYPE2(key->rk + 88, t1, delta[6][18], delta[6][19], delta[7][15], delta[7][16], rot3);

		SET_256ROUND_KEY_TYPE1(key->rk + 96, t1, delta[0] + 16, rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 104, t1, delta[1] + 19, rot2);
		SET_256ROUND_KEY_TYPE2(key->rk + 112, t1, delta[2][22], delta[2][23], delta[3][19], delta[3][20], rot3);

		SET_256ROUND_KEY_TYPE1(key->rk + 120, t1, delta[4] + 20, rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 128, t1, delta[5] + 23, rot2);
		SET_256ROUND_KEY_TYPE2(key->rk + 136, t1, delta[6][26], delta[6][27], delta[7][23], delta[7][24], rot3);

		SET_256ROUND_KEY_TYPE1(key->rk + 144, t1, delta[0] + 24, rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 152, t1, delta[1] + 27, rot2);
		SET_256ROUND_KEY_TYPE2(key->rk + 160, t1, delta[2][30], delta[2][31], delta[3][27], delta[3][28], rot3);

		SET_256ROUND_KEY_TYPE1(key->rk + 168, t1, delta[4] + 28, rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 176, t1, delta[5] + 31, rot2);
		SET_256ROUND_KEY_TYPE2(key->rk + 184, t1, delta[6][34], delta[6][35], delta[7][31], delta[7][32], rot3);



		t2 = _mm_loadu_si128((__m128i *)(mk + 16));
		SET_256ROUND_KEY_TYPE2(key->rk + 4, t2, delta[0][4], delta[0][5], delta[1][1], delta[1][2], rot3);
		SET_256ROUND_KEY_TYPE1(key->rk + 12, t2, delta[2] + 2, rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 20, t2, delta[3] + 5, rot2);

		SET_256ROUND_KEY_TYPE2(key->rk + 28, t2, delta[4][8], delta[4][9], delta[5][5], delta[5][6], rot3);
		SET_256ROUND_KEY_TYPE1(key->rk + 36, t2, delta[6] + 6, rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 44, t2, delta[7] + 9, rot2);

		SET_256ROUND_KEY_TYPE2(key->rk + 52, t2, delta[0][12], delta[0][13], delta[1][9], delta[1][10], rot3);
		SET_256ROUND_KEY_TYPE1(key->rk + 60, t2, delta[2] + 10, rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 68, t2, delta[3] + 13, rot2);

		SET_256ROUND_KEY_TYPE2(key->rk + 76, t2, delta[4][16], delta[4][17], delta[5][13], delta[5][14], rot3);
		SET_256ROUND_KEY_TYPE1(key->rk + 84, t2, delta[6] + 14, rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 92, t2, delta[7] + 17, rot2);

		SET_256ROUND_KEY_TYPE2(key->rk + 100, t2, delta[0][20], delta[0][21], delta[1][17], delta[1][18], rot3);
		SET_256ROUND_KEY_TYPE1(key->rk + 108, t2, delta[2] + 18, rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 116, t2, delta[3] + 21, rot2);

		SET_256ROUND_KEY_TYPE2(key->rk + 124, t2, delta[4][24], delta[4][25], delta[5][21], delta[5][22], rot3);
		SET_256ROUND_KEY_TYPE1(key->rk + 132, t2, delta[6] + 22, rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 140, t2, delta[7] + 25, rot2);

		SET_256ROUND_KEY_TYPE2(key->rk + 148, t2, delta[0][28], delta[0][29], delta[1][25], delta[1][26], rot3);
		SET_256ROUND_KEY_TYPE1(key->rk + 156, t2, delta[2] + 26, rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 164, t2, delta[3] + 29, rot2);

		SET_256ROUND_KEY_TYPE2(key->rk + 172, t2, delta[4][32], delta[4][33], delta[5][29], delta[5][30], rot3);
		SET_256ROUND_KEY_TYPE1(key->rk + 180, t2, delta[6] + 30, rot1);
		SET_256ROUND_KEY_TYPE1(key->rk + 188, t2, delta[7] + 1, rot2);

		break;
	}
}

#endif
