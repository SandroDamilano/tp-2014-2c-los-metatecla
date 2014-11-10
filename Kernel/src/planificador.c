/*
 * planificador.c
 *
 *  Created on: 06/10/2014
 *      Author: utnso
 */

#include "planificador.h"


void* main_PLANIFICADOR(arg_PLANIFICADOR* parametros)
{
	inicializar_ready_block();
	inicializar_semaforos_colas();

	pthread_t thr_consumidor_new, thr_parca;
	pthread_create(&thr_consumidor_new, NULL, (void*)&poner_new_a_ready, NULL);
	pthread_create(&thr_parca, NULL, (void*)&terminar_TCBs, NULL);

	printf("voy a entrar al boot\n");
	boot(parametros->syscalls_path);

	while(1){
	/*
	 * 1 - Mandar a ejecutar TCBs a CPUs que lo hayan requerido
	 * 2 - Mandarle a los CPUs imputs de la Consola
	 * 3 - Atender nuevas solicitudes de CPUs
	 */
		atender_solicitudes_pendientes();
		atender_cpus();

	}

	pthread_join(thr_consumidor_new, NULL);
	pthread_join(thr_parca, NULL);

return 0;
}

bool hay_en_ready(){
	pthread_mutex_lock(&mutex_ready);
	bool resp = !list_is_empty(cola_ready);
	pthread_mutex_unlock(&mutex_ready);
	return resp; //Este valor puede quedar desactualizado, pero no importa
}

void atender_solicitudes_pendientes(){
	while((!list_is_empty(solicitudes_tcb)) && (hay_en_ready())){
		t_hilo* tcb = obtener_tcb_a_ejecutar();
		int* sockCPU = list_remove(solicitudes_tcb, 0);
		mandar_a_ejecutar(tcb, *sockCPU);
	}
}

void atender_cpus(){
	int i;
	fd_set read_cpus;
	FD_ZERO(&read_cpus);

	//Tiempo que se queda bloqueado esperando conexiones. En este caso no se va a esperar.
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	pthread_mutex_lock(&mutex_master_cpus);
	read_cpus = master_cpus;
	pthread_mutex_unlock(&mutex_master_cpus);

	if (select(cpus_fdmax+1, &read_cpus, NULL, NULL, &tv) == -1) {
		perror("select");
		exit(1);
	}
	for(i=0; i<=cpus_fdmax; i++){
		if(FD_ISSET(i, &read_cpus)){
			handler_cpu(i);
		}
	}
}

/*********************** HILOS DEDICADOS ******************************/

//Este hilo se queda haciendo loop hasta que termine la ejecución
void poner_new_a_ready(){
	while(1){
		t_hilo* tcb;// = malloc(sizeof(t_hilo));
		sacar_de_new(tcb);
		encolar_en_ready(tcb);
	}
};

/* Este hilo se encarga de:
 * 		- Liberar todos los recursos del hilo
 * 		- Mandar a EXIT otros hilos que deban terminarse
 * 		- Notificar a la consola correspondiente
 */
void terminar_TCBs(){
	while(1){
		t_data_nodo_exit* data = malloc(sizeof(t_data_nodo_exit));
		sacar_de_exit(data);

		switch(data->fin){
		case TERMINAR:
			if(es_padre(data->tcb)==1){
				// Hay que finalizar todos los hijos
				escribir_consola(data->tcb->pid, "El proceso ha finalizado");
				terminar_proceso(data->tcb);
			}else{
				// Se finaliza sólo este hilo
				terminar_hilo(data->tcb);
			};
			break;

		case ABORTAR:
			// Hay que finalizar el proceso completo
			escribir_consola(data->tcb->pid, "Ha ocurrido una excepción");
			terminar_proceso(data->tcb);
			break;
		};
	};
}

/***************** FUNCIONES DEL HILO TERMINAR_TCBS() *****************/

bool es_padre(t_hilo* tcb){
	tid_de_consola = tcb->tid;
	pthread_mutex_lock(&mutex_consolas);
	t_data_nodo_consolas* data = list_find(consolas, (void*)es_el_tid_consola);
	pthread_mutex_unlock(&mutex_consolas);
	if(data!=NULL){
		return true;
	};
	return false;
}

void escribir_consola(uint32_t pid, char* mensaje){
	pid_de_consola = pid;
	pthread_mutex_lock(&mutex_consolas);
	t_data_nodo_consolas* data = list_find(consolas, (void*)es_el_pid_consola);
	pthread_mutex_unlock(&mutex_consolas);
	int socket = data->socket;
	t_struct_string* impresion = malloc(sizeof(t_struct_string));
	impresion->string = mensaje;
	socket_enviar(socket, ENVIAR_IMPRIMIR_TEXTO, impresion);
	free(impresion);
}

