#define _CRT_SECURE_NO_WARNINGS
#include "src/lea.h"
#include "src/lea_locl.h"

#include "benchmark.h"
#include "lea_benchmark.h"
#include "lea_vs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int gcd(int a, int b);
int RSA(int n, int m, int d);

int main(void)
{
    FILE* file;

    LEA_KEY key;

    FILE* fp = fopen("encrypted.rcv", "w");

    int ch;
    int size;
    unsigned char* buffer;
    unsigned char* output;
    unsigned char* result;

    int prime1 = 13;
    int prime2 = 29;

    int e, N; //public key
    int d; //private key
    
    N = prime1 * prime2;
    int f = (prime1 - 1) * (prime2 - 1);
    
    for(int i = 2; i < f; i++)
        if (gcd(prime1 - 1, i) == 1 && gcd(prime2 - 1, i) == 1) {
            e = i;
            break;
        }

    for(int i = 1; i < N; i++)
        if ((1 + f * i) % e == 0) {
            d = (1 + f * i) / e;
            break;
        }

    //���̳ʸ� ������ �б� �������� ����
    if ((file = fopen("16.txt", "rb")) == NULL) {
        fputs("���� ���� ����!", stderr);
        exit(1);
    }

    //���� ������(����Ʈ) ���ϱ�
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    printf("size: %d \n", size);
    fseek(file, 0, SEEK_SET);


    //�����ŭ �����Ҵ�
    buffer = (unsigned char*)malloc(size);
    memset(buffer, 0, size);

    output = (unsigned char*)malloc(size);
    memset(output, 0, size);

    result = (unsigned char*)malloc(size);
    memset(result, 0, size);

    int change[32];
    int rsa[32];

    //�������� �о ���ۿ� ����
    fread(buffer, size, 1, file);
    for (int i = 0; i < size; i++) {
        printf("%02X ", *buffer++);
    }
    lea_ecb_enc(output, buffer, 32, &key);
    printf("\noutput : ");
    for (int i = 0; i < size; i++) {
        printf("%02X ", *output++);
    }
    printf("\nchange : ");
    for (int i = 0; i < size; i++) {
        change[i] = *output++;
        rsa[i] = RSA(N, change[i], e);
        printf("%d ", change[i]);
    }

    printf("\nRSA : ");
    for (int i = 0; i < size; i++) {
        printf("%d ", rsa[i]);
    }
    
    printf("\nResult : ");
    for (int i = 0; i < size; i++) {
        *result++ = rsa[i];
        printf("%c ", result[i]);
    }

    fputs(result, fp);

    fclose(fp);
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

int gcd(int a, int b) {
    int tmp, n;

    if (a < b) {
        tmp = a;
        a = b;
        b = tmp;
    }

    while (b != 0) {
        n = a % b;
        a = b;
        b = n;
    }

    return a;
}

int RSA(int n, int m, int d) {
    int bin1[10] = { 0, };
    int tmp, j = 1;
    unsigned long long x_tmp = 1;
    int x[10]; // m^2^0~9 mod n

    // �ŵ� ������ ������ ���� �� �̸� ����
    x[0] = (unsigned long long)pow(m, 1) % n;
    for (int i = 1; i < 10; i++) {
        x[i] = (x[i - 1] * x[i - 1]) % n;
    }

    // ������ �������� ��ȯ (���� ���Ǽ��� ���� �׳� ������ ����) 
    for (int i = 0; d > 0; i++) {
        tmp = d % 2;
        d /= 2;
        bin1[i] = tmp;
    }

    // �������� 1�� ���� �̸� ���� �� ����
    for (int i = 0; i < 10; i++) {
        if (bin1[i] == 1) {
            x_tmp *= x[i];
        }
    }

    return x_tmp % n; // ���� ���� �����ؼ� ��ȯ
}