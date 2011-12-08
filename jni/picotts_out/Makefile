CC=gcc
LD=gcc

CFLAGS=-Wall -fPIC
LDFLAGS=-Wall

PICOTTS_OUT=picoacph.o picocep.o picodbg.o picokdbg.o picoklex.o \
			picokpr.o picopal.o picorsrc.o picosig.o picotrns.o \
			picoapi.o picoctrl.o picoextapi.o picokdt.o picoknow.o \
			picoktab.o picopam.o picosa.o picospho.o picowa.o \
			picobase.o picodata.o picofftsg.o picokfst.o picokpdf.o \
			picoos.o picopr.o picosig2.o picotok.o

picotts: $(PICOTTS_OUT)

clean:
	rm -f -- *.o

%.o: ${PICOTTS_ROOT}/%.c
	$(CC) -o $@ -c $< $(CFLAGS)
