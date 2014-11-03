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

void * structRecibido;
t_tipoEstructura tipo_struct;
void* datos_recibidos;
int resultado;

int main(int argc, char** argv) {

	PATH = argv[1];
	inicializar_configuracion();

	tcb = malloc(sizeof(t_hilo));

	//Abrir conexion de sockets con Kernel y MSP.
	//int sockKernel = socket_crearYConectarCliente(config_struct_cpu.ip_kernel, config_struct_cpu.puerto_kernel);

	printf("me voy a conectar a msp\n");

	int sockMSP = socket_crearYConectarCliente(config_struct_cpu.ip_msp, config_struct_cpu.puerto_msp);

	if(sockMSP == -1){
		printf("no se pudo conectar a MSP\n");
	} else {
		printf("numero de socket de MSP es %d\n", sockMSP);
	}

	/*
	//Recibir quantum del Kernel
	socket_recibir(sockKernel, &tipo_struct, &structRecibido);
	if(tipo_struct == D_STRUCT_QUANTUM){
		quantum = ((t_struct_numero *) structRecibido)->numero;
	}

	//socket a Kernel pidiendo tcb
	t_struct_numero* pedir_tcb = malloc(sizeof(t_struct_numero));
	resultado = socket_enviar(sockKernel, D_STRUCT_PEDIR_TCB, pedir_tcb);
	if(resultado != 1){
		printf("No se pudo pedir TCB\n");
	}

	//socket de Kernel con tcb
	socket_recibir(sockKernel, &tipo_struct, &structRecibido);
	copiar_structRecibido_a_tcb(tcb, structRecibido);*/


	//////////////////////
	quantum = 3;

	tcb->pid = 0;
	tcb->tid = 0;
	tcb->kernel_mode = true;
	tcb->segmento_codigo = 0;
	tcb->segmento_codigo_size = 99;
	tcb->registros[0] = 0;
	tcb->registros[1] = 0;
	tcb->registros[2] = 0;
	tcb->registros[3] = 0;
	tcb->registros[4] = 0;
	tcb->puntero_instruccion = 0;
	///////////////////////


	int bytecode[4];// = malloc(sizeof(uint32_t));

	uint32_t direccionMSP;
	t_struct_sol_bytes* datos_solicitados = malloc(sizeof(t_struct_sol_bytes));

	cantidad_lineas_ejecutadas = 0;
	copiar_tcb_a_registros();
	comienzo_ejecucion(tcb,quantum);


	while(1){
	if(registros_cpu.K == false){ //SI NO ES MODO KERNEL

		if(terminoEjecucion == true){
			//socket a Kernel pidiendo tcb
			t_struct_numero* pedir_tcb = malloc(sizeof(t_struct_numero));
			int resultado = socket_enviar(sockKernel, D_STRUCT_PEDIR_TCB, pedir_tcb);
			if(resultado != 1){
				printf("No se pudo pedir TCB\n");
			}

			//socket de Kernel con tcb
			socket_recibir(sockKernel, &tipo_struct, &structRecibido);
			copiar_structRecibido_a_tcb(tcb, structRecibido);

			cantidad_lineas_ejecutadas = 0;

			terminoEjecucion = false;

			comienzo_ejecucion(tcb,quantum);

		} else {
			if(cantidad_lineas_ejecutadas == quantum){

				//socket a kernel con tcb
				copiar_registros_a_tcb();
				t_struct_tcb* tcb_enviar = malloc(sizeof(t_struct_tcb));
				copiar_tcb_a_structTcb(tcb, tcb_enviar);
				resultado = socket_enviar(sockKernel, D_STRUCT_TCB, tcb_enviar);
				controlar_envio(resultado, D_STRUCT_TCB);

				fin_ejecucion();

				//socket a Kernel pidiendo tcb
				t_struct_numero* pedir_tcb = malloc(sizeof(t_struct_numero));
				int resultado = socket_enviar(sockKernel, D_STRUCT_PEDIR_TCB, pedir_tcb);
				if(resultado != 1){
					printf("No se pudo pedir TCB\n");
				}

				//socket de Kernel con tcb
				socket_recibir(sockKernel, &tipo_struct, &structRecibido);
				copiar_structRecibido_a_tcb(tcb, structRecibido);

				cantidad_lineas_ejecutadas = 0;

				copiar_tcb_a_registros();
				comienzo_ejecucion(tcb,quantum);

			} else{
				//socket a MSP con PC
				direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

				datos_solicitados->base = direccionMSP;
				datos_solicitados->PID = tcb->pid;
				datos_solicitados->tamanio = 4; //tamaño bytecode

				resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
				controlar_envio(resultado, D_STRUCT_SOL_BYTES);

				//socket de MSP con bytecode

				socket_recibir(sockMSP, &tipo_struct, &structRecibido);
				controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

				datos_recibidos = malloc(sizeof(int32_t)); //numero
				datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

				obtener_num(datos_recibidos,0,bytecode);

				free(datos_recibidos);
				ejecutarLinea(bytecode);
				cantidad_lineas_ejecutadas++;
			}
		}
	}

	if(registros_cpu.K == true){ //SI ES MODO KERNEL
		if(terminoEjecucion == true){
			//socket a Kernel pidiendo tcb
			t_struct_numero* pedir_tcb = malloc(sizeof(t_struct_numero));
			int resultado = socket_enviar(sockKernel, D_STRUCT_PEDIR_TCB, pedir_tcb);
			if(resultado != 1){
				printf("No se pudo pedir TCB\n");
			}

			//socket de Kernel con tcb
			socket_recibir(sockKernel, &tipo_struct, &structRecibido);
			copiar_structRecibido_a_tcb(tcb, structRecibido);

			cantidad_lineas_ejecutadas = 0;

			terminoEjecucion = false;

			comienzo_ejecucion(tcb,quantum);
		} else{
			//socket a MSP con PC
			direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

			datos_solicitados->base = direccionMSP;
			datos_solicitados->PID = tcb->pid;
			datos_solicitados->tamanio = 4; //tamaño bytecode

			resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
			controlar_envio(resultado, D_STRUCT_SOL_BYTES);

			//socket de MSP con bytecode

			socket_recibir(sockMSP, &tipo_struct, &structRecibido);
			controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

			//printf("recibi %s de tam %d\n", ((t_struct_respuesta_msp*) structRecibido)->buffer, ((t_struct_respuesta_msp*) structRecibido)->tamano_buffer);
			memcpy(bytecode,((t_struct_respuesta_msp*) structRecibido)->buffer, ((t_struct_respuesta_msp *) structRecibido)->tamano_buffer);
			printf("bytecode %s\n", bytecode);
			incrementar_pc(4);
			ejecutarLinea(bytecode);
			}
		}

	esperar_retardo();

	}

	//free(bytecode);
	free(datos_solicitados);
	//free(pedir_tcb);

	return EXIT_SUCCESS;
}
