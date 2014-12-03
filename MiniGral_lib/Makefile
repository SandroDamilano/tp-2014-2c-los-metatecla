CC := gcc
RM := rm -rf
C_SRCS := ./serialize/functions.c ./sockets/enviar_recibir_paquete.c funciones_comunes.c
H_SRCS := ./serialize/functions.h ./sockets/enviar_recibir_paquete.h estructuras_auxiliares.h
OBJS := functions.o enviar_recibir_paquete.o funciones_comunes.o

all:
	make functions
	make enviar_recibir_paquete
	make funciones_comunes
	make libMiniGral_lib.so

libMiniGral_lib.so:
	$(CC) -shared -Wall -o "libMiniGral_lib.so" $(OBJS)

functions: ./serialize/functions.c
	$(CC) -c -fmessage-length=0 -fPIC ./serialize/functions.c

enviar_recibir_paquete: ./sockets/enviar_recibir_paquete.c
	$(CC) -c -fmessage-length=0 -fPIC ./sockets/enviar_recibir_paquete.c

funciones_comunes: funciones_comunes.c
	$(CC) -c -fmessage-length=0 -fPIC funciones_comunes.c

install: all
	sudo cp libMiniGral_lib.so /usr/lib
	sudo cp ./serialize/functions.h /usr/include
	sudo cp ./sockets/enviar_recibir_paquete.h /usr/include
	sudo cp funciones_comunes.h /usr/include
	sudo cp estructuras_auxiliares.h /usr/include
	make clean

uninstall:
	sudo rm -f /usr/lib/libMiniGral_lib.so
	sudo rm -rf /usr/include/serialize/functions.h
	sudo rm -rf /usr/include/sockets/enviar_recibir_paquete.h
	sudo rm -rf /usr/include/funciones_comunes.h
	sudo rm -rf /usr/include/estructuras_auxiliares.h

clean:
	$(RM) libMiniGral_lib.so
	$(RM) functions.o
	$(RM) enviar_recibir_paquete.o
	$(RM) funciones_comunes.o

reinstall:
	make uninstall
	make install

.PHONY: all install uinstall clean reinstall
