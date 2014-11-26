CC := gcc
CFLAGS := -Wall -ansi

OBJS := MSP.o funcionesMSP.o
LIBS := -lpthread -lcommons -lparser-ansisop -lMiniGral_lib

#All

all: MSP

#MSP

MSP:  $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o MSP $(OBJS)

MSP.o: ./src/MSP.c ./src/MSP.h
	$(CC) -c $(CFLAGS)  ./src/MSP.c

funcionesMSP.o: ./src/funcionesMSP.c ./src/funcionesMSP.h  ../MiniGral_lib/sockets/enviar_recibir_paquete.h
	$(CC) -c $(CFLAGS) -lMiniGral_lib ./src/funcionesMSP.c