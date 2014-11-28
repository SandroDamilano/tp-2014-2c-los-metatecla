/*
 ============================================================================
 LOADER 	 	 	 	.
 ============================================================================
 *
 */

#include "loader.h"

uint32_t tamanio_stack;

void* main_LOADER(void* parametros) {

	//Se atienden una a una cada solicitud de las consolas
	while(1){
		int i;
		fd_set read_consolas;
		FD_ZERO(&read_consolas);

		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		pthread_mutex_lock(&mutex_master_consolas);
		read_consolas = master_consolas;
		pthread_mutex_unlock(&mutex_master_consolas);

		if (select(consolas_fdmax+1, &read_consolas, NULL, NULL, &tv) == -1) {
			perror("select");
			exit(1);
		}
		for(i=0; i<=consolas_fdmax; i++){
			if(FD_ISSET(i, &read_consolas)){
				printf("Procedo a atender a la Consola %d\n", i);
				handler_consola(i);
			}
		}
	}

	return 0;
}

void handler_consola(int sock_consola){

	char* codigo;
	//char* texto;
	int tamanio;
	//int numero;
	int sockCPU;

	t_tipoEstructura tipoRecibido;
	void* structRecibido;

	if(socket_recibir(sock_consola, &tipoRecibido, &structRecibido)==-1){
		//La Consola cerró la conexión

		//TODO: Averiguar qué hacer con el proceso que se estaba ejecutando
		//Saco el nodo correspondiente de la estructura que relaciona el socket de la consola con el proceso
		eliminar_consola(sock_consola);

		//Cierro el socket y lo saco del FD maestro
		desconexion_consola(sock_consola);
		close(sock_consola);
		//pthread_mutex_lock(&mutex_master_consolas);
		FD_CLR(sock_consola, &master_consolas);
		//pthread_mutex_unlock(&mutex_master_consolas);
	}else{

	switch(tipoRecibido){
	case FILE_RECV_SUCCESS: //Habria que serializar como el struct de abajo. Por ahora queda asi porque me da paja
	case D_STRUCT_ENV_BYTES:
		//Recibo un script a ejecutar

		tamanio = ((t_struct_env_bytes*) structRecibido)->tamanio;
		codigo = malloc(tamanio);
		memcpy(codigo, ((t_struct_env_bytes*) structRecibido)->buffer, tamanio);


		//Creo el TCB, reservo y escribo sus segmentos, y lo encolo en NEW.
		//En caso de fallo, informo a la consola.
		if (crear_nuevo_tcb(codigo, tamanio, sock_consola) == 0){
			//imprimir_texto(sock_consola, "Falló al crear los segmentos.");
		}

		break;

	case D_STRUCT_STRING:
		//Recibo un imput de una cadena de texto

		//Así como viene, se lo mando a la CPU
		printf("Me llego una cadena\n");
		sockCPU = obtener_cpu_ejecutando_la_consola(sock_consola);
		socket_enviar(sockCPU, tipoRecibido, structRecibido);
		printf("Mande cadena a CPU %d\n", sockCPU);

		break;

	case D_STRUCT_INNN:
		//Recibo un imput de un número

		//Así como viene, se lo mando a la CPU
		sockCPU = obtener_cpu_ejecutando_la_consola(sock_consola);
		socket_enviar(sockCPU, tipoRecibido, structRecibido);

		break;
	}

	free(structRecibido);
	}

}

