/*
 * planificador.c
 *
 *  Created on: 06/10/2014
 *      Author: utnso
 */

#include "planificador.h"


//////////////////////////////////////////////////////////////////////

void* main_PLANIFICADOR(arg_PLANIFICADOR* parametros)
{
	inicializar_ready_block();
	inicializar_semaforo_ready();

	pthread_t thr_consumidor_new;
	pthread_create(&thr_consumidor_new, NULL, (void*)&poner_new_a_ready, NULL);

	boot(parametros->syscalls_path);

	pthread_join(thr_consumidor_new, NULL);

return 0;
}

/**************************** COLAS ***********************************/

void encolar_en_ready(t_hilo* tcb){
	pthread_mutex_lock(&mutex_ready);
	if (tcb->kernel_mode == 1){
		list_add_in_index(cola_ready, 0, (void*)tcb);
	}else{
		list_add(cola_ready, (void*)tcb);
	};
	pthread_mutex_unlock(&mutex_ready);
	tcb->cola = READY;
};


void pop_new(t_hilo* tcb){
	void *nuevo = queue_pop(cola_new);
	*tcb = *(t_hilo*)nuevo;
};

void sacar_de_new(t_hilo* tcb){
	consumir_tcb(pop_new, &sem_new, &mutex_new, tcb);
}

//Este hilo se queda haciendo loop hasta que termine la ejecución
void poner_new_a_ready(){
	while(1){
		t_hilo* tcb = malloc(sizeof(t_hilo));
		sacar_de_new(tcb);
		encolar_en_ready(tcb);
	}
};

t_hilo* obtener_tcb_a_ejecutar(){
	return (t_hilo*) list_remove(cola_ready, 0);
};

/********************************** BLOQUEAR ***************************************/

//Este bloquear es GENERAL. Para bloquear, usar los particulares.
void bloquear_tcb(t_hilo* tcb, t_evento evento, uint32_t parametro){
	t_data_nodo_block* data = malloc(sizeof(t_data_nodo_block));
	data->tcb = malloc(sizeof(t_hilo));
	data->tcb = tcb;
	data->evento = evento;
	data->parametro = parametro;
	list_add(cola_block, (void*)data);
	tcb->cola = BLOCK;
};

void bloquear_tcbKernel(t_hilo* tcb){
	bloquear_tcb(tcb, TCBKM, 0);
}

void bloquear_tcbJoin(t_hilo* tcb, uint32_t tid){
	bloquear_tcb(tcb, JOIN, tid);
}

void bloquear_tcbSystcall(t_hilo* tcb, uint32_t dir_systcall){
	bloquear_tcb(tcb, SYSTCALL, dir_systcall);
}

void bloquear_tcbSemaforo(t_hilo* tcb, uint32_t sem){
	bloquear_tcb(tcb, SEM, sem);
}


/******************* FUNCIONES BOOLEANAS PARA BUSCAR EN COLAS ***************************/

bool es_el_tcbKernel(t_data_nodo_block* data){
	return (data->evento == TCBKM);
};

bool es_el_tcbSystcall(t_data_nodo_block* data){
	return (data->evento == SYSTCALL) & (data->tcb->tid == tid_a_buscar);
}

bool es_el_tcbBuscado(t_data_nodo_block* data){
	return ((data->parametro == parametro_a_buscar) & (data->evento == evento_a_buscar));
};


/******************************* DESBLOQUEAR ***************************************/

// Este desbloquear es GENERAL. Para desbloquear, hay que usar los particulares.
void desbloquear_proceso(t_evento evento, uint32_t parametro){
	t_data_nodo_block *data_desbloqueado;
	parametro_a_buscar = parametro;
	evento_a_buscar = evento;
	data_desbloqueado = list_remove_by_condition(cola_block,(void*)es_el_tcbBuscado);
	if (data_desbloqueado != NULL){
		t_hilo * tcb_desbloqueado = data_desbloqueado->tcb;
		encolar_en_ready(tcb_desbloqueado);
	}
}

t_hilo* desbloquear_tcbKernel(){
	t_data_nodo_block* data = list_remove_by_condition(cola_block, (void*)es_el_tcbKernel);
	if (data != NULL){
		return data->tcb;
	}else{
		return NULL;
	}
};

t_hilo* desbloquear_tcbSystcall(uint32_t tid){
	tid_a_buscar = tid;
	t_data_nodo_block* data = list_remove_by_condition(cola_block, (void*)es_el_tcbSystcall);
	if (data != NULL){
		return data->tcb;
	}else{
		return NULL;
	}
}

