

OBJS := consola.o funciones_consola.o
LIBS := -lMiniGral_lib  -lcommons -lansisop-panel -lpthread
MINI:=/home/utnso/tp-2014-2c-los-metatecla/MiniGral_lib
ANSI:=/home/utnso/tp-2014-2c-los-metatecla/ansisop-panel

#All

all: consola

#Consola

consola:  $(OBJS)
	gcc -Wall   -L $(MINI) -L $(ANSI) $(OBJS) $(LIBS) -o CONSOLA

consola.o: ./src/consola.c ./src/funciones_consola.c 
	gcc -c -I $(MINI) -I $(ANSI)  ./src/consola.c ./src/funciones_consola.c 

funcionesMSP.o: ./src/funciones_consola.c 
	gcc -c -I $(MINI) -I $(ANSI) ./src/funciones_consola.c 

#Correr Consola
#Hay que correrlo con valgrind en la prueba ??
run: 
	./CONSOLA

#Clean
clean: 
	rm CONSOLA $(OBJS)
