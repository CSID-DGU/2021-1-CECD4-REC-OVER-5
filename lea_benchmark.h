#ifndef _LEA_BENCHMARK_H_
#define _LEA_BENCHMARK_H_



#define TEST_KEY_BYTE			16
#define TEST_BLOCK_SIZE			10000 * 16
#define TEST_KEY_SAMPLE_COUNT	10000
#define TEST_ENC_SAMPLE_COUNT	10



void lea_key_benchmark();
void lea_ecb_benchmark();
void lea_cbc_benchmark();
void lea_ctr_benchmark();
void lea_ofb_benchmark();
void lea_cfb_benchmark();
void lea_cmac_benchmark();
void lea_ccm_benchmark();
void lea_gcm_benchmark();






#endif
