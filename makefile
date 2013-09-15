
CC = gcc
RM = rm

LEVELDB = bench_leveldb
LEVELDB_CFLAGS = -I. -Ileveldb/include/leveldb -Wall -O2 -DNDEBUG
LEVELDB_LFLAGS = -Lleveldb -lleveldb

HYPERLEVELDB = bench_hyperleveldb
HYPERLEVELDB_CFLAGS = -I. -Ihyperleveldb/hyperleveldb -Wall -O2 -DNDEBUG
HYPERLEVELDB_LFLAGS = -Lhyperleveldb/.libs/ -lhyperleveldb -pthread

SOPHIA = bench_sophia
SOPHIA_CFLAGS = -I. -Isophia/db -Wall -O2 -DNDEBUG
SOPHIA_LFLAGS = -Lsophia/db -lsophia -pthread

all: build ${LEVELDB} ${SOPHIA}
build:
	@(cd sophia; make)
	@(cd leveldb; make)
${LEVELDB}: clean
	$(CC) ${LEVELDB_CFLAGS} ${LEVELDB}.c $(LEVELDB_LFLAGS) -o ${LEVELDB}
${HYPERLEVELDB}: clean
	$(CC) ${HYPERLEVELDB_CFLAGS} ${LEVELDB}.c $(HYPERLEVELDB_LFLAGS) -o ${HYPERLEVELDB}
${SOPHIA}: clean
	$(CC) ${SOPHIA_CFLAGS} ${SOPHIA}.c $(SOPHIA_LFLAGS) -o ${SOPHIA}
clean:
	$(RM) -f ${LEVELDB} ${HYPERLEVELDB} ${SOPHIA}