void terminar_proceso(t_hilo* tcb){
	uint32_t pid = tcb->pid;
	eliminar_ready(pid);
	eliminar_block(pid);
	eliminar_exec(pid);
	sacar_de_consolas(pid);
	terminar_hilo(tcb);
}

void terminar_hilo(t_hilo* tcb){
	liberar_memoria(tcb);
}

void liberar_memoria(t_hilo* tcb){
	//Libero segmento de codigo
	t_struct_free* free_segmento = malloc(sizeof(t_struct_free));
	free_segmento->PID = tcb->pid;
	free_segmento->direccion_base = tcb->segmento_codigo;

	socket_enviar(sockfd_cte, D_STRUCT_FREE, free_segmento);

	//Libero segmento de stack
	free_segmento->direccion_base = tcb->base_stack;
	socket_enviar(sockfd_cte, D_STRUCT_FREE, free_segmento);

	free(free_segmento);
	free(tcb);
}

void eliminar_ready(uint32_t pid){
	pid_a_eliminar = pid;
	pthread_mutex_lock(&mutex_ready);
	t_hilo* tcb = list_remove_by_condition(cola_ready, (void*)es_el_pid_ready);
	pthread_mutex_unlock(&mutex_ready);
	while(tcb!=NULL){
		mandar_a_exit(tcb, TERMINAR);
		pthread_mutex_lock(&mutex_ready);
		tcb = list_remove_by_condition(cola_ready, (void*)es_el_pid_ready);
		pthread_mutex_unlock(&mutex_ready);
	};
}

void eliminar_block(uint32_t pid){
	pid_a_eliminar = pid;
	pthread_mutex_lock(&mutex_block);
	t_data_nodo_block* data = list_remove_by_condition(cola_block, (void*)es_el_pid_block);
	pthread_mutex_unlock(&mutex_block);
	while(data!=NULL){
		mandar_a_exit(data->tcb, TERMINAR);
		free(data);
		pthread_mutex_lock(&mutex_block);
		data = list_remove_by_condition(cola_block, (void*)es_el_pid_block);
		pthread_mutex_unlock(&mutex_block);
	};
}

void eliminar_exec(uint32_t pid){
	pid_a_eliminar = pid;
	pthread_mutex_lock(&mutex_exec);
	t_data_nodo_exec* data = list_remove_by_condition(cola_exec, (void*)es_el_pid_exec);
	pthread_mutex_unlock(&mutex_exec);
	while(data!=NULL){
		//TODO decirle a la cpu que aborte su ejecución (¿Eliminar solicitud de atención?)
		mandar_a_exit(data->tcb, TERMINAR);
		free(data);
		pthread_mutex_lock(&mutex_exec);
		data = list_remove_by_condition(cola_exec, (void*)es_el_pid_exec);
		pthread_mutex_unlock(&mutex_exec);
	};
}

