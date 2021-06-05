#include "lea_benchmark.h"
#include "src/lea.h"
#include "src/lea_locl.h"
#include "benchmark.h"

#include <stdio.h>
#include <stdlib.h>





void lea_key_benchmark()
{
	unsigned int calibration, tMin = 0xFFFFFFFF, t0, t1;
	int num_samples = 3, i, j = TEST_BLOCK_SIZE;
	unsigned int key_len, sample_count;
	unsigned char mk[32] = {	
		0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78, 
		0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0,
		0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 
		0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f 
	};
	unsigned char src1[16] = {0, }, src2[16] = {0, };
	LEA_KEY lea_key;

	//	Initialization for test
	printf("***** %s *****\n", __FUNCTION__);

	calibration = calibrate();

	//	Encryption Key schedule test
	printf("- Encryption Key schedule Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		tMin = 0xFFFFFFFF;
		for(sample_count = 0; sample_count < TEST_KEY_SAMPLE_COUNT; sample_count++)
		{
			for (i = 0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_set_key(&lea_key, mk, key_len >> 3);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}
		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin, key_len / 8));
	}

	for(i = 0; i < 16; i++)
	{
		lea_ecb_enc(src2, src1, 16, &lea_key);
		lea_ecb_enc(src1, src2, 16, &lea_key);
	}

	//	Decryption Key schedule test
	printf("- Decryption Key schedule Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		for(sample_count = 0; sample_count < TEST_KEY_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_set_key(&lea_key, mk, key_len >> 3);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}
		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin, key_len >> 3));
	}

	for(i = 0; i < 16; i++)
	{
		lea_ecb_enc(src2, src1, 16, &lea_key);
		lea_ecb_enc(src1, src2, 16, &lea_key);
	}

	printf("\n");
}


void lea_ecb_benchmark()
{
	unsigned int calibration, tMin = 0xFFFFFFFF, t0, t1;
	int num_samples = 3, i, j = TEST_BLOCK_SIZE;
	int key_len, sample_count;
	float current_cycles_byte = 0;
	unsigned char mk[32] = {	
		0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78, 
		0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0,
		0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 
		0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f 
	};
	unsigned char *src1, *src2, *src3;
	LEA_KEY lea_key;

	//	Initialization for test
	printf("***** %s *****\n", __FUNCTION__);
	src1 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src2 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src3 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);

	calibration = calibrate();

	//	Encryption Key schedule test
	printf("- Encryption Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		lea_set_key(&lea_key, mk, key_len >> 3);

		for(sample_count = 0; sample_count < TEST_ENC_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_ecb_enc(src2, src1, TEST_BLOCK_SIZE, &lea_key);
				lea_ecb_enc(src1, src2, TEST_BLOCK_SIZE, &lea_key);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}
		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin >> 1, TEST_BLOCK_SIZE));
	}

	//	Decryption Key schedule test
	printf("- Decryption Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		lea_set_key(&lea_key, mk, key_len >> 3);

		for(sample_count = 0; sample_count < TEST_ENC_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_ecb_dec(src2, src1, TEST_BLOCK_SIZE, &lea_key);
				lea_ecb_dec(src1, src2, TEST_BLOCK_SIZE, &lea_key);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}

		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin >> 1, TEST_BLOCK_SIZE));
	}

	free(src1);
	free(src2);
	free(src3);

	printf("\n");
}

void lea_cbc_benchmark()
{
	unsigned int calibration, tMin = 0xFFFFFFFF, t0, t1;
	int num_samples = 3, i, j = TEST_BLOCK_SIZE;
	int key_len, sample_count;
	float current_cycles_byte = 0;
	unsigned char mk[32] = {	
		0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78, 
		0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0,
		0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 
		0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f 
	};
	unsigned char *src1, *src2, *src3;
	unsigned char iv[16] = {0, };
	LEA_KEY lea_key;

	//	Initialization for test
	printf("***** %s *****\n", __FUNCTION__);
	src1 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src2 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src3 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);

	calibration = calibrate();

	//	Encryption Key schedule test
	printf("- Encryption Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		lea_set_key(&lea_key, mk, key_len >> 3);

		for(sample_count = 0; sample_count < TEST_ENC_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_cbc_enc(src2, src1, TEST_BLOCK_SIZE, iv, &lea_key);
				lea_cbc_enc(src1, src2, TEST_BLOCK_SIZE, iv, &lea_key);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}
		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin >> 1, TEST_BLOCK_SIZE));
	}

	//	Decryption Key schedule test
	printf("- Decryption Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		lea_set_key(&lea_key, mk, key_len >> 3);

		for(sample_count = 0; sample_count < TEST_ENC_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_cbc_dec(src2, src1, TEST_BLOCK_SIZE, iv, &lea_key);
				lea_cbc_dec(src1, src2, TEST_BLOCK_SIZE, iv, &lea_key);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}

		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin >> 1, TEST_BLOCK_SIZE));
	}

	free(src1);
	free(src2);
	free(src3);

	printf("\n");
}

