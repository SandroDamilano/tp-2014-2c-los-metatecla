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

	/*int* codigo = malloc(7*sizeof(uint32_t));
	codigo = "SETM1AB";
	printf("codigo = %s\n",codigo);
	printf("codigo[0] = %d y en char = %c\n",codigo[0],codigo[0]);
	char* bytecode = codigo;


	int reg = obtener_registro(codigo,5);
	printf("el registro es = %d y en char = %c\n", reg, reg);
	int reg2 = obtener_registro(codigo,6);
	printf("el registro es = %d y en char = %c\n", reg2, reg2);
	int num = obtener_numero(codigo, 4);
	printf("el numero es = %d y en s = %s y en char = %c\n",num, string_itoa(num),num);
	char* j = string_itoa(num);
	printf("el numero es s = %s\n",j);

	t_list* parametros = list_create();
	list_add(parametros,&j);
	list_add(parametros,&reg);
	list_add(parametros,&reg2);

	ejecucion_instruccion(bytecode,parametros);*/

	PATH = argv[1];
	inicializar_configuracion();

	//Abrir conexion de sockets con Kernel y MSP.

	//Recibir quantum del Kernel

	//ejecutar();

	return EXIT_SUCCESS;
}
