#ifndef BENCH_H_
#define BENCH_H_

#include <sys/time.h>
#include <time.h>

unsigned long long now(void)
{
	unsigned long long tm;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	tm = ((long)tv.tv_sec) * 1000;
	tm += tv.tv_usec / 1000;
	return tm;
}

enum bench {
	UNKNOWN,
	SETSEQ,
	SETRAND,
	GETSEQ,
	GETRAND,
	RANGEFWD
};

static inline enum bench
benchof(char *str) {
	if (strcasecmp(str, "setseq") == 0)
		return SETSEQ;
	else
	if (strcasecmp(str, "setrand") == 0)
		return SETRAND;
	else
	if (strcasecmp(str, "getseq") == 0)
		return GETSEQ;
	else
	if (strcasecmp(str, "getrand") == 0)
		return GETRAND;
	else
	if (strcasecmp(str, "rangefwd") == 0)
		return RANGEFWD;
	return UNKNOWN;
}

static inline void
print_current(int i) {
	if (i > 0 && (i % 100000) == 0)
		printf("%.1fM\n", i / 1000000.0);
}

#endif
