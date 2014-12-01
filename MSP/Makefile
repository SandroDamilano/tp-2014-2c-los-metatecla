

OBJS := MSP.o funcionesMSP.o
LIBS := -lMiniGral_lib  -lcommons -lansisop-panel -lpthread
MINI:=/home/utnso/tp-2014-2c-los-metatecla/MiniGral_lib
ANSI:=/home/utnso/tp-2014-2c-los-metatecla/ansisop-panel

#All

all: MSP

#MSP

MSP:  $(OBJS)
	gcc -Wall   -L $(MINI) -L $(ANSI) $(OBJS) $(LIBS) -o MSP

MSP.o: ./src/MSP.c ./src/funcionesMSP.c 
	gcc -c -I $(MINI) -I $(ANSI)  ./src/MSP.c ./src/funcionesMSP.c 

funcionesMSP.o: ./src/funcionesMSP.c 
	gcc -c -I $(MINI) -I $(ANSI) ./src/funcionesMSP.c 

#Correr MSP
#Hay que correrlo con valgrind en la prueba ??
run: 
	./MSP ./src/config.cfg

#Clean
clean: 
	rm MSP $(OBJS)
