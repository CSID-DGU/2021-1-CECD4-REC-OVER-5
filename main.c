#define _CRT_SECURE_NO_WARNINGS
#include "src/lea.h"
#include "src/lea_locl.h"

#include "benchmark.h"
#include "lea_benchmark.h"
#include "lea_vs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <iostream>

int main(void)
{
    FILE* file;

    LEA_KEY key;


    int ch;
    int size;
    unsigned char* buffer;
    unsigned char* output;


    //바이너리 데이터 읽기 전용으로 열기
    if ((file = fopen("16.txt", "rb")) == NULL) {
        fputs("파일 열기 에러!", stderr);
        exit(1);
    }

    //파일 사이즈(바이트) 구하기
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    printf("size: %d \n", size);
    fseek(file, 0, SEEK_SET);


    //사이즈만큼 동적할당
    buffer = (unsigned char*)malloc(size);
    memset(buffer, 0, size);

    output = (unsigned char*)malloc(size);
    memset(output, 0, size);


    //이진파일 읽어서 버퍼에 저장
    fread(buffer, size, 1, file);
    for (int i = 0; i < size; i++) {
        printf("%02X ", *buffer++);
    }
    lea_ecb_enc(output, buffer, 32, &key);
    printf("\noutput : ");
    for (int i = 0; i < size; i++)
        printf("%02X ", *output++);
    
    fclose(file);

	//int ret;
	//printf("SIMD: %s\n", get_simd_type());
	//if(ret = lea_mmt_ecb_test())	printf("LEA ECB FAIL(%d)\n", ret);
	//if(ret = lea_mmt_cbc_test())	printf("LEA CBC FAIL(%d)\n", ret);
	//if(ret = lea_mmt_ctr_test())	printf("LEA CTR FAIL(%d)\n", ret);
	//if(ret = lea_mmt_ofb_test())	printf("LEA OFB FAIL(%d)\n", ret);
	//if(ret = lea_mmt_cfb_test())	printf("LEA CFB FAIL(%d)\n", ret);
	//if(ret = lea_cmac_g_test())		printf("LEA CMAC FAIL(%d)\n", ret);
	//if(ret = lea_ccm_ge_test())		printf("LEA CCM FAIL(%d)\n", ret);
	//if(ret = lea_gcm_ae_test())		printf("LEA GCM FAIL(%d)\n", ret);
	//
	//lea_key_benchmark();
	//lea_ecb_benchmark();
	//lea_cbc_benchmark();
	//lea_ctr_benchmark();
	//lea_ofb_benchmark();
	//lea_cfb_benchmark();
	//lea_cmac_benchmark();
	//lea_ccm_benchmark();
	//lea_gcm_benchmark();
	//
	return 0;
}