void sacar_de_consolas(uint32_t pid){
	pid_de_consola = pid;
	pthread_mutex_lock(&mutex_consolas);
	t_data_nodo_consolas* data = list_remove_by_condition(consolas, (void*)es_el_pid_consola);
	pthread_mutex_unlock(&mutex_consolas);
	free(data);
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

void push_exit(t_data_nodo_exit* data){
	queue_push(cola_exit, (void*)data);
}

void mandar_a_exit(t_hilo* tcb, t_fin fin){
	t_data_nodo_exit* data = malloc(sizeof(t_data_nodo_exit));
	data->fin = fin;
	data->tcb = tcb;
	pthread_mutex_lock(&mutex_exit);
	push_exit(data);
	pthread_mutex_unlock(&mutex_exit);
	sem_post(&sem_exit);
	data->tcb->cola = EXIT;
}

t_hilo* obtener_tcb_a_ejecutar(){
	pthread_mutex_lock(&mutex_ready);
	t_hilo* tcb = list_remove(cola_ready, 0);
	pthread_mutex_unlock(&mutex_ready);
	return tcb;
};

t_hilo* obtener_tcb_de_cpu(int sock_cpu){
	sockCPU_a_buscar = sock_cpu;
	t_data_nodo_exec* data;
	t_hilo* tcb;
	pthread_mutex_lock(&mutex_exec);
	data = list_remove_by_condition(cola_exec, (void*)es_el_tcbCPU);
	pthread_mutex_unlock(&mutex_exec);
	if (data!=NULL){
		tcb = data->tcb;
	}else{
		printf("ERROR: No fue encontrado el CPU\n");
	};
	free(data);
	return tcb;
}

void agregar_a_exec(int sockCPU, t_hilo* tcb){
	t_data_nodo_exec* data = malloc(sizeof(t_data_nodo_exec));
	data->sock = sockCPU;
	data->tcb = tcb;
	pthread_mutex_lock(&mutex_exec);
	list_add(cola_exec, data);
	pthread_mutex_unlock(&mutex_exec);
	tcb->cola = EXEC;
}

/********************************** BLOQUEAR ***************************************/

//Este bloquear es GENERAL. Para bloquear, usar los particulares.
void bloquear_tcb(t_hilo* tcb, t_evento evento, uint32_t parametro){
	t_data_nodo_block* data = malloc(sizeof(t_data_nodo_block));
	data->tcb = malloc(sizeof(t_hilo));
	data->tcb = tcb;
	data->evento = evento;
	data->parametro = parametro;
	pthread_mutex_lock(&mutex_block);
	list_add(cola_block, (void*)data);
	pthread_mutex_unlock(&mutex_block);
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

bool es_el_tcbCPU(t_data_nodo_exec* data){
	return (data->sock == sockCPU_a_buscar);
}

bool es_el_tid_consola(t_data_nodo_consolas* data){
	return (data->tid == tid_de_consola);
}

bool es_el_pid_consola(t_data_nodo_consolas* data){
	return (data->pid == pid_de_consola);
}

bool es_el_pid_ready(t_hilo* tcb){
	return(tcb->pid == pid_a_eliminar);
}

bool es_el_pid_block(t_data_nodo_block* data){
	return(data->tcb->pid == pid_a_eliminar);
}

bool es_el_pid_exec(t_data_nodo_exec* data){
	return(data->tcb->pid == pid_a_eliminar);
}

/******************************* DESBLOQUEAR ***************************************/

// Este desbloquear es GENERAL. Para desbloquear, hay que usar los particulares.
void desbloquear_proceso(t_evento evento, uint32_t parametro){
	t_data_nodo_block *data_desbloqueado;
	parametro_a_buscar = parametro;
	evento_a_buscar = evento;
	pthread_mutex_lock(&mutex_block);
	data_desbloqueado = list_remove_by_condition(cola_block,(void*)es_el_tcbBuscado);
	pthread_mutex_unlock(&mutex_block);
	if (data_desbloqueado != NULL){
		t_hilo * tcb_desbloqueado = data_desbloqueado->tcb;
		encolar_en_ready(tcb_desbloqueado);
	}
	free(data_desbloqueado);
}

t_hilo* desbloquear_tcbKernel(){
	pthread_mutex_lock(&mutex_block);
	t_data_nodo_block* data = list_remove_by_condition(cola_block, (void*)es_el_tcbKernel);
	pthread_mutex_unlock(&mutex_block);
	t_hilo* tcbKM;
	if (data != NULL){
		tcbKM = data->tcb;
	}
	free(data);
	return tcbKM;
};

t_hilo* desbloquear_tcbSystcall(uint32_t tid){
	tid_a_buscar = tid;
	pthread_mutex_lock(&mutex_block);
	t_data_nodo_block* data = list_remove_by_condition(cola_block, (void*)es_el_tcbSystcall);
	pthread_mutex_unlock(&mutex_block);
	t_hilo* tcb;
	if (data != NULL){
		tcb = data->tcb;
	}
	free(data);
	return tcb;
}

void desbloquear_por_join(uint32_t tid){
	desbloquear_proceso(JOIN, tid);
}

void desbloquear_por_semaforo(uint32_t sem){
	desbloquear_proceso(SEM, sem);
}

t_data_nodo_block* desbloquear_alguno_por_systcall(t_hilo* tcb_kernel){
	pthread_mutex_lock(&mutex_block);
	t_data_nodo_block* data = list_remove_by_condition(cola_block, (void*)esta_por_systcall);
	pthread_mutex_unlock(&mutex_block);
	return data;
}

/********************************* INICIO *******************************************/

void inicializar_ready_block(){
	cola_ready = list_create();
	cola_block = list_create();
	cola_exec = list_create();
	solicitudes_tcb = list_create();
}

void inicializar_semaforos_colas(){
	pthread_mutex_init(&mutex_ready, NULL);
	pthread_mutex_init(&mutex_block, NULL);
	pthread_mutex_init(&mutex_exec, NULL);
};

void boot(char* systcalls_path){
	void * structRecibido;
	t_tipoEstructura tipoStruct;

	// Usar el mismo socket para la msp del main.c
	int tamanio_codigo;

	//Levanta archivo de syst calls
	printf("Levanto arch de syscalls\n");
	FILE* syscalls_file = abrir_archivo(systcalls_path, logger, &mutex_log);
	tamanio_codigo = calcular_tamanio_archivo(syscalls_file);
	char* syscalls_code = leer_archivo(syscalls_file, tamanio_codigo);
	printf("Tamaño de arch %d\n", tamanio_codigo);

	//Mando identificacion kernel
	printf("Digo que soy kernel\n");
	uint32_t senial = ES_KERNEL;
	socket_enviarSignal(sockfd_cte, senial);

	//Creo segmento para las syscalls
	printf("Creo seg de codigo de syscalls\n");
	t_struct_malloc* crear_seg = malloc(sizeof(t_struct_malloc));
	crear_seg->PID = 0; //Pid del tcb kernel
	crear_seg->tamano_segmento = tamanio_codigo;
	int resultado = socket_enviar(sockfd_cte, D_STRUCT_MALC, crear_seg);
	if(resultado != 1){
		printf("No se pudo crear segmento de codigo para las syscalls\n");
	}
	free(crear_seg);

	//Recibo direccion del nuevo segmento de codigo
	printf("Recibo direccion del seg de codigo\n");
	socket_recibir(sockfd_cte, &tipoStruct, &structRecibido);
	if(tipoStruct == D_STRUCT_NUMERO ){
		direccion_codigo_syscalls = ((t_struct_numero *) structRecibido)->numero;
		printf("Recibi direccion %d\n", direccion_codigo_syscalls);
	} else {
		printf("No se recibio la direccion del segmento de codigo de las syscalls\n");
	}

	//Mando identificacion kernel
	printf("Digo que soy kernel\n");
	socket_enviarSignal(sockfd_cte, senial);

	//Creo segmento de stack para las syscalls
	printf("Creo stack de syscalls\n");
	crear_seg = malloc(sizeof(t_struct_malloc));
	crear_seg->PID = 0; //Pid del tcb kernel
	printf("tamanio De stack %d\n", tamanio_stack);
	crear_seg->tamano_segmento = tamanio_stack;
	resultado = socket_enviar(sockfd_cte, D_STRUCT_MALC, crear_seg);
	if(resultado != 1){
		printf("No se pudo crear segmento de stack para las syscalls\n");
	}
	free(crear_seg);

	//Recibo direccion del nuevo segmento de stack
	socket_recibir(sockfd_cte, &tipoStruct, &structRecibido);
	if(tipoStruct == D_STRUCT_NUMERO ){
		direccion_stack_syscalls = ((t_struct_numero *) structRecibido)->numero;
		printf("Se recibio la direcc del stack %d\n", direccion_stack_syscalls);
	} else {
		printf("No se recibio la direccion del segmento de stack de las syscalls\n");
	}

	//Mando identificacion kernel
	printf("Digo que soy kernel\n");
	socket_enviarSignal(sockfd_cte, senial);

	//Manda codigo de syscalls a la MSP
	printf("Mando codigo a MSP\n");
	t_struct_env_bytes* paquete_syscalls = malloc(sizeof(t_struct_env_bytes));
	paquete_syscalls->buffer = syscalls_code;
	paquete_syscalls->tamanio = tamanio_codigo;
	paquete_syscalls->base = direccion_codigo_syscalls;
	paquete_syscalls->PID = 0; //Pid del tcb kernel

	printf("voy a enviar pid %d, base %d, tamanio %d y un buffer\n", paquete_syscalls->PID, paquete_syscalls->base, paquete_syscalls->tamanio);

	socket_enviar(sockfd_cte, D_STRUCT_ENV_BYTES, paquete_syscalls);//sockMSP
	free(paquete_syscalls);
	free(structRecibido);

	socket_recibir(sockfd_cte, &tipoStruct, &structRecibido);//sockMSP
	if (tipoStruct != D_STRUCT_RESPUESTA_MSP) {
		printf("No se ha recibido correctamente direccion del codigo y direccion de stack\n");
	}

	void *datos_recibidos = malloc(2*sizeof(int32_t)); //direccion_codigo + direccion_stack
	datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

	memcpy(&direccion_codigo_syscalls,datos_recibidos,sizeof(uint32_t));
	memcpy(&direccion_stack_syscalls,datos_recibidos + sizeof(uint32_t),sizeof(uint32_t));

	free(datos_recibidos);
	free(structRecibido);

	//Crea hilo en modo kernel y lo encola en bloqueados
	printf("Encolo al tcb kernel\n");
	t_hilo *tcb_kernel = crear_TCB(0, direccion_codigo_syscalls, direccion_stack_syscalls, tamanio_codigo);
	tcb_kernel->kernel_mode = true;
	bloquear_tcbKernel(tcb_kernel);
}


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
//	printf("está atendiendo al tid: %d\n", tcb->tid);
	if (tcb_kernel != NULL){
		copiar_tcb(tcb, tcb_kernel);
		tcb_kernel->puntero_instruccion = dir_systcall;
		tcb_kernel->segmento_codigo = direccion_codigo_syscalls; //FIXME: FIJARSE QUE ESTO ESTE BIEN
		tcb_kernel->base_stack = direccion_stack_syscalls;
		tcb_kernel->pid = 0; //Pid de kernel
		encolar_en_ready(tcb_kernel);
	}
	//Avisarle a la cpu que pida otro proceso para ejecutar
};

bool esta_por_systcall(t_data_nodo_block* data){
	return (data->evento == SYSTCALL);
}

void atender_otra_systcall_si_hay(t_hilo* tcb_kernel){
	t_data_nodo_block* data_otro_tcb = desbloquear_alguno_por_systcall(tcb_kernel);

	if (data_otro_tcb != NULL){
		atender_systcall(data_otro_tcb->tcb, data_otro_tcb->parametro);
	};
}

void retornar_de_systcall(t_hilo* tcb_kernel, t_fin fin){
	t_hilo* tcb = desbloquear_tcbSystcall(tcb_kernel->tid);
	int i;

	if (tcb!=NULL){
		switch(fin){
		case TERMINAR:
			for(i=0; i<=4; i++){
				tcb->registros[i] = tcb_kernel->registros[i];
			}
			encolar_en_ready(tcb);
			break;

		case ABORTAR:
			mandar_a_exit(tcb, fin);
		}
	}
	bloquear_tcbKernel(tcb_kernel);

	atender_otra_systcall_si_hay(tcb_kernel);
}

void crear_nuevo_hilo(t_hilo* tcb_padre){
	//TODO Averiguar qué hay que poner en el nuevo TCB hijo
}

/******************************** HANDLER CPU *****************************************/

void mandar_a_ejecutar(t_hilo* tcb, int sockCPU){
	t_struct_tcb* paquete_tcb = malloc(sizeof(t_struct_tcb));
	copiar_tcb_a_structTcb(tcb, paquete_tcb);
	socket_enviar(sockCPU, D_STRUCT_TCB, paquete_tcb);
	free(paquete_tcb);
	agregar_a_exec(sockCPU, tcb);
}

void handler_numeros_cpu(int32_t numero_cpu, int sockCPU){
	t_hilo* tcb;

	switch(numero_cpu){
	case D_STRUCT_INNN:
		//TODO
		//pido numero por consola
		//Mando el numero obtenido con la señal D_STRUCT_INNN a cpu
		break;
	case D_STRUCT_ABORT:

		tcb = obtener_tcb_de_cpu(sockCPU);

		//Verifico si es el de Kernel
		if(tcb->kernel_mode == true){
			//TODO Consultar: si es el de Kernel, aborto sólo el proceso que hizo a la systcall?
			retornar_de_systcall(tcb, ABORTAR);
		}else{
			mandar_a_exit(tcb, ABORTAR);
		}

		break;

	case D_STRUCT_PEDIR_TCB:
		printf("me pidieron TCB\n");
		//darle otro tcb a cpu, si tiene
		/*tcb = obtener_tcb_a_ejecutar();

		if (tcb!=NULL){
			//Había un tcb en ready, entonces se lo mando
			mandar_a_ejecutar(tcb, sockCPU);
		}else{
			//No hay ninguno en ready, por lo que guardo la solicitud para atenderla después
			list_add(solicitudes_tcb, (void*)&sockCPU);
		}*/
		// PARA TESTEAR
		tcb = malloc(sizeof(t_hilo));
		tcb->segmento_codigo = 0;
		tcb->puntero_instruccion = 0;
		tcb->pid = 0;
		tcb->tid = 0;
		tcb->kernel_mode = true;
		tcb->registros[0] = 0;
		tcb->registros[1] = 0;
		tcb->registros[2] = 0;
		tcb->registros[3] = 0;
		tcb->registros[4] = 0;

		t_struct_tcb* paquete_tcb = malloc(sizeof(t_struct_tcb));
		copiar_tcb_a_structTcb(tcb, paquete_tcb);
		socket_enviar(sockCPU, D_STRUCT_TCB, paquete_tcb);
		printf("envie tcb\n");
		free(paquete_tcb);

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
	int32_t numero_consola;
	uint32_t maximo_caracteres;
	char* cadena_consola;

	t_tipoEstructura tipoRecibido;
	t_tipoEstructura tipoRecibido2;
	void* structRecibido;
	void* structRecibido2;

	if(socket_recibir(sockCPU, &tipoRecibido, &structRecibido)==-1){
		//La CPU cerró la conexión
		printf("Se perdió la comunicación con la CPU: %d\n", sockCPU);
		t_hilo* tcb = obtener_tcb_de_cpu(sockCPU);
		mandar_a_exit(tcb, ABORTAR);
		close(sockCPU);
		pthread_mutex_lock(&mutex_master_cpus);
		FD_CLR(sockCPU, &master_cpus);
		pthread_mutex_unlock(&mutex_master_cpus);
	}

	//TODO: PONER LOGS!
	switch(tipoRecibido){
	case D_STRUCT_INTE:
		//Recibo la direccion
		direccion_syscall = ((t_struct_direccion*) structRecibido)->numero;
		//otro socket para el tcb
		socket_recibir(sockCPU, &tipoRecibido2, &structRecibido2);

		if(tipoRecibido == D_STRUCT_TCB){
			copiar_structRecibido_a_tcb(tcb, structRecibido);//Chupala toga
		} else {
			printf("No llegó el TCB para la operacion INTE\n");
		}

		obtener_tcb_de_cpu(sockCPU);
		atender_systcall(tcb, direccion_syscall);

		break;
	case D_STRUCT_NUMERO:

		numero_cpu = ((t_struct_numero*) structRecibido)->numero;
		handler_numeros_cpu(numero_cpu, sockCPU);
		break;
	case D_STRUCT_INNC:

		maximo_caracteres = ((t_struct_numero *) structRecibido)->numero;
		//TODO pido string por consola con el maximo de caracteres recibido

		break;
	case D_STRUCT_TCB_CREA:

		copiar_structRecibido_a_tcb(tcb, structRecibido);
		// TODO invocar funcion crear_nuevo_hilo()

		break;
	case D_STRUCT_JOIN:
		tid_llamador = ((t_struct_join*) structRecibido)->tid_llamador;
		tid_a_esperar = ((t_struct_join*) structRecibido)->tid_a_esperar;

		tcb = desbloquear_tcbSystcall(tid_llamador);
		bloquear_tcbJoin(tcb, tid_a_esperar);

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

		obtener_tcb_de_cpu(sockCPU);

		// Revisar si está bien (idem WAKE)
		// bloquear_tcbSemaforo espera un uint32_t en vez de un int32_t
		bloquear_tcbSemaforo(tcb, id_semaforo);

		break;
	case D_STRUCT_WAKE:
		//Recibo id semaforo
		id_semaforo = ((t_struct_numero*) structRecibido)->numero;
		desbloquear_por_semaforo(id_semaforo);

		break;
	case D_STRUCT_TCB_QUANTUM:

		copiar_structRecibido_a_tcb(tcb, structRecibido);
		//Lo saco de EXEC
		obtener_tcb_de_cpu(sockCPU);
		encolar_en_ready(tcb);

		break;
	case D_STRUCT_TCB:

		copiar_structRecibido_a_tcb(tcb, structRecibido);
		//Lo saco de EXEC
		obtener_tcb_de_cpu(sockCPU);

		//Verifico si es el de Kernel
		if(tcb->kernel_mode == true){
			retornar_de_systcall(tcb, TERMINAR);
		}else{
			//terminar tcb
			mandar_a_exit(tcb, TERMINAR);
		}

		break;

	case D_STRUCT_OUTN:
		numero_consola = ((t_struct_numero*) structRecibido)->numero;
		//TODO Mandar ese numero a consola para ser mostrado
		break;
	case D_STRUCT_OUTC:
		cadena_consola = ((t_struct_string*) structRecibido)->string;
		//TODO Mandar esa cadena a consola para ser mostrada
		break;
	}
	free(structRecibido);
}

