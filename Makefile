CC=gcc
AS=as
AR=ar
RANLIB=ranlib
LN=ln
MV=mv

#auto detect architecture
TARGET_CPU=native
#TARGET_CPU=generic #non-SIMD


#make Shared Object
PREFIX?=/usr/local
SHARED=TRUE

CFLAGS=-O3
LDFLAGS=-Wl,-rpath '-Wl,$$ORIGIN' -L./

MAJOR_VERSION = 1
MINOR_VERSION = 0.1

ARMFLAGS?= -march=armv7-a -mfloat-abi=softfp -mfpu=neon

SIMD_AVX2?= -mavx2
SIMD_XOP?= -mxop
SIMD_PCLMUL?= -mssse3 -msse4.1 -mpclmul
SIMD_SSE2?= -msse2

ifeq ($(OS),Windows_NT)
IS_SO = FALSE
else
IS_SO = TRUE
endif

ifeq ($(SHARED),TRUE)
ifeq ($(IS_SO),TRUE)
SHARED_FLAGS = -fPIC
endif
endif

ifeq ($(IS_SO),TRUE)
SO_NAME = liblea.so.$(MAJOR_VERSION)
SO_REAL_NAME = $(SO_NAME).$(MINOR_VERSION)
else
SO_NAME = liblea.dll
SO_REAL_NAME = liblea.dll
endif

A_NAME = liblea.a

ifneq ($(TARGET_CPU),native)
#do nothing
else ifeq ($(OS),Windows_NT)
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		TARGET_CPU = x86_64
	else 
	ifeq ($(PROCESSOR_ARCHITECTURE),x86_64)
		TARGET_CPU = x86_64
	else 
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		TARGET_CPU = x86
	endif #not support ARM on Windows.. :P
	endif
	endif
else
	#Let's test cc
	CCMACHINE := $(shell $(CC) -dumpmachine)
	ifneq ($(CCMACHINE),)
		ifneq ($(filter x86_64%, $(CCMACHINE)),)
			TARGET_CPU = x86_64
		else 
		ifneq ($(filter AMD64%, $(CCMACHINE)),)
			TARGET_CPU = x86_64
		else 
		ifneq (,$(findstring 86, $(CCMACHINE)))
			TARGET_CPU = x86
		else 
		ifneq ($(filter armv8%, $(CCMACHINE)),)
			TARGET_CPU = arm64
		else 
		ifneq ($(filter arm%, $(CCMACHINE)),)
			TARGET_CPU = arm
		endif
		endif
		endif
		endif
		endif
	else
		UNAME_P := $(shell uname -p)
		ifeq ($(UNAME_P),x86_64)
			TARGET_CPU = x86_64
		else 
		ifeq ($(UNAME_P),AMD64)
			TARGET_CPU = x86_64
		else 
		ifneq ($(filter %86, $(UNAME_P)),)
			TARGET_CPU = x86
		else 
		ifneq ($(filter armv8%, $(UNAME_P)),)
			TARGET_CPU = arm64
		else 
		ifneq ($(filter arm%, $(UNAME_P)),)
			TARGET_CPU = arm
		endif
		endif
		endif
		endif
		endif
	endif
endif

.SUFFIXES:.c .o .S
.PHONY: all install clean objclean

LEA_COMMON =  src/lea_base.o src/lea_online.o src/lea_core.o src/lea_gcm_generic.o src/lea_t_fallback.o src/lea_t_generic.o
LEA_IA32 = src/cpu_info_ia32.o src/lea_t_avx2_sse2.o src/lea_t_avx2_xop.o src/lea_core_xop.o \
		src/lea_gcm_pclmul.o src/lea_t_sse2.o src/lea_t_xop.o
LEA_ARM = src/cpu_info_arm.o src/armv4cpuid.o
LEA_ARM64 = src/lea_t_neon.o src/cpu_info_arm.o src/arm64cpuid.o
TARGET_SIMD = 
ifeq ($(TARGET_CPU),x86)
	LEA_PLATFORM = $(LEA_IA32)
	TARGET_SIMD = 'x86 automatic(SSE2, XOP, AVX2, PCLMUL)'
	ARMFLAGS = 
else
ifeq ($(TARGET_CPU),x86_64)
	LEA_PLATFORM = $(LEA_IA32)
	TARGET_SIMD = 'x86_64 automatic(SSE2, XOP, AVX2, PCLMUL)'
	ARMFLAGS =