void desbloquear_por_join(uint32_t tid){
	desbloquear_proceso(JOIN, tid);
}

void desbloquear_por_semaforo(uint32_t sem){
	desbloquear_proceso(SEM, sem);
}

t_data_nodo_block* desbloquear_alguno_por_systcall(t_hilo* tcb_kernel){
	return list_remove_by_condition(cola_block, (void*)esta_por_systcall);
}

/********************************* INICIO *******************************************/

void inicializar_ready_block(){
	cola_ready = list_create();
	cola_block = list_create();
}


void inicializar_semaforo_ready(){
	pthread_mutex_init(&mutex_ready, NULL);
};

void boot(char* systcalls_path){
	uint32_t dir_codigo;
	uint32_t dir_stack;
	int tamanio_codigo;

	//Levanta archivo de syst calls
	FILE* syscalls_file = abrir_archivo(systcalls_path, logger, &mutex_log);
	tamanio_codigo = calcular_tamanio_archivo(syscalls_file);
	char* syscalls_code = leer_archivo(syscalls_file, tamanio_codigo);

	//Manda codigo de syscalls a la MSP y recibe las direcciones de segmento de codigo y stack
	t_struct_string* paquete_syscalls = malloc(sizeof(t_struct_string));
	paquete_syscalls->string = syscalls_code;

	socket_enviar(sockMSP, D_STRUCT_STRING, paquete_syscalls);
	free(paquete_syscalls);

	void * structRecibido;
	t_tipoEstructura tipoStruct;

	socket_recibir(sockMSP, &tipoStruct, &structRecibido);
	if (tipoStruct != D_STRUCT_RESPUESTA_MSP) {
		printf("No se ha recibido correctamente direccion del codigo y direccion de stack\n");
	}

	void *datos_recibidos = malloc(2*sizeof(int32_t)); //direccion_codigo + direccion_stack
	datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

	memcpy(&dir_codigo,datos_recibidos,sizeof(uint32_t));
	memcpy(&dir_stack,datos_recibidos + sizeof(uint32_t),sizeof(uint32_t));

	free(datos_recibidos);

	//Crea hilo en modo kernel y lo encola en bloqueados
	t_hilo *tcb_kernel = crear_TCB(obtener_pid(), dir_codigo, dir_stack, tamanio_codigo);
	tcb_kernel->kernel_mode = true;
	bloquear_tcbKernel(tcb_kernel);
}


///////////////////////////////////////////////////////////////////////////
//EX SERVICIOS_CPU.C


/*************************** SYSCALLS *******************************************/
void copiar_tcb(t_hilo* original, t_hilo* copia){
	copia->tid = original->tid;
	copia->pid = original->pid;
	int i;
	for(i=0; i<=4; i++){
		copia->registros[i] = original->registros[i];
	}
};

void atender_systcall(t_hilo* tcb, uint32_t dir_systcall){
	t_hilo* tcb_kernel = desbloquear_tcbKernel();
	bloquear_tcbSystcall(tcb, dir_systcall);
	printf("está atendiendo al tid: %d\n", tcb->tid);
	if (tcb_kernel != NULL){
		copiar_tcb(tcb, tcb_kernel);
		tcb_kernel->puntero_instruccion = dir_systcall;
		encolar_en_ready(tcb_kernel);
	}
	//TODO Avisarle a la cpu que pida otro proceso para ejecutar
};

bool esta_por_systcall(t_data_nodo_block* data){
	return (data->evento == SYSTCALL);
}

void retornar_de_systcall(t_hilo* tcb_kernel){
	t_hilo* tcb = desbloquear_tcbSystcall(tcb_kernel->tid);
	int i;
	for(i=0; i<=4; i++){
		tcb->registros[i] = tcb_kernel->registros[i];
	}
	encolar_en_ready(tcb);
	bloquear_tcbKernel(tcb_kernel);
	t_data_nodo_block* data_otro_tcb = desbloquear_alguno_por_systcall(tcb_kernel);

	//TODO Avisarle a la cpu que pida otro proceso para ejecutar

	if (data_otro_tcb != NULL){
		atender_systcall(data_otro_tcb->tcb, data_otro_tcb->parametro);
	};
}

