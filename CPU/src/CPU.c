/*
 ============================================================================
 Name        : CPU.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "libs/funcionesCPU.h"

#include "commons/collections/list.h"

t_hilo* tcb;
char* PATH;

int main(int argc, char** argv) {

	PATH = argv[1];
	inicializar_configuracion();

	//Abrir conexion de sockets con Kernel y MSP.

	//Recibir quantum del Kernel

	//ejecutar();

	return EXIT_SUCCESS;
}
