SQLITE_VERSION=3071602
SQLITE_URL=http://www.sqlite.org/2013/
SQLITE_ARCHIVE=sqlite-amalgamation-${SQLITE_VERSION}.zip
SQLITE_INCLUDE=sqlite-amalgamation-${SQLITE_VERSION}
SQLITE_C=${SQLITE_INCLUDE}/sqlite.c

CFLAGS=-Wall -I./ -I${SQLITE_INCLUDE} -DURL=\"$(shell cat .remote_url)\"

memebase: memebase.c common.c common.h ${SQLITE_C}
	gcc ${CFLAGS} memebase.c common.c ${SQLITE_C} -o memebase 

${SQLITE_ARCHIVE}:
	wget ${SQLITE_URL}/${SQLITE_ARCHIVE}

${SQLITE_C}: ${SQLITE_ARCHIVE}
	unzip ${SQLITE_ARCHIVE}
	touch ${SQLITE_C}

clean:
	-rm memebase
	-rm -fR ${SQLITE_INCLUDE}

mrproper: clean
	rm ${SQLITE_ARCHIVE}

deploy: memebase
	ascp memebase $(shell cat .remote_path)

.PHONY: clean mrproper deploy
