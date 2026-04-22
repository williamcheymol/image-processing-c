# William CHEYMOL / 1A

CC = gcc
CFLAGS = -Wall -g

TARGET = projet_wc

SRCS = main.c lut.c filename.c pictures.c pixel.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

#Explication des dépendances
filename.o : filename.h
pixel.o : pixel.h pictures.h
lut.o : lut.h pictures.h pixel.h
main.o: pictures.h filename.h
pictures.o: lut.h pixel.h filename.h pictures.h

#Edition de liens
$(TARGET): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c $< -o $@

clean :
	rm -f $(OBJS) $(TARGET)
