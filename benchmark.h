#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_



typedef unsigned int uint_32t;



#define TIMER_SAMPLE_CNT (100)

#define LOOP_COUNT	100
#define ARM_RATE	(1400 / LOOP_COUNT)


uint_32t HiResTime(void);           /* return the current value of time stamp counter */
uint_32t calibrate();
float get_cpb(unsigned int cycle, unsigned int data_len);



#endif
