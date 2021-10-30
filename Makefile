CFLAGS = -g -Wall
CC = gcc
LIBS =
TARGET = VSFS
SOURCES = init.c list.c mkdir.c rm.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = $(SOURCES:.c=.h)
RM = rm -f

.PHONY: all clean


all: $(TARGET)

$(TARGET): main.o $(OBJECTS)
	${CC} ${CFLAGS} -o $(TARGET) main.o $(OBJECTS) ${LIBS}

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c $(HEADERS)
	${CC} ${CFLAGS} -c main.c -o main.o

clean:
	${RM} ${TARGET} *.o