void crear_nuevo_hilo(t_hilo* tcb_padre){
	//TODO Averiguar qué hay que poner en el nuevo TCB hijo
}

/******************************** HANDLER CPU *****************************************/
void copiar_structRecibido_a_tcb(t_hilo* tcb, void* structRecibido){
	tcb->base_stack = ((t_struct_tcb*) structRecibido)->base_stack;
	tcb->cola = ((t_struct_tcb*) structRecibido)->cola;
	tcb->cursor_stack = ((t_struct_tcb*) structRecibido)->cursor_stack;
	tcb->kernel_mode = ((t_struct_tcb*) structRecibido)->kernel_mode;
	tcb->pid = ((t_struct_tcb*) structRecibido)->pid;
	tcb->puntero_instruccion = ((t_struct_tcb*) structRecibido)->puntero_instruccion;
	tcb->registros[0] = ((t_struct_tcb*) structRecibido)->registros[0];
	tcb->registros[1] = ((t_struct_tcb*) structRecibido)->registros[1];
	tcb->registros[2] = ((t_struct_tcb*) structRecibido)->registros[2];
	tcb->registros[3] = ((t_struct_tcb*) structRecibido)->registros[3];
	tcb->registros[4] = ((t_struct_tcb*) structRecibido)->registros[4];
	tcb->segmento_codigo = ((t_struct_tcb*) structRecibido)->segmento_codigo;
	tcb->segmento_codigo_size = ((t_struct_tcb*) structRecibido)->segmento_codigo_size;
	tcb->tid = ((t_struct_tcb*) structRecibido)->tid;
}

void handler_numeros_cpu(int32_t numero_cpu, int sockCPU){
	switch(numero_cpu){
	case D_STRUCT_INNN:
		//pido numero por consola
		break;
	case D_STRUCT_INNC:
		//pido string por consola
		break;
	case D_STRUCT_ABORT:
		//abortar
		break;
	case D_STRUCT_PEDIR_TCB:
		//darle otro tcb a cpu, si tiene
		break;
	case D_STRUCT_TERMINO:
		//terminar tcb
		break;
	}
}

void handler_cpu(int sockCPU){
	t_hilo* tcb;
	uint32_t tid_llamador;
	uint32_t tid_a_esperar;
	uint32_t direccion_syscall;
	int32_t id_semaforo;
	int32_t numero_cpu;

	t_tipoEstructura tipoRecibido;
	t_tipoEstructura tipoRecibido2;
	void* structRecibido;
	void* structRecibido2;
	socket_recibir(sockCPU, &tipoRecibido, &structRecibido);

	//TODO: PONER LOGS!
	switch(tipoRecibido){
	case D_STRUCT_INTE:
		//Recibo la direccion
		direccion_syscall = ((t_struct_direccion*) structRecibido)->numero;
		//otro socket para el tcb
		socket_recibir(sockCPU, &tipoRecibido2, &structRecibido2);

		if(tipoRecibido == D_STRUCT_TCB){
			copiar_structRecibido_a_tcb(tcb, structRecibido);
		} else {
			printf("No llego el TCB para la operacion INTE\n");
		}

		break;
	case D_STRUCT_NUMERO:

		numero_cpu = ((t_struct_numero*) structRecibido)->numero;
		handler_numeros_cpu(numero_cpu, sockCPU);
		break;
	case D_STRUCT_TCB_CREA:

		copiar_structRecibido_a_tcb(tcb, structRecibido);

		break;
	case D_STRUCT_JOIN:

		tid_llamador = ((t_struct_join*) structRecibido)->tid_llamador;
		tid_a_esperar = ((t_struct_join*) structRecibido)->tid_a_esperar;

		break;
	case D_STRUCT_BLOCK:
		//Recibo id semaforo
		id_semaforo = ((t_struct_numero*) structRecibido)->numero;

		//otro socket para el tcb
		socket_recibir(sockCPU, &tipoRecibido2, &structRecibido2);

		if(tipoRecibido2 == D_STRUCT_TCB){
			copiar_structRecibido_a_tcb(tcb, structRecibido);
		} else {
			printf("No se recibio el TCB para la operacion BLOCK\n");
		}

		break;
	case D_STRUCT_WAKE:
		//Recibo id semaforo
		id_semaforo = ((t_struct_numero*) structRecibido)->numero;
		break;
	case D_STRUCT_TCB_QUANTUM:

		copiar_structRecibido_a_tcb(tcb, structRecibido);

		break;
	}
}

