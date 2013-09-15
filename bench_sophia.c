
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#include <bench.h>
#include <sophia.h>

static void *env;
static void *db;

static char key[16];
static char value[100];
static uint64_t keyseq = 0;
static int keytraversed = 0;

static int
setseq(int n)
{
	int rc;
	int i;
	for (i = 0; i < n; i++) {
		memcpy(key + 8, &keyseq, sizeof(keyseq));
		keyseq++;
		rc = sp_set(db, key, sizeof(key), value, sizeof(value));
		if (rc == -1) {
			printf("sp_set: %s\n", sp_error(db));
			return 1;
		}
		print_current(i);
	}
	return 0;
}

static int
setrand(int n)
{
	int rc;
	int i;
	for (i = 0; i < n; i++) {
		uint32_t k = rand();
		memcpy(key, &k, sizeof(k));
		k = rand();
		memcpy(key + 4, &k, sizeof(k));
		k = rand();
		memcpy(key + 8, &k, sizeof(k));
		k = rand();
		memcpy(key + 12, &k, sizeof(k));
		rc = sp_set(db, key, sizeof(key), value, sizeof(value));
		if (rc == -1) {
			printf("sp_set: %s\n", sp_error(db));
			return 1;
		}
		print_current(i);
	}
	return 0;
}

static int
getseq(int n)
{
	int rc;
	int i;
	for (i = 0; i < n; i++) {
		memcpy(key + 8, &keyseq, sizeof(keyseq));
		keyseq++;
		void *v = NULL;
		size_t vsize = 0;
		rc = sp_get(db, key, sizeof(key), &v, &vsize);
		if (rc <= 0) {
			printf("get: %s\n", sp_error(db));
			break;
		}
		free(v);
		print_current(i);
	}
	return 0;
}

static int
getrand(int n)
{
	int rc;
	int i;
	for (i = 0; i < n; i++) {
		uint32_t k = rand();
		memcpy(key, &k, sizeof(k));
		k = rand();
		memcpy(key + 4, &k, sizeof(k));
		k = rand();
		memcpy(key + 8, &k, sizeof(k));
		k = rand();
		memcpy(key + 12, &k, sizeof(k));
		void *v = NULL;
		size_t vsize = 0;
		rc = sp_get(db, key, sizeof(key), &v, &vsize);
		if (rc <= 0) {
			printf("get: %s\n", sp_error(db));
			break;
		}
		free(v);
		print_current(i);
	}
	return 0;
}

static int
rangefwd(int n)
{
	void *c = sp_cursor(db, SPGTE, NULL, 0);
	if (c == NULL) {
		printf("cursor failed: %s\n", sp_error(db));
		return 1;
	}
	while (sp_fetch(c)) {
		const char *k = sp_key(c);
		size_t sz = sp_valuesize(c);
		(void)k;
		(void)sz;
		keytraversed++;
	}
	sp_destroy(c);
	return 0;
}

static void usage(char *argv0)
{
	printf("%s <type> <n>\n", argv0);
	printf("  type: setseq, setrand, getseq, getrand, rangefwd\n");
	exit(1);
}

int
main(int argc, char * argv[])
{
	if (argc != 3)
		usage(argv[0]);

	int n = atol(argv[2]);
	srand(701888);

	env = sp_env();
	sp_ctl(env, SPDIR, SPO_CREAT|SPO_RDWR, "./sophia_bench_data");
	db = sp_open(env);
	if (db == NULL) {
		printf("open: %s\n", sp_error(env));
		return 1;
	}
	
	int rc = 0;
	unsigned long long start = now();
	
	switch (benchof(argv[1]))
	{
	case SETSEQ: rc = setseq(n);
		break;
	case SETRAND: rc = setrand(n);
		break;
	case GETSEQ:rc = getseq(n);
		break;
	case GETRAND: rc = getrand(n);
		break;
	case RANGEFWD: rc = rangefwd(n);
		break;
	case UNKNOWN:
		printf("unknown operation\n");
	}

	if (rc == 0) {
		unsigned long long diff = now() - start;
		float rps = n / (diff / 1000.0);
		printf("%d rps\n", (int)rps);
	}
	if (keytraversed > 0 && keytraversed != n)
		printf("(%d keys traversed)\n", keytraversed);

	extern int gaps;
	extern int total;
	printf("(gaps: %d, total: %d)\n", gaps, total);

	sp_destroy(db);
	sp_destroy(env);
	return rc;
}
