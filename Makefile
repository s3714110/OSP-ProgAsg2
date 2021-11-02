CFLAGS = -g -Wall -std=c11
CC = gcc
LIBS =
TARGET = VSFS
SOURCES = init.c list.c mkdir.c rm.c defrag.c index.c copy.c base64.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = $(SOURCES:.c=.h)
RM = rm -f

.PHONY: all clean


all: $(TARGET)

$(TARGET): main.o $(OBJECTS)
	${CC} ${CFLAGS} -o $(TARGET) main.o $(OBJECTS) ${LIBS} 

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

copy.o: copy.c rm.h mkdir.h base64.h
	$(CC) $(CFLAGS) -c copy.c -o copy.o

main.o: main.c $(HEADERS)
	${CC} ${CFLAGS} -c main.c -o main.o

clean:
	${RM} ${TARGET} *.o