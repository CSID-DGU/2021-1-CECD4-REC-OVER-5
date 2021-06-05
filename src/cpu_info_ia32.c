#include "lea_locl.h"
#include "cpu_info.h"


#include <string.h>

#ifndef NO_SIMD
#if defined(__i386__) || defined(_M_IX86) || defined(_M_X64) || defined(__x86_64__)

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <intrin.h>
#endif

#ifdef _MSC_VER
#if _MSC_VER >= 1600
#define cpuid(info,x)	__cpuidex(info,x,0)
#else
#define cpuid(info,x)	__cpuid(info,x)
#endif
int check_xcr0_ymm(){
	#if _MSC_FULL_VER >= 160040219
	unsigned int xcr0 = (unsigned int)_xgetbv(0);
	return xcr0;
	#else
	return 0;
	#endif
}

#else

void cpuid(int CPUInfo[4], int InfoType){
	unsigned int eax=InfoType, ebx=0, ecx=0, edx=0;
#if defined(__i386__) && defined(__PIC__)
	__asm__("movl %%ebx, %%edi \n\t cpuid \n\t xchgl %%ebx, %%edi" : "=D" (ebx),
#else
	__asm__("cpuid" : "+b"(ebx),
#endif
	"+a" (eax),
	"+c" (ecx),
	"=d" (edx));
	CPUInfo[0] = eax;
	CPUInfo[1] = ebx;
	CPUInfo[2] = ecx;
	CPUInfo[3] = edx;
}

int check_xcr0_ymm(){
	unsigned int xcr0;
	__asm__("xgetbv" : "=a" (xcr0) : "c" (0) : "%edx");
	return xcr0;
}
#endif

unsigned char isX64(){
	#if defined(_M_X64) || defined(__x86_64__)
	return 1;
	#else
	return 0;
	#endif
}

unsigned char support_os_avx(){
	int info[4];
	unsigned char avxSupport = 0;
	unsigned char osUsesXSAVE_XRSTORE;
	unsigned char cpuAVXSupport;
	
	cpuid(info,0x1);
	osUsesXSAVE_XRSTORE = (info[2] & ((int)1 << 27)) != 0;
	cpuAVXSupport = (info[2] & ((int)1 << 28)) != 0;
	
	if(osUsesXSAVE_XRSTORE && cpuAVXSupport)
	{
		avxSupport = (check_xcr0_ymm() & 6) == 6;
	}
	
	return avxSupport;
}

unsigned char support_os_avx512(){
	if(!support_os_avx()){
		return 0;
	}
	return (check_xcr0_ymm() & 0xe6) == 0xe6;
}

void get_ia32_cpuinfo(info_ia32* pInfo, unsigned char check_os_support){
	int info[4];
	int nIds;
	unsigned int nExIds;
	
	if(pInfo == NULL){
		return;
	}
	
	memset(pInfo, 0, sizeof(info_ia32));
	
	cpuid(info, 0);
	nIds = info[0];
	cpuid(info, 0x80000000);
	nExIds = info[0];
	
	if (nIds >= 0x00000001){
		cpuid(info,0x00000001);
		
		pInfo->mmx	 = (info[3] & ((int)1 << 23)) != 0;
		pInfo->sse	 = (info[3] & ((int)1 << 25)) != 0;
		pInfo->sse2	 = (info[3] & ((int)1 << 26)) != 0;
		pInfo->sse3	 = (info[2] & ((int)1 <<  0)) != 0;
			
		pInfo->pclmul= (info[2] & ((int)1 <<  1)) != 0;
		pInfo->ssse3 = (info[2] & ((int)1 <<  9)) != 0;
		pInfo->sse41 = (info[2] & ((int)1 << 19)) != 0;
		pInfo->sse42 = (info[2] & ((int)1 << 20)) != 0;
		pInfo->aes	 = (info[2] & ((int)1 << 25)) != 0;
			
		pInfo->avx	 = (info[2] & ((int)1 << 28)) != 0;
		pInfo->fma3	 = (info[2] & ((int)1 << 12)) != 0;
			
		pInfo->rdrand= (info[2] & ((int)1 << 30)) != 0;
	}
	if (nIds >= 0x00000007){
		cpuid(info,0x00000007);
		
		pInfo->avx2	 		= (info[1] & ((int)1 <<  5)) != 0;
			
		pInfo->bmi1			= (info[1] & ((int)1 <<  3)) != 0;
		pInfo->bmi2			= (info[1] & ((int)1 <<  8)) != 0;
		pInfo->adx		 	= (info[1] & ((int)1 << 19)) != 0;
		pInfo->sha			= (info[1] & ((int)1 << 29)) != 0;
		pInfo->prefetchwt1	= (info[2] & ((int)1 <<  0)) != 0;
			
		pInfo->avx512f	 	= (info[1] & ((int)1 << 16)) != 0;
		pInfo->avx512cd	 	= (info[1] & ((int)1 << 28)) != 0;
		pInfo->avx512pf	 	= (info[1] & ((int)1 << 26)) != 0;
		pInfo->avx512er	 	= (info[1] & ((int)1 << 27)) != 0;
		pInfo->avx512vl	 	= (info[1] & ((int)1 << 31)) != 0;
		pInfo->avx512bw	 	= (info[1] & ((int)1 << 30)) != 0;
		pInfo->avx512dq	 	= (info[1] & ((int)1 << 17)) != 0;
		pInfo->avx512ifma	= (info[1] & ((int)1 << 21)) != 0;
		pInfo->avx512vbmi	= (info[2] & ((int)1 <<  1)) != 0;
	}
	if (nExIds >= 0x80000001){
		cpuid(info,0x80000001);
		
		pInfo->x64	 = (info[3] & ((int)1 << 29)) != 0;
		pInfo->abm	 = (info[2] & ((int)1 <<  5)) != 0;
		pInfo->sse4a = (info[2] & ((int)1 <<  6)) != 0;
		pInfo->fma4	 = (info[2] & ((int)1 << 16)) != 0;
		pInfo->xop	 = (info[2] & ((int)1 << 11)) != 0;
	}
	
	if(check_os_support){
		pInfo->x64 &= isX64();
		
		if(!support_os_avx()){
			pInfo->avx=0;
			pInfo->avx2=0;
		}
		
		if(!support_os_avx512()){
			pInfo->avx512f = 0;
			pInfo->avx512cd = 0;
			pInfo->avx512pf = 0;
			pInfo->avx512er= 0;
			pInfo->avx512vl = 0;
			pInfo->avx512bw = 0;
			pInfo->avx512dq = 0;
			pInfo->avx512ifma = 0;
			pInfo->avx512vbmi = 0;
		}
	}
}

#endif
#endif /* NO_SIMD */
