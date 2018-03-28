IDIR =./include
CC=g++
CFLAGS=-I$(IDIR) -Wall -Wextra

# ODIR=obj
ODIR=./
LDIR=/usr/local/Cellar/libusb/1.0.21/lib/

LIBS=-lusb-1.0

_DEPS = penseldriver.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = penseldriver.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

penseldriver: $(OBJ)
	${CC} -o $@ $^ $(CFLAGS) $(LIBS) -L ${LDIR}

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
