CC=gcc
LD=gcc

JAVA_HOME=`readlink -f /usr/bin/javac | sed "s:bin/javac::"`
CFLAGS=-Wall -I${PICOTTS_ROOT} -I${JAVA_HOME}/include/ -I${JAVA_HOME}/include/linux/ -fPIC
LDFLAGS=-Wall

world: jpicotts.so

picotts: picottsroot_verify 
	+cd picotts_out && make $(MFLAGS)
	
jpicotts.so: picotts jpicotts.o 
	${LD} -o libjpicotts.so -shared ${LDFLAGS} jpicotts.o picotts_out/*.o
	
jpicotts.o: jpicotts.c org_alexis_jpicotts_PicoTTS.h picottsroot_verify
	${CC} -c ${CFLAGS} jpicotts.c

.PHONY: clean picottsroot_verify

clean:
	rm -f -- *.o
	rm -f libjpicotts.so
	cd picotts_out && make clean

picottsroot_verify:
	@if [[ "${PICOTTS_ROOT}" == "" ]] ; then \
		echo "You must configure PICOTTS_ROOT to point picotts sources" ;\
		exit 1 ; \
	fi \
