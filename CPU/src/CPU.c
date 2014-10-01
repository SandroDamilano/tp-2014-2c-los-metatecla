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
#include <string.h>
#include <math.h>

t_hilo* tcb;
char* PATH;
t_config* config_cpu;
t_config_cpu config_struct_cpu;

typedef struct{
	int32_t numero_struct;
} __attribute__ ((__packed__)) t_numero;

int main(int argc, char** argv) {

	PATH = argv[1];
	inicializar_configuracion();

	//Abrir conexion de sockets con Kernel y MSP.
	//Recibir quantum del Kernel

		//socket a Kernel pidiendo tcb
	//socket de Kernel con tcb

	int* bytecode = malloc(sizeof(uint32_t));
	cantidad_lineas_ejecutadas = 0;
	copiar_tcb_a_registros();
	tcb->cola = EXEC;
	comienzo_ejecucion(tcb,quantum);

	while(1){
	if(registros_cpu.K == 1){ //SI NO ES MODO KERNEL

		if(tcb->cola == EXIT){
			//socket a Kernel enviandole el tcb ya actualizado en la operacion XXXX
			//socket de Kernel con tcb nuevo

			fin_ejecucion();

			cantidad_lineas_ejecutadas = 0;

			copiar_tcb_a_registros();
			tcb->cola = EXEC;
			comienzo_ejecucion(tcb,quantum);

		} else {
			if(cantidad_lineas_ejecutadas == quantum){
				tcb->cola = BLOCK; //esta bien este estado?
				copiar_registros_a_tcb();
				//socket a Kernel con el tcb
				fin_ejecucion();

				//socket a Kernel pidiendo tcb
				//socket de Kernel con tcb

				cantidad_lineas_ejecutadas = 0;

				copiar_tcb_a_registros();
				tcb->cola = EXEC;
				comienzo_ejecucion(tcb,quantum);

			} else{
				//socket a MSP con PC
				//socket de MSP con bytecode

				ejecutarLinea(bytecode);
				cantidad_lineas_ejecutadas++;
			}
		}
	}

	if(registros_cpu.K == 0){ //SI ES MODO KERNEL
		if(tcb->cola == EXIT){
			//socket a Kernel enviandole el tcb ya actualizado en la operacion XXXX
			//socket de Kernel con tcb nuevo
			fin_ejecucion();

			cantidad_lineas_ejecutadas = 0;

			copiar_tcb_a_registros();
			tcb->cola = EXEC;
			comienzo_ejecucion(tcb,quantum);
		} else{
			//socket a MSP con PC
			//socket de MSP con bytecode
			ejecutarLinea(bytecode);
			}
		}
	}

	free(bytecode);

	return EXIT_SUCCESS;
}
