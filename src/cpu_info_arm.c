/* from OPENSSL 1.0.2 */
#include "lea_locl.h"
#include "arm_arch.h"
#ifdef ARCH_ARM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include "cpu_info.h"


#if __ARM_MAX_ARCH__<7
void get_arm_cpuinfo(info_arm* pInfo)
{
	if(pInfo == NULL){
		return;
	}
	lea_memset(pInfo, 0, sizeof(info_arm));
}
/*
unsigned long arm_rdtsc(void)
{
    return 0;
}
*/
#else

static sigset_t all_masked;

static sigjmp_buf ill_jmp;
static void ill_handler(int sig)
{
    siglongjmp(ill_jmp, sig);
}


void _armv7_neon_probe(void);
void _armv8_aes_probe(void);
void _armv8_sha1_probe(void);
void _armv8_sha256_probe(void);
void _armv8_pmull_probe(void);
//unsigned long _armv7_tick(void);

# if defined(__GNUC__) && __GNUC__>=2
void get_arm_cpuinfo(info_arm*) __attribute__ ((constructor));
extern unsigned long getauxval(unsigned long type) __attribute__ ((weak));
# else
static unsigned long (*getauxval) (unsigned long) = NULL;
# endif

# if defined(__arm__) || defined (__arm)
#  define HWCAP                  16
                                  /* AT_HWCAP */
#  define HWCAP_NEON             (1 << 12)

#  define HWCAP_CE               26
                                  /* AT_HWCAP2 */
#  define HWCAP_CE_AES           (1 << 0)
#  define HWCAP_CE_PMULL         (1 << 1)
#  define HWCAP_CE_SHA1          (1 << 2)
#  define HWCAP_CE_SHA256        (1 << 3)
# elif defined(__aarch64__)
#  define HWCAP                  16
                                  /* AT_HWCAP */
#  define HWCAP_NEON             (1 << 1)

#  define HWCAP_CE               HWCAP
#  define HWCAP_CE_AES           (1 << 3)
#  define HWCAP_CE_PMULL         (1 << 4)
#  define HWCAP_CE_SHA1          (1 << 5)
#  define HWCAP_CE_SHA256        (1 << 6)
# endif

void get_arm_cpuinfo(info_arm* pInfo)
{
    char *e;
    struct sigaction ill_oact, ill_act;
    sigset_t oset;
    static int trigger = 0;

    if (trigger)
        return;
    trigger = 1;

    sigfillset(&all_masked);
    sigdelset(&all_masked, SIGILL);
    sigdelset(&all_masked, SIGTRAP);
    sigdelset(&all_masked, SIGFPE);
    sigdelset(&all_masked, SIGBUS);
    sigdelset(&all_masked, SIGSEGV);

    lea_memset(pInfo,0,sizeof(info_arm));

    lea_memset(&ill_act, 0, sizeof(ill_act));
    ill_act.sa_handler = ill_handler;
    ill_act.sa_mask = all_masked;

    sigprocmask(SIG_SETMASK, &ill_act.sa_mask, &oset);
    sigaction(SIGILL, &ill_act, &ill_oact);

    if (getauxval != NULL) {
        if (getauxval(HWCAP) & HWCAP_NEON) {
            unsigned long hwcap = getauxval(HWCAP_CE);

            pInfo->neon=1;

            if (hwcap & HWCAP_CE_AES)
            	pInfo->aes=1;

            if (hwcap & HWCAP_CE_PMULL)
            	pInfo->pmull=1;

            if (hwcap & HWCAP_CE_SHA1)
            	pInfo->sha1=1;

            if (hwcap & HWCAP_CE_SHA256)
            	pInfo->sha256=1;
        }
    } else if (sigsetjmp(ill_jmp, 1) == 0) {
        _armv7_neon_probe();
        pInfo->neon = 1;
        if (sigsetjmp(ill_jmp, 1) == 0) {
            _armv8_pmull_probe();
            pInfo->pmull = 1;
            pInfo->aes = 1;
        } else if (sigsetjmp(ill_jmp, 1) == 0) {
            _armv8_aes_probe();
            pInfo->aes = 1;
        }
        if (sigsetjmp(ill_jmp, 1) == 0) {
            _armv8_sha1_probe();
            pInfo->sha1 = 1;
        }
        if (sigsetjmp(ill_jmp, 1) == 0) {
            _armv8_sha256_probe();
            pInfo->sha256 = 1;
        }
    }
    /*
    if (sigsetjmp(ill_jmp, 1) == 0) {
        _armv7_tick();
        pInfo->tick = 1;
    }
    */

    sigaction(SIGILL, &ill_oact, NULL);
    sigprocmask(SIG_SETMASK, &oset, NULL);
}
#endif
#endif
