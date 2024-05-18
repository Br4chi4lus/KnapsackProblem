HDIR = headers
CC = gcc
CFLAGS = -I$(HDIR)

ODIR = obj

SDIR = source

_DEPS = knapsack.h
DEPS = $(patsubst %,$(HDIR)/%,$(_DEPS))

_OBJ = knapsack.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

LIBS = -lm

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
.PHONY: clean
clean:
		rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~