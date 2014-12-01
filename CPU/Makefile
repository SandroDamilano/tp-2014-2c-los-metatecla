

OBJS := CPU.o  ./funcionesCPU.o  ./operaciones.o ./auxiliares.o
LIBS := -lMiniGral_lib  -lcommons -lansisop-panel -lpthread
MINI:=/home/utnso/tp-2014-2c-los-metatecla/MiniGral_lib
ANSI:=/home/utnso/tp-2014-2c-los-metatecla/ansisop-panel

#All

all: CPU

#CPU

CPU:  $(OBJS)
	gcc -Wall   -L $(MINI) -L $(ANSI) $(OBJS) $(LIBS) -o CPU

CPU.o: ./src/CPU.c ./src/libs/funcionesCPU.c 
	gcc -c -I $(MINI) -I $(ANSI) ./src/CPU.c ./src/libs/funcionesCPU.c  

funcionesMSP.o: ./src/funcionesCPU.c  ./src/libs/operaciones.c
	gcc -c -I $(MINI) -I $(ANSI) ./src/funcionesCPU.c  ./src/libs/operaciones.c

operaciones.o: ./src/libs/operaciones.c ./src/libs/auxiliares.c 
	gcc -c -I $(MINI) -I $(ANSI) ./src/libs/operaciones.c ./src/libs/auxiliares.c

auxiliares.o:  ./src/libs/auxiliares.c 
	gcc -c -I $(MINI) -I $(ANSI) ./src/libs/auxiliares.c 

#Correr CPU
#Hay que correrlo con valgrind en la prueba ??
run: 
	./CPU ./src/config.cfg

#Clean
clean: 
	rm CPU $(OBJS)
