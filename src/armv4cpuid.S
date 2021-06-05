@from OPENSSL 1.0.2
#include "arm_arch.h"

.text
.code	32

#if __ARM_MAX_ARCH__>=7
.arch	armv7-a
.fpu	neon

.align	5
.global	_armv7_neon_probe
.type	_armv7_neon_probe,%function
_armv7_neon_probe:
	vorr	q0,q0,q0
	bx	lr
.size	_armv7_neon_probe,.-_armv7_neon_probe

.global	_armv7_tick
.type	_armv7_tick,%function
_armv7_tick:
	mrrc	p15,1,r0,r1,c14		@ CNTVCT
	bx	lr
.size	_armv7_tick,.-_armv7_tick

.global	_armv8_aes_probe
.type	_armv8_aes_probe,%function
_armv8_aes_probe:
	.byte	0x00,0x03,0xb0,0xf3	@ aese.8	q0,q0
	bx	lr
.size	_armv8_aes_probe,.-_armv8_aes_probe

.global	_armv8_sha1_probe
.type	_armv8_sha1_probe,%function
_armv8_sha1_probe:
	.byte	0x40,0x0c,0x00,0xf2	@ sha1c.32	q0,q0,q0
	bx	lr
.size	_armv8_sha1_probe,.-_armv8_sha1_probe

.global	_armv8_sha256_probe
.type	_armv8_sha256_probe,%function
_armv8_sha256_probe:
	.byte	0x40,0x0c,0x00,0xf3	@ sha256h.32	q0,q0,q0
	bx	lr
.size	_armv8_sha256_probe,.-_armv8_sha256_probe
.global	_armv8_pmull_probe
.type	_armv8_pmull_probe,%function
_armv8_pmull_probe:
	.byte	0x00,0x0e,0xa0,0xf2	@ vmull.p64	q0,d0,d0
	bx	lr
.size	_armv8_pmull_probe,.-_armv8_pmull_probe
#endif
