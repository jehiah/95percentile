TARGET ?=/usr/local
CFLAGS = -I. -I/usr/local/include -O2 -g
LIBS = -L. -L/usr/local/lib

all: 95percentile

95percentile: 95percentile.c 
	$(CC) $(CFLAGS) -o 95percentile 95percentile.c $(LIBS)

install:
	/usr/bin/install -d $TARGET/bin
	/usr/bin/install 95percentile $TARGET/bin/

clean:
	rm -f 95percentile *.o *.dSYM
