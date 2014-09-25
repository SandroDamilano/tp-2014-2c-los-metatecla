/*
 * ConsolaMSP.c
 *
 *  Created on: 11/09/2014
 *      Author: utnso
 */

#include "ConsolaMSP.h"

void *inciarConsola(void *param1){

	char command;
	uint32_t baseSeg, direcVir;
	int terminarConsola = 1;

	printf("\nConsola MSP inciada(exit para cerrar)\n");
    printf("Introduzca numero de comando elegido:\n"
    		"1. Crear segmento\n"
    		"2. Destruir Segmento\n"
    		"3. Escribir memoria\n"
    		"4. Leer memoria\n"
    		"5. Tabla de segmentos\n"
    		"6. Tabla de paginas\n"
    		"7. Listar marcos\n"
    		"8. Exit\n");
	while (terminarConsola){
	scanf("%c",&command);
	switch(command){
	case '1': printf("El comando elegido fue: Crear Segmento\n"
				"Ingrese un PID:\n");
				// definir que tipo de dato es el PID
				printf("Ingrese Tamaño (en bytes):\n");
				// definir tamaño de bytes
	break;
	case '2': printf("El comando elegido fue: Destruir segmento\n"
				"Ingrese un PID:\n");
				// definir que tipo de dato es el PID
				printf("Ingrese base del segmento:");
				scanf("%d",&baseSeg);
	break;
	case '3': printf("El comando elegido fue: Escribir Memoria\n"
				"Ingrese un PID:\n");
				// definir que tipo de dato es el PID
				printf("Ingrese direccion virtual:\n");
				scanf("%d", &direcVir);
				printf("Ingrese un tamaño:\n");
				// definir tamaño de bytes
				printf("Ingrese un texto:\n");
				//definir tipo texto

	break;
	case '4': printf("El comando elegido fue: Leer memoria\n"
				"Ingrese un PID:\n");
				// definir que tipo de dato es el PID
				printf("Ingrese una direccion virtual:\n");
				scanf("%d", &direcVir);
				printf("Ingrese un tamaño:\n");
				// definir tamaño de bytes
	break;
	case '5': printf("El comando elegido fue: Tabla de segmentos\n"); break;
	case '6': printf("El comando elegido fue: Tabla de paginas\n"
				"Ingrese un PID:\n");
				// definir que tipo de dato es el PID\n");
	break;
	case '7': printf("El comando elegido fue: Listar marcos\n"); break;
	case '8': terminarConsola=0; break;
	}
	}
pthread_exit((void*) "Termino el hilo Consola");	/* No se si esta bien implementado el pthread_exit */
// Falta ajustar cada case a lo que el comando pida: definir que funcion se llama de la MSP, definir tipos y liberar memoria
return NULL;
}


