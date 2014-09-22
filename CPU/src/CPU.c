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

#include "commons/bitarray.h"
#include <math.h>

t_hilo* tcb;
char* PATH;

int main(int argc, char** argv) {
	/*t_bitarray* bit = bitarray_create("173",1);
	printf("max bits %d\n",bitarray_get_max_bit(bit));
	int i = 0;
	while(i < 8){
		int a = bitarray_test_bit(bit,i);
		printf("%d con i = %d\n",a,i);
		i++;
	}*/

	PATH = argv[1];
	inicializar_configuracion();

	//Abrir conexion de sockets con Kernel y MSP.

	//Recibir quantum del Kernel

	//ejecutar();

	return EXIT_SUCCESS;
}