void lea_ctr_benchmark()
{
	unsigned int calibration, tMin = 0xFFFFFFFF, t0, t1;
	int num_samples = 3, i, j = TEST_BLOCK_SIZE;
	int key_len, sample_count;
	float current_cycles_byte = 0;
	unsigned char mk[32] = {	
		0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78, 
		0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0,
		0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 
		0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f 
	};
	unsigned char *src1, *src2, *src3;
	unsigned char iv[16] = {0, };
	LEA_KEY lea_key;

	//	Initialization for test
	printf("***** %s *****\n", __FUNCTION__);
	src1 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src2 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src3 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);

	calibration = calibrate();

	//	Encryption Key schedule test
	printf("- Encryption Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		lea_set_key(&lea_key, mk, key_len >> 3);

		for(sample_count = 0; sample_count < TEST_ENC_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_ctr_enc(src2, src1, TEST_BLOCK_SIZE, iv, &lea_key);
				lea_ctr_enc(src1, src2, TEST_BLOCK_SIZE, iv, &lea_key);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}
		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin >> 1, TEST_BLOCK_SIZE));
	}

	//	Decryption Key schedule test
	printf("- Decryption Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		lea_set_key(&lea_key, mk, key_len >> 3);

		for(sample_count = 0; sample_count < TEST_ENC_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_ctr_dec(src2, src1, TEST_BLOCK_SIZE, iv, &lea_key);
				lea_ctr_dec(src1, src2, TEST_BLOCK_SIZE, iv, &lea_key);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}

		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin >> 1, TEST_BLOCK_SIZE));
	}

	free(src1);
	free(src2);
	free(src3);

	printf("\n");
}

void lea_ofb_benchmark()
{
	unsigned int calibration, tMin = 0xFFFFFFFF, t0, t1;
	int num_samples = 3, i, j = TEST_BLOCK_SIZE;
	int key_len, sample_count;
	float current_cycles_byte = 0;
	unsigned char mk[32] = {	
		0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78, 
		0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0,
		0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 
		0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f 
	};
	unsigned char *src1, *src2, *src3;
	unsigned char iv[16] = {0, };
	LEA_KEY lea_key;

	//	Initialization for test
	printf("***** %s *****\n", __FUNCTION__);
	src1 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src2 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src3 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);

	calibration = calibrate();

	//	Encryption Key schedule test
	printf("- Encryption Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		lea_set_key(&lea_key, mk, key_len >> 3);

		for(sample_count = 0; sample_count < TEST_ENC_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_ofb_enc(src2, src1, TEST_BLOCK_SIZE, iv, &lea_key);
				lea_ofb_enc(src1, src2, TEST_BLOCK_SIZE, iv, &lea_key);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}
		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin >> 1, TEST_BLOCK_SIZE));
	}

	//	Decryption Key schedule test
	printf("- Decryption Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		lea_set_key(&lea_key, mk, key_len >> 3);

		for(sample_count = 0; sample_count < TEST_ENC_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_ofb_dec(src2, src1, TEST_BLOCK_SIZE, iv, &lea_key);
				lea_ofb_dec(src1, src2, TEST_BLOCK_SIZE, iv, &lea_key);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}

		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin >> 1, TEST_BLOCK_SIZE));
	}

	free(src1);
	free(src2);
	free(src3);

	printf("\n");
}

void lea_cfb_benchmark()
{
	unsigned int calibration, tMin = 0xFFFFFFFF, t0, t1;
	int num_samples = 3, i, j = TEST_BLOCK_SIZE;
	int key_len, sample_count;
	float current_cycles_byte = 0;
	unsigned char mk[32] = {	
		0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78, 
		0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0,
		0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 
		0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f 
	};
	unsigned char *src1, *src2, *src3;
	unsigned char iv[16] = {0, };
	LEA_KEY lea_key;

	//	Initialization for test
	printf("***** %s *****\n", __FUNCTION__);
	src1 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src2 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src3 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);

	calibration = calibrate();

	//	Encryption Key schedule test
	printf("- Encryption Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		lea_set_key(&lea_key, mk, key_len >> 3);

		for(sample_count = 0; sample_count < TEST_ENC_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_cfb128_enc(src2, src1, TEST_BLOCK_SIZE, iv, &lea_key);
				lea_cfb128_enc(src1, src2, TEST_BLOCK_SIZE, iv, &lea_key);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}
		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin >> 1, TEST_BLOCK_SIZE));
	}

	//	Decryption Key schedule test
	printf("- Decryption Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		lea_set_key(&lea_key, mk, key_len >> 3);

		for(sample_count = 0; sample_count < TEST_ENC_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_cfb128_dec(src2, src1, TEST_BLOCK_SIZE, iv, &lea_key);
				lea_cfb128_dec(src1, src2, TEST_BLOCK_SIZE, iv, &lea_key);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}

		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin >> 1, TEST_BLOCK_SIZE));
	}

	free(src1);
	free(src2);
	free(src3);

	printf("\n");
}