else
ifeq ($(TARGET_CPU),arm)
	LEA_PLATFORM = $(LEA_ARM)
	ifneq ($(shell $(CC) --target-help 3>&1 2>&1 1>&2 | grep neon),)
		LEA_PLATFORM := $(LEA_PLATFORM) src/lea_t_neon.o
		TARGET_SIMD := $(TARGET_SIMD) NEON
	endif
else
ifeq ($(TARGET_CPU),arm64)
	LEA_PLATFORM = $(LEA_ARM64)
else
#generic
	TARGET_SIMD = 'non-SIMD'
	LEA_PLATFORM = 
	ARMFLAGS =
	CFLAGS += -DNO_SIMD
endif
endif
endif
endif

TEST_OBJS = main.o lea_vs.o lea_benchmark.o benchmark.o


all : info lib lea_test lea_0tv

ifeq ($(SHARED),TRUE)
lib: $(SO_NAME) $(SO_REAL_NAME)
else
lib: $(A_NAME)
endif

install: lib
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	cp -P liblea.* $(PREFIX)/lib
	cp src/lea.h $(PREFIX)/include
	cp src/config.h $(PREFIX)/include
ifeq ($(IS_SO),TRUE)
	ldconfig
endif

info: 
	@echo '=LEA GCC makefile='
	@echo Current target : $(TARGET_CPU)
	@echo Current CC, AS, AR, RANLIB : $(CC), $(AS), $(AR), $(RANLIB)
	@echo Target SIMD : $(TARGET_SIMD)
	@echo CFLAGS : $(CFLAGS)
	@echo LDFLAGS : $(LDFLAGS)
ifeq ($(TARGET_CPU),arm)
	@echo ARMFLAGS : $(ARMFLAGS)
else
ifeq ($(TARGET_CPU),arm64)
	@echo ARMFLAGS : $(ARMFLAGS)
endif
endif
	@echo

help: info
	@echo =Examples=
	@echo '1. Just "make"'
	@echo '2. "make SHARED=FALSE" for static library
	@echo '3. "make CC=arm-linux-gnueabi-gcc AS=arm-linux-gnueabi-as AR=** RANLIB=** ARMFLAGS=**"'
