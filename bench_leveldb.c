
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#include <bench.h>
#include <c.h>

static leveldb_readoptions_t *ropts;
static leveldb_writeoptions_t *wopts;
static leveldb_t *db;

static char key[16];
static char value[100];
static uint64_t keyseq = 0;
static int keytraversed = 0;

static int
setseq(int n)
{
	char *err = NULL;
	int i;
	for (i = 0; i < n; i++) {
		memcpy(key + 8, &keyseq, sizeof(keyseq));
		keyseq++;
		leveldb_put(db, wopts, key, sizeof(key), value, sizeof(value), &err);
		if (err != NULL) {
			printf("put failed: %s\n", err);
			return 1;
		}
		print_current(i);
	}
	return 0;
}

static int
setrand(int n)
{
	char *err = NULL;
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
		leveldb_put(db, wopts, key, sizeof(key), value, sizeof(value), &err);
		if (err != NULL) {
			printf("put failed: %s\n", err);
			return 1;
		}
		print_current(i);
	}
	return 0;
}

static int
getseq(int n)
{
	char *err = NULL;
	int i;
	for (i = 0; i < n; i++) {
		memcpy(key + 8, &keyseq, sizeof(keyseq));
		keyseq++;
		size_t vsize = 0;
		char *p = leveldb_get(db, ropts, key, sizeof(key), &vsize, &err);
		if (err != NULL || p == NULL) {
			printf("get failed: %s\n", err);
			return 1;
		}
		free(p);
		print_current(i);
	}
	return 0;
}

static int
getrand(int n)
{
	char *err = NULL;
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
		size_t vsize = 0;
		char *p = leveldb_get(db, ropts, key, sizeof(key), &vsize, &err);
		if (err != NULL || p == NULL) {
			printf("get failed: %s\n", err);
			return 1;
		}
		free(p);
		print_current(i);
	}
	return 0;
}

static int
rangefwd(int n)
{
	leveldb_iterator_t *it = leveldb_create_iterator(db, ropts);
	leveldb_iter_seek_to_first(it);
	while (leveldb_iter_valid(it)) {
		size_t sz = 0;
		const char *k = leveldb_iter_key(it, &sz);
		(void)k;
		keytraversed++;
		leveldb_iter_next(it);
	}
	leveldb_iter_destroy(it);
	return 0;
}

static void usage(char *argv0) {
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
	
	leveldb_options_t *opts = leveldb_options_create();
	leveldb_options_set_compression(opts, leveldb_no_compression);
	leveldb_options_set_info_log(opts, NULL);
	leveldb_options_set_create_if_missing(opts, 1);

	char *err = NULL;
	db = leveldb_open(opts, "./leveldb_bench_data", &err);
	if (err != NULL) {
		printf("failed to open db: %s\n", err);
		return 1;
	}

	wopts = leveldb_writeoptions_create();
	ropts = leveldb_readoptions_create();
	leveldb_readoptions_set_fill_cache(ropts, 1);

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

	leveldb_close(db);
	leveldb_readoptions_destroy(ropts);
	leveldb_writeoptions_destroy(wopts);
	leveldb_options_destroy(opts);
	return rc;
}