void lea_cmac_benchmark()
{
	unsigned int calibration, tMin = 0xFFFFFFFF, t0, t1;
	int num_samples = 3, i, j = TEST_BLOCK_SIZE;
	int key_len, sample_count;
	float current_cycles_byte = 0;
	unsigned char mk[32] = {	
		0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78, 
		0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0,
		0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 
		0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f 
	};
	unsigned char *src1, *src2, *src3;
	LEA_CMAC_CTX cmac_ctx;

	//	Initialization for test
	printf("***** %s *****\n", __FUNCTION__);
	src1 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src2 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src3 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	
	calibration = calibrate();

	//	Encryption Key schedule test
	printf("- Encryption Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		lea_cmac_init(&cmac_ctx, mk, key_len >> 3);

		for(sample_count = 0; sample_count < TEST_ENC_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_cmac_update(&cmac_ctx, src1, TEST_BLOCK_SIZE);
				lea_cmac_final(&cmac_ctx, src2, 16);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}
		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin, TEST_BLOCK_SIZE));
	}
	
	free(src1);
	free(src2);
	free(src3);

	printf("\n");
}

void lea_ccm_benchmark()
{
	unsigned int calibration, tMin = 0xFFFFFFFF, t0, t1;
	int num_samples = 3, i, j = TEST_BLOCK_SIZE;
	int key_len, sample_count;
	float current_cycles_byte = 0;
	unsigned char mk[32] = {	
		0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78, 
		0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0,
		0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 
		0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f 
	};
	unsigned char nonce[7] = { 0, 0, 0, 0, 0, 0, 0 };
	unsigned char *src1, *src2, *src3;
	LEA_KEY lea_key;

	//	Initialization for test
	printf("***** %s *****\n", __FUNCTION__);
	src1 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src2 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src3 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);

	calibration = calibrate();

	//	Encryption Key schedule test
	printf("- Encryption Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		lea_set_key(&lea_key, mk, key_len >> 3);

		for(sample_count = 0; sample_count < TEST_ENC_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_ccm_enc(src2, src3, src1, TEST_BLOCK_SIZE, 16, nonce, 7, NULL, 0, &lea_key);
				lea_ccm_enc(src1, src3, src2, TEST_BLOCK_SIZE, 16, nonce, 7, NULL, 0, &lea_key);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}
		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin >> 1, TEST_BLOCK_SIZE));
	}

	//	Decryption Key schedule test
	printf("- Decryption Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		lea_set_key(&lea_key, mk, key_len >> 3);

		for(sample_count = 0; sample_count < TEST_ENC_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_ccm_dec(src1, src2, TEST_BLOCK_SIZE, src3, 16, nonce, 7, NULL, 0, &lea_key);
				lea_ccm_dec(src2, src1, TEST_BLOCK_SIZE, src3, 16, nonce, 7, NULL, 0, &lea_key);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}

		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin >> 1, TEST_BLOCK_SIZE));
	}

	free(src1);
	free(src2);
	free(src3);

	printf("\n");
}

void lea_gcm_benchmark()
{
	unsigned int calibration, tMin = 0xFFFFFFFF, t0, t1;
	int num_samples = 3, i, j = TEST_BLOCK_SIZE;
	int key_len, sample_count;
	float current_cycles_byte = 0;
	unsigned char mk[32] = {	
		0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78, 
		0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0,
		0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 
		0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f 
	};
	unsigned char *src1, *src2, *src3;
	LEA_GCM_CTX gcm_ctx;

	//	Initialization for test
	printf("***** %s *****\n", __FUNCTION__);
	src1 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src2 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);
	src3 = (unsigned char *)calloc(TEST_BLOCK_SIZE, 1);

	calibration = calibrate();

	//	Encryption Key schedule test
	printf("- Encryption Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		lea_gcm_init(&gcm_ctx, mk, key_len >> 3);

		for(sample_count = 0; sample_count < TEST_ENC_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_gcm_enc(&gcm_ctx, src2, src1, TEST_BLOCK_SIZE);
				lea_gcm_final(&gcm_ctx, src3, 16);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}
		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin, TEST_BLOCK_SIZE));
	}

	//	Decryption Key schedule test
	printf("- Decryption Speed\n");
	for(key_len = 128; key_len <= 256; key_len += 64)
	{
		lea_gcm_init(&gcm_ctx, mk, key_len >> 3);

		for(sample_count = 0; sample_count < TEST_ENC_SAMPLE_COUNT; sample_count++)
		{
			tMin = 0xFFFFFFFF;

			for (i=0; i<TIMER_SAMPLE_CNT;i++)  /* calibrate the overhead for measuring time */
			{
				t0 = HiResTime();

				/*	function for test	*/
				lea_gcm_dec(&gcm_ctx, src2, src1, TEST_BLOCK_SIZE);
				lea_gcm_final(&gcm_ctx, src3, 16);
				/*	function for test	*/

				t1 = HiResTime();

				if (tMin > t1 - t0 - calibration)       /* keep only the minimum time */
					tMin = t1 - t0 - calibration;
			}

		}
		printf("[%d] %7.2f cycles/byte\n", key_len, get_cpb(tMin, TEST_BLOCK_SIZE));
	}

	free(src1);
	free(src2);
	free(src3);

	printf("\n");
}

