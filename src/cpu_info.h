#pragma once
#ifndef _UTILS_CPU_INFO_H
#define _UTILS_CPU_INFO_H

typedef struct {
	unsigned char mmx;
	unsigned char sse;
	unsigned char sse2;
	unsigned char sse3;
	
	unsigned char pclmul;
	unsigned char ssse3;
	unsigned char sse41;
	unsigned char sse42;
	unsigned char aes;
	
	unsigned char avx;
	unsigned char fma3;
	
	unsigned char rdrand;
	
	unsigned char avx2;
	
	unsigned char bmi1;
	unsigned char bmi2;
	unsigned char adx;
	unsigned char sha;
	unsigned char prefetchwt1;
	
	unsigned char avx512f;
	unsigned char avx512cd;
	unsigned char avx512pf;
	unsigned char avx512er;
	unsigned char avx512vl;
	unsigned char avx512bw;
	unsigned char avx512dq;
	unsigned char avx512ifma;
	unsigned char avx512vbmi;
	
	unsigned char x64;
	unsigned char abm;
	unsigned char sse4a;
	unsigned char fma4;
	unsigned char xop;
} info_ia32;

typedef struct{
	unsigned char neon;
	
	unsigned char aes;
	unsigned char sha1;
	unsigned char sha256;
	unsigned char pmull;
} info_arm;

void get_ia32_cpuinfo(info_ia32* pInfo, unsigned char check_os_support);
void get_arm_cpuinfo(info_arm* pInfo);
#endif
