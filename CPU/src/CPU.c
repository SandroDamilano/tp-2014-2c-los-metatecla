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

	int* codigo = malloc(7*sizeof(uint32_t));
	codigo = "LOADA1253";
	printf("codigo = %s\n",codigo);
	printf("codigo[0] = %d y en char = %c\n",codigo[0],codigo[0]);
	char* bytecode;
	bytecode = obtener_bytecode(codigo);

	printf("bytecode[0] = %d y en char = %c\n", bytecode[0], bytecode[0]);
	printf("el bytecode es = %s\n", bytecode);
	printf("lo convierto a string = %d\n",convertirAString(bytecode));

	int reg = obtener_registro(codigo,4);
	printf("el registro es = %d y en char = %c\n", reg, reg);
	int num = obtener_numero(codigo, 5);
	printf("el numero es = %d y en s = %s y en char = %c\n",num, string_itoa(num),num);

	t_list* parametros = list_create();
	list_add(parametros,&reg);
	list_add(parametros,&num);

	ejecucion_instruccion(bytecode,parametros);

	PATH = argv[1];
	inicializar_configuracion();

	//Abrir conexion de sockets con Kernel y MSP.

	//Recibir quantum del Kernel

	//ejecutar();

	return EXIT_SUCCESS;
}
