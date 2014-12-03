CC := gcc
RM := rm -rf
C_SRCS := cpu.c kernel.c panel.c
H_SRCS := cpu.h kernel.h panel.h
OBJS := cpu.o kernel.o panel.o

all:
	make cpu
	make kernel
	make panel
	make libansisop-panel.so

libMiniGral_lib.so:
	$(CC) -shared -Wall -o "libansisop-panel.so" $(OBJS)

cpu: cpu.c
	$(CC) -c -fmessage-length=0 -fPIC cpu.c

kernel: kernel.c
	$(CC) -c -fmessage-length=0 -fPIC kernel.c

panel: panel.c
	$(CC) -c -fmessage-length=0 -fPIC panel.c

install: all
	sudo cp libansisop-panel.so /usr/lib
	sudo cp cpu.h /usr/include
	sudo cp kernel /usr/include
	sudo cp panel.h /usr/include
	make clean

uninstall:
	sudo rm -f /usr/lib/libansisop-panel.so
	sudo rm -rf /usr/include/cpu.h
	sudo rm -rf /usr/include/kernel.h
	sudo rm -rf /usr/include/panel.h

clean:
	$(RM) libansisop-panel.so
	$(RM) cpu.o
	$(RM) kernel.o
	$(RM) panel.o

reinstall:
	make uninstall
	make install

.PHONY: all install uinstall clean reinstall
