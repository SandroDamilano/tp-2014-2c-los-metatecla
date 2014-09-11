/*
 ============================================================================
 Name        : CPU.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>


char* PATH;

int main(int argc, const char* argv[]) {

	PATH = argv[1];
	inicializar_configuracion();

	return EXIT_SUCCESS;
}