objclean: 
	$(RM) *.o
	$(RM) src/*.o
clean: objclean
	$(RM) -f lea_test* lea_0tv* liblea.*

ifeq ($(IS_SO),TRUE)
$(SO_NAME): $(SO_REAL_NAME) liblea.so
	rm -f $(SO_NAME)
	$(LN) -s $(SO_REAL_NAME) $(SO_NAME)
liblea.so: $(SO_REAL_NAME)
	rm -f liblea.so
	$(LN) -s $(SO_REAL_NAME) liblea.so
endif

$(SO_REAL_NAME):$(LEA_COMMON) $(LEA_PLATFORM)
	$(CC) -shared -Wl,-soname,$(SO_NAME) -o $(SO_REAL_NAME) $^

$(A_NAME):$(LEA_COMMON) $(LEA_PLATFORM)
	$(AR) rc $@ $^
	$(RANLIB) $@

lea_test : $(TEST_OBJS)
	$(CC) -lm $(LDFLAGS)  $(ARMFLAGS) -o $@ $^ -llea
lea_0tv : main_0tv.o
	$(CC) -lm $(LDFLAGS)  $(ARMFLAGS) -o $@ $< -llea

src/arm64cpuid.o: src/arm64cpuid.S src/arm_arch.h
	$(CC) $(ARMFLAGS) -E $< | $(AS) -o $@
	
src/armv4cpuid.o: src/armv4cpuid.S src/arm_arch.h
	$(CC) $(ARMFLAGS) -E $< | $(AS) -o $@

src/lea_core_xop.o: src/lea_core_xop.c src/lea.h src/lea_locl.h src/config.h src/cpu_info.h
	$(CC) $(CFLAGS) $(SIMD_SSE2) $(SIMD_XOP) $(SHARED_FLAGS) -o $@ -c $<
src/lea_gcm_pclmul.o: src/lea_gcm_pclmul.c src/lea.h src/lea_locl.h src/config.h src/cpu_info.h
	$(CC) $(CFLAGS) $(SIMD_SSE2) $(SIMD_PCLMUL) $(SHARED_FLAGS) -o $@ -c $<
src/lea_t_avx2_sse2.o: src/lea_t_avx2_sse2.c src/lea_avx2.h src/lea_sse2.h src/lea_key.h src/lea_ecb.h src/lea_cbc.h \
	src/lea_ctr.h src/lea_cfb.h src/lea_ofb.h src/lea_cmac.h src/lea_ccm.h src/lea_gcm.h
	$(CC) $(CFLAGS) $(SIMD_SSE2) $(SIMD_AVX2) $(SHARED_FLAGS) -o $@ -c $<
src/lea_t_avx2_xop.o: src/lea_t_avx2_xop.c src/lea_avx2.h src/lea_xop.h src/lea_key.h src/lea_ecb.h src/lea_cbc.h \
	src/lea_ctr.h src/lea_cfb.h src/lea_ofb.h src/lea_cmac.h src/lea_ccm.h src/lea_gcm.h
	$(CC) $(CFLAGS) $(SIMD_SSE2) $(SIMD_XOP) $(SIMD_AVX2) $(SHARED_FLAGS) -o $@ -c $<
src/lea_t_neon.o: src/lea_t_neon.c src/lea_key.h src/lea_ecb.h src/lea_cbc.h \
	src/lea_ctr.h src/lea_cfb.h src/lea_ofb.h src/lea_cmac.h src/lea_ccm.h src/lea_gcm.h
	$(CC) $(CFLAGS) $(ARMFLAGS) $(SHARED_FLAGS) -o $@ -c $<
src/lea_t_sse2.o: src/lea_t_sse2.c src/lea_key.h src/lea_ecb.h src/lea_cbc.h \
	src/lea_ctr.h src/lea_cfb.h src/lea_ofb.h src/lea_cmac.h src/lea_ccm.h src/lea_gcm.h
	$(CC) $(CFLAGS) $(SIMD_SSE2) $(SHARED_FLAGS) -o $@ -c $<
src/lea_t_xop.o: src/lea_t_xop.c src/lea_key.h src/lea_ecb.h src/lea_cbc.h \
	src/lea_ctr.h src/lea_cfb.h src/lea_ofb.h src/lea_cmac.h src/lea_ccm.h src/lea_gcm.h
	$(CC) $(CFLAGS) $(SIMD_SSE2) $(SIMD_XOP) $(SHARED_FLAGS) -o $@ -c $<
src/lea_neon.o: src/lea_neon.c src/lea.h src/lea_locl.h src/config.h src/cpu_info.h
	$(CC) $(CFLAGS)  $(ARMFLAGS) $(SHARED_FLAGS) -o $@ -c $<

.c.o:
	$(CC) $(CFLAGS) $(ARMFLAGS) $(SHARED_FLAGS) -o $@ -c $<


src/cpu_info_arm.o: src/cpu_info_arm.c src/cpu_info.h src/lea_locl.h src/config.h
src/cpu_info_ia32.o: src/cpu_info_ia32.c src/cpu_info.h src/lea_locl.h src/config.h
src/lea_base.o: src/lea_base.c src/lea.h src/lea_locl.h src/config.h src/cpu_info.h
src/lea_online.o: src/lea_online.c src/lea.h src/lea_locl.h src/config.h src/cpu_info.h
src/lea_core.o: src/lea_core.c src/lea.h src/lea_locl.h src/config.h src/cpu_info.h
src/lea_gcm_generic.o: src/lea_gcm_generic.c src/lea.h src/lea_locl.h src/config.h src/cpu_info.h
src/lea_t_fallback.o : src/lea_t_fallback.c src/lea.h src/lea_locl.h src/config.h src/cpu_info.h
src/lea_t_generic.o : src/lea_t_generic.c src/lea_ecb.h src/lea_cbc.h src/lea_ctr.h src/lea_cfb.h \
		src/lea_ofb.h src/lea_cmac.h src/lea_ccm.h src/lea_gcm.h
main.o : main.c lea_vs.h lea_benchmark.h benchmark.h src/lea.h
lea_benchmark.o : lea_benchmark.c lea_benchmark.h benchmark.h src/lea.h src/lea_locl.h src/config.h
lea_vs.o : lea_vs.c lea_vs.h src/lea.h src/lea_locl.h src/config.h
benchmark.o : benchmark.c benchmark.h
main_0tv.o: main_0tv.c src/lea.h src/lea_locl.h src/config.h src/cpu_info.h


