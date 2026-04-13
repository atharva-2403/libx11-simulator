CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lX11 -lm
TARGET = engine

all: $(TARGET)

$(TARGET): renderer.o shapes.o
	$(CC) -o $@ $^ $(LIBS)

renderer.o: renderer.c renderer.h shapes.h
	$(CC) $(CFLAGS) -c renderer.c -o $@

shapes.o: shapes.c shapes.h
	$(CC) $(CFLAGS) -c shapes.c -o $@

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean