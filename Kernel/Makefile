

OBJS := main.o funciones_kernel.o loader.o planificador.o
LIBS := -lMiniGral_lib  -lcommons -lansisop-panel -lpthread
MINI:=/home/utnso/tp-2014-2c-los-metatecla/MiniGral_lib
ANSI:=/home/utnso/tp-2014-2c-los-metatecla/ansisop-panel

#All

all: KERNEL

#Consola

KERNEL:  $(OBJS)
	gcc -Wall   -L $(MINI) -L $(ANSI) $(OBJS) $(LIBS) -o KERNEL

main.o: ./src/main.c ./src/loader.c ./src/planificador.c ./src/funciones_kernel.c
	gcc -c -I $(MINI) -I $(ANSI) ./src/main.c ./src/loader.c ./src/planificador.c ./src/funciones_kernel.c

funciones_kernel.o: ./src/funciones_kernel.c 
	gcc -c -I $(MINI) -I $(ANSI) ./src/funciones_kernel.c 

loader.o:  ./src/loader.c  ./src/funciones_kernel.c   ./src/planificador.c 
	gcc -c -I $(MINI) -I $(ANSI) ./src/loader.c  ./src/funciones_kernel.c   ./src/planificador.c 

planificador.o: ./src/planificador.c ./src/funciones_kernel.c 
	gcc -c -I $(MINI) -I $(ANSI) ./src/planificador.c ./src/funciones_kernel.c  

#Correr Kernel
#Hay que correrlo con valgrind en la prueba ??
run: 
	./KERNEL ./config.cfg
#Clean
clean: 
	rm KERNEL $(OBJS)