int crear_nuevo_tcb(char* codigo, int tamanio, int sock_consola){
	void * structRecibido;
	t_tipoEstructura tipoStruct;

	uint32_t dir_codigo;
	uint32_t dir_stack;

	int respuesta;

	//Mando identificación kernel
	/*uint32_t senial = ES_KERNEL;
	socket_enviarSignal(socket_MSP, senial);*/

	int pid = obtener_pid();

	// Pido a la MSP un segmento para el código
	t_struct_malloc* crear_seg = malloc(sizeof(t_struct_malloc));
	crear_seg->PID = pid;
	crear_seg->tamano_segmento = tamanio;
	int resultado = socket_enviar(socket_MSP, D_STRUCT_MALC, crear_seg);
	if(resultado != 1){
		printf("No se pudo crear segmento de codigo\n");
		return 0;
	}
	free(crear_seg);

	//Recibo direccion del nuevo segmento de código
	socket_recibir(socket_MSP, &tipoStruct, &structRecibido);
	if(tipoStruct == D_STRUCT_NUMERO ){
		respuesta = ((t_struct_numero *) structRecibido)->numero;
		if (respuesta != -1){
			dir_codigo = ((t_struct_numero *) structRecibido)->numero;
		}else{
			printf("No hay espacio suficiente en memoria para el código\n");
			return 0;
		}
	} else {
		printf("No se recibio la direccion del segmento de codigo del proceso\n");
		return 0;
	}

	//Pido a la MSP un segmento de stack
	crear_seg = malloc(sizeof(t_struct_malloc));
	crear_seg->PID = pid;
	crear_seg->tamano_segmento = tamanio_stack;
	resultado = socket_enviar(socket_MSP, D_STRUCT_MALC, crear_seg);
	if(resultado != 1){
		printf("No se pudo crear segmento de stack\n");
		//Si no logré crear el segmento de stack, destruyo el de código que creé antes
		destruir_seg_codigo(pid, dir_codigo);
		return 0;
	}
	free(crear_seg);

	//Recibo direccion del nuevo segmento de stack
	socket_recibir(socket_MSP, &tipoStruct, &structRecibido);
	if(tipoStruct == D_STRUCT_NUMERO ){
		respuesta = ((t_struct_numero *) structRecibido)->numero;
		if (respuesta != -1){
			dir_stack = ((t_struct_numero *) structRecibido)->numero;
		}else{
			printf("No hay espacio suficiente en memoria para el stack\n");
			//Si no logré crear el segmento de stack, destruyo el de código que creé antes
			destruir_seg_codigo(pid, dir_codigo);
			return 0;
		}
	} else {
		printf("No se recibio la direccion del segmento de stack\n");
		//Si no logré crear el segmento de stack, destruyo el de código que creé antes
		destruir_seg_codigo(pid, dir_codigo);
		return 0;
	}

	//Escribo el código en la MSP
	t_struct_env_bytes* paquete_codigo = malloc(sizeof(t_struct_env_bytes));
	paquete_codigo->buffer = malloc(tamanio);
	memcpy(paquete_codigo->buffer, codigo, tamanio);
	paquete_codigo->tamanio = tamanio;
	paquete_codigo->base = dir_codigo;
	paquete_codigo->PID = pid;

	socket_enviar(socket_MSP, D_STRUCT_ENV_BYTES, paquete_codigo);
	free(paquete_codigo->buffer);
	free(paquete_codigo);
	free(structRecibido);

	socket_recibir(socket_MSP, &tipoStruct, &structRecibido);
	if (tipoStruct != D_STRUCT_NUMERO) {
		if(((t_struct_numero*) structRecibido)->numero == 0){
			printf("Se escribio correctamente en memoria\n");
		}
	}

	//Creo el TCB, agrego otra entrada a la estructura consolas y encolo el TCB en new
	t_hilo* tcb = crear_TCB(pid, dir_codigo, dir_stack, tamanio);
	agregar_consola(pid, tcb->tid, sock_consola);
	poner_en_new(tcb);

	return 1;
}

void destruir_seg_codigo(uint32_t pid, uint32_t dir_codigo){
	t_struct_free* free_segmento = malloc(sizeof(t_struct_free));
	free_segmento->PID = pid;
	free_segmento->direccion_base = dir_codigo;

	socket_enviar(socket_MSP, D_STRUCT_FREE, free_segmento);

	free(free_segmento);
}

void agregar_consola(uint32_t pid, uint32_t tid, int sock){
	t_data_nodo_consolas* data = malloc(sizeof(t_data_nodo_consolas));
	data->pid = pid;
	data->socket = sock;
	data->tid = tid;
	pthread_mutex_lock(&mutex_consolas);
	list_add(consolas, data);
	pthread_mutex_unlock(&mutex_consolas);
}

void eliminar_consola(int sock){
	sock_a_eliminar = sock;
	pthread_mutex_lock(&mutex_consolas);
	t_data_nodo_consolas* data = list_remove_by_condition(consolas, (void*)es_la_consola);
	pthread_mutex_unlock(&mutex_consolas);
	free(data);
}

bool es_la_consola(t_data_nodo_consolas* data){
	return (data->socket == sock_a_eliminar);
}

//Sólo puede haber una solicitud de un imput por vez, y lo hace la consola que está ejecutando el TCB de kernel
int obtener_cpu_ejecutando_la_consola(int sock_consola){
	pthread_mutex_lock(&mutex_exec);
	t_data_nodo_exec* data = list_find(cola_exec, (void*)tiene_al_tcbKernel);
	pthread_mutex_unlock(&mutex_exec);
	return data->sock;
}

bool tiene_al_tcbKernel(t_data_nodo_exec* data){
	return (data->tcb->kernel_mode == 1);
}

