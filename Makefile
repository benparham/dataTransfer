CC = gcc
AR = ar

TARGET = libtransfer.a

INC = transfer.h
SRC = transfer.c
OBJ = transfer.o

CFLAGS = -g -Wall -std=c99 -I .
ARFLAGS = ru

# Rules #######################

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

$(OBJ): $(SRC) $(INC)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm $(TARGET)
	@rm $(OBJ)