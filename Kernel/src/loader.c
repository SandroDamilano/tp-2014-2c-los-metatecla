/*
 ============================================================================
 LOADER 	 	 	 	.
 ============================================================================
 *
 */

#include "loader.h"

uint32_t tamanio_stack;

/*
// Variables para sockets Servidor
fd_set master;		// master file descriptor list
fd_set read_fds;	// temp file descriptor list for select()
int fdmax;			// maximum file descriptor number
int listener;		// listening socket descriptor
int newfd;			// newly accept()ed socket descriptor
struct sockaddr_storage remoteaddr;	// client address
socklen_t addrlen;
int nbytes;
int yes=1;
char remoteIP[INET6_ADDRSTRLEN];
int i, j, rv;
struct addrinfo hints, *ai, *p;

//----------------------------------------------------

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int crear_listener(const char *ip, const char *puerto)
{
	FD_ZERO(&master);	// clear the master and temp sets
	FD_ZERO(&read_fds);

	memset(&hints, 0, sizeof(hints));	// get us a socket and bind it
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(ip, puerto, &hints, &ai)) != 0)	{
		fprintf(stderr, "[Kernel-LDR] Error on selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}

	for(p = ai; p != NULL; p = p->ai_next)
	{
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)	{
			close(listener);
			continue;
		}
		break;
	}

	if (p == NULL)	{	// if we got here, it means we didn't get bound
		fprintf(stderr, "[Kernel-LDR] Error on selectserver: Failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai);	// all done with this

	if (listen(listener, 10) == -1)	{
		perror("[Kernel-LDR]: Error on 'listen()' function.");
		exit(3);
	}
	// add the listener to the master set
	FD_SET(listener, &master);
	// keep track of the biggest file descriptor
	fdmax = listener; // so far, it's this one

	printf("[Kernel-LDR]: Hilo a la espera de Procesos...\n");
	return listener;
}

// --------------------------------------
// to RECEIVE

int handleFileLine(char* datos)
{
	t_struct_string *msg = deserializeStruct_string(datos, 0);
	char* dirty_file;
	char* file = "";

	dirty_file = msg->string;
	file = malloc(strlen(dirty_file));

	//Limpio el archivo ?
	int i,j = 0;
	for (i = 0; i <= strlen(dirty_file); i++){
		if (dirty_file[i] != '\t')
		{
			file[j] = dirty_file[i];
			//printf("El archivo va quedando...: %d -> [%c] \n",j, file[j]);	//only for debug purpose
			j++;
		}
	}

//	printf("El archivo finalmente quedo: %s \n", file);

	free(msg->string);
	free(msg);

	return 0;
}

//int handleMemoriaSuccess(char* datos)
//{
//	paq_res_SolicitudMemoria *deserializado = deserialize_struct_res_SolicitudMemoria(datos);
//
//	return deserializado->posicion;
//}

int handleMemoriaFail(char* datos)
{	//TODO: ver que hacer aca cuando la MSP no da memoria.

	return 0;
}

int analizar_paquete_loader(u_int32_t socket, char *paquete, t_tipoEstructura *op)
{
	int res; // Resultado de cada handler

	socket_recibir(socket, op, (void*)paquete);

	switch(*op) {
		case FILE_LINE:
			res = handleFileLine(paquete);
			break;
		case FILE_EOF:
			res = 0;
			break;
		case MEMORIA_MSP_SUCCESS:
//			res = handleMemoriaSuccess(paquete);
//			return res;
			break;
		case MEMORIA_MSP_FAIL:
			res = handleMemoriaFail(paquete);
			break;
		default:
			return -1;
			break;
	}

	// Se libera el mensaje  cuando ya no lo necesitamos
	if (paquete != NULL) {
		free (paquete);
		paquete = NULL;
	}
	return res;
}

// --------------------------------------
// to SEND


//stream_t* do_struct_solicitud_memoria(paq_SolicitudMemoria* datos, int *size)
//{
//	stream_t *serializado = serialize_struct_solicitud_memoria(datos,size);
//
//	return serializado;
//}
//
//stream_t *handleSolicitudMemoria(paq_SolicitudMemoria* datos, int *size)
//{
//	return do_struct_solicitud_memoria(datos, size);
//}

int preparar_paquete(u_int32_t socket, t_tipoEstructura op, void* estructura)
{
	t_stream *paquete = NULL;

	switch(op)
	{
		case HANDSHAKE_SUCCESS: case HANDSHAKE_FAIL:
			paquete = serializeStruct_string((t_struct_string*)estructura);
			break;
		case FILE_RECV_SUCCESS: case FILE_RECV_FAIL:
			paquete = serializeStruct_string((t_struct_string*)estructura);
			break;
		case END_PROGRAM:
			paquete = serializeStruct_string((t_struct_string*)estructura);
			break;
		case SOLICITAR_MEMORIA_MSP:
//			paquete = handleSolicitudMemoria((paq_SolicitudMemoria*)estructura, &tamanho);
			break;
		default:
			break;
	}

	socket_enviar(socket, op, paquete->data);

	free(paquete->data);
	free(paquete);

	return 0;
}

int recibir_BESO_file(u_int32_t socket)
{
	t_struct_string msg;
	t_tipoEstructura operacion;
	char *buffer = NULL;

	while (analizar_paquete_loader(socket, buffer, &operacion) == 0)
	{
		if (operacion == FILE_EOF) {
			break;
		}
	}

	msg.string = "[Kernel-LDR]: BESO file recibido correctamente.";
	log_info(logger,"BESO file recibido correctamente.");

	preparar_paquete(socket,FILE_RECV_SUCCESS, &msg);

	return 0;
}

uint32_t solicitarMemoria(u_int32_t pid, u_int32_t size, char* datos)
{
	uint32_t res;
	paq_SolicitudMemoria solicitud;
	t_tipoEstructura resultado;

//	printf("\n================\n pid [%d], size [%d], datos[%s]\n",pid,size,datos);	// only for debug purpose
	solicitud.id_proceso = pid;
	solicitud.size = size;
	solicitud.datos = datos;

	if((preparar_paquete(socket_MSP, SOLICITAR_MEMORIA_MSP, &solicitud)) != 0) {	// Envia los datos para solicitar memoria
		perror("[Kernel-LDR]: Error al enviar solicitud de memoria a la MSP.");
		return 1;
	}

	res = analizar_paquete_loader(socket_MSP, NULL, &resultado);

	if(resultado == MEMORIA_MSP_FAIL) {
		perror("[Kernel-LDR]: la MSP nego espacio en memoria.");
		return 1;
	}

	return res;
}

int atender_Proceso(uint32_t socket,uint32_t tamanio_stack)
{
	*//*	PASOS
	* 1- Recibir BESO file
	* 2- Asignar nuevo PID
	* 3- Asignar nuevo TID
	* 3- Solicitar memoria a la MSP
	* 4- Crear TCB e Init del PC (IP)
	* 5- Encolar proceso (segun BBNP)
	*//*

	u_int32_t pid, dir_code_segment, dir_stack_segment, long_beso_file;
	char* beso_file = "";

	//1- Recibir BESO file

	if(!recibir_BESO_file(socket)) {
		msg.string = "[Kernel-LDR]: Fallo la recepcion del BESO code. Se cierra la conexion.";
		preparar_paquete(socket, FILE_RECV_FAIL, &msg);
		free(msg.string);
		FD_CLR(socket, &master);
		close(socket);
		return 1;
	}
	long_beso_file = strlen(beso_file);

	//2- Asignar nuevo PID
	pid = process_getpid();	// returns the process ID of the calling process. These functions are always successful.

	//2- Asignar TID
//	u_int32_t tid = process_get_thread_id();	// In single threaded code both PID & TID are same.

	//3- Solicitar memoria a la MSP
	// TODO: Evaluar si hay necesidad de contar con un dictionary de procesos(programas)

		// CODE segment
	if((dir_code_segment = solicitarMemoria(pid, long_beso_file, beso_file)) < 0)	{
		msg.string = "[Kernel-LDR]: Fallo en la solicitud de memoria a la MSP. Se cierra la conexion.";
		preparar_paquete(socket, END_PROGRAM, &msg);
		free(msg.string);
		FD_CLR(socket, &master);
		close(socket);
		return 2;
	}

		// STACK segment
	if((dir_stack_segment = solicitarMemoria(pid, tamanio_stack, "")) < 0) {
		msg.string = "[Kernel-LDR]: Fallo en la solicitud de memoria a la UMV. Se cierra la conexion.";
		preparar_paquete(socket, END_PROGRAM, &msg);
		free(msg.string);
		FD_CLR(socket, &master);
		close(socket);
		return 2;
	}

	//4- Crear TCB e Init del PC (IP)
	t_hilo *tcb = crear_TCB(pid, dir_code_segment, dir_stack_segment, long_beso_file);

	//5- Encolar proceso (segun BBNP)
	poner_en_new(tcb);

	return 0;
}

void check_exit()
{
	//Terminar Programa (mensaje a traves del file descriptor)
	//Cierra conexion socket
	//TODO Borrar todos los segmentos del TCB en MSP
	//Eliminar file descriptor
	//Remove and destroy

	t_struct_string msg;
	t_hilo* elemento;
	u_int32_t *socket;

	if(!queue_is_empty(cola_exit))
	{
//		pthread_mutex_lock(&mutex_cola_exit);
		elemento = queue_pop(cola_exit);
		msg.string = "[Kernel-PLP]: Fin de instruccion alcanzado. Hasta la vista Programa!";
		preparar_paquete(*socket, END_PROGRAM, &msg);
		close(*socket);
		FD_CLR(*socket, &master);
//		pthread_mutex_unlock(&mutex_cola_exit);

		free(elemento);
	}
}

void* main_LOADER(void* parametros) {
*//* El LOADER siempre acepta procesos, JAMAS los rechaza. Otra historia es si despues no se pueden alojar en la MSP.
 * CICLO
 *
 * 1- PONER SOCKETS A LA ESCUCHA DE PROGRAMAS
 * 2- ATENDER PROCESO
 * 	2.1- Recibir BESO file
 * 	2.2- Asignar nuevo PID
 * 	2.4- Solicitar memoria a la MSP
 * 	2.5- Crear TCB e Init del PC (IP)
 * 	2.6- Encolar proceso (segun BBNP)
*//*

	// parametros recibidos del Kernel
	logger = ((arg_LOADER *)parametros)->logger;
	char *ip_kernel = "127.0.0.1";	// harcodeo..de momento
	char *puerto_kernel = ((arg_LOADER *)parametros)->puerto_kernel;
//	char *ip_msp = ((arg_LOADER *)parametros)->ip_msp;
//	char *puerto_msp = ((arg_LOADER *)parametros)->puerto_msp;
	uint32_t tamanio_stack = ((arg_LOADER *)parametros)->tamanio_stack;

	log_trace(logger,"INICIO de registro de actividades del Hilo LOADER.");

	// creacion de estructuras
	cola_new = queue_create();
	cola_exit = queue_create();

	log_info(logger,"Se crean las estructuras necesarias del hilo.");

	int select_result;
	struct timeval tv; //Timeout del Select
	tv.tv_sec = DELAY_CHECK_NEW_READY_SEC;
	tv.tv_usec = DELAY_CHECK_NEW_READY_USEC;

	//2- Socket de escucha para Programas
	crear_listener(ip_kernel, puerto_kernel);

	for(;;)
	{
		read_fds = master; // copy it (copiamos el master al read_fds)
		while((select_result = select(fdmax+1, &read_fds, NULL, NULL, &tv)) <= 0)
		{
			if (select_result == -1) {
				perror("[Kernel-LDR]: Error on 'select()' function.");
				exit(4);
			} else
				if(select_result == 0) {
				check_exit();				//Se fija si hay algun programa para terminar en la cola de Exit.
			}
			tv.tv_sec = DELAY_CHECK_NEW_READY_SEC;
			tv.tv_usec = DELAY_CHECK_NEW_READY_USEC;
			read_fds = master;
		}

		// run through the existing connections looking for data to read
		for(i = 0; i <= fdmax; i++)
		{
			if (FD_ISSET(i, &read_fds))
			{	// we got one!!
				if (i == listener)
				{	// handle new connections
					addrlen = sizeof remoteaddr;
					newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
					if (newfd == -1) {
						perror("[Kernel-LDR]: Error on 'accept()' function.");
						msg.string = "[Kernel-LDR] Handshake con Consola, sin exito.";
						preparar_paquete(newfd, HANDSHAKE_FAIL, &msg);
					} else {
						FD_SET(newfd, &master); // add to master set
						if (newfd > fdmax)
						{	// keep track of the max
							fdmax = newfd;
						}
						conexion_consola(newfd);
//						printf("[Kernel-LDR]: Nuevo proceso en host [%s] y socket [%d]\n",
//								inet_ntop(
//										remoteaddr.ss_family,
//										get_in_addr((struct sockaddr*)&remoteaddr),
//										remoteIP, INET6_ADDRSTRLEN
//										),
//										newfd);
						msg.string = "[Kernel-LDR] Handshake con Consola, exitoso!";
						preparar_paquete(newfd, HANDSHAKE_SUCCESS, &msg);
					}
				}
				else
				{	// handle data from a client
					atender_Proceso(i,tamanio_stack);
				} // END handle data from client
			} // END got new incoming connection
		} // END looping through file descriptors
	} // END for(;;)--and you thought it would never end!

	log_trace(logger,"FIN de registro de actividades del Hilo LOADER.");
	return 0;
}*/

void* main_LOADER(void* parametros) {

	//Se atienden una a una cada solicitud de las consolas
	while(1){
		int i;
		fd_set read_consolas;
		FD_ZERO(&read_consolas);

		pthread_mutex_lock(&mutex_master_consolas);
		read_consolas = master_consolas;
		pthread_mutex_unlock(&mutex_master_consolas);

		if (select(consolas_fdmax+1, &read_consolas, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(1);
		}
		for(i=0; i<=consolas_fdmax; i++){
			if(FD_ISSET(i, &read_consolas)){
				handler_consola(i);
			}
		}
	}

	return 0;
}

void handler_consola(int sock_consola){

	char* codigo;
	char* texto;
	int tamanio;
	int numero;

	t_tipoEstructura tipoRecibido;
	void* structRecibido;

	if(socket_recibir(sock_consola, &tipoRecibido, &structRecibido)==-1){
		//La Consola cerró la conexión
		printf("Se perdió la comunicación con la Consola: %d\n", sock_consola);

		//TODO: Averiguar qué hacer con el proceso que se estaba ejecutando
		//Saco el nodo correspondiente de la estructura que relaciona el socket de la consola con el proceso
		eliminar_consola(sock_consola);

		//Cierro el socket y lo saco del FD maestro
		close(sock_consola);
		pthread_mutex_lock(&mutex_master_consolas);
		FD_CLR(sock_consola, &master_consolas);
		pthread_mutex_unlock(&mutex_master_consolas);
	}

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

		while(1){

		}

		break;

	case D_STRUCT_INNC:
		//Recibo un imput de una cadena de texto

		tamanio = strlen(((t_struct_string*)structRecibido)->string);
		texto = malloc(tamanio);
		memcpy(texto, ((t_struct_string*)structRecibido)->string, tamanio);
		//TODO

		break;

	case D_STRUCT_INNN:
		//Recibo un imput de un número

		numero = ((t_struct_numero*)structRecibido)->numero;
		//TODO

		break;
	}

	free(structRecibido);

}

int crear_nuevo_tcb(char* codigo, int tamanio, int sock_consola){
	void * structRecibido;
	t_tipoEstructura tipoStruct;

	uint32_t dir_codigo;
	uint32_t dir_stack;

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
		dir_codigo = ((t_struct_numero *) structRecibido)->numero;
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
		dir_stack = ((t_struct_numero *) structRecibido)->numero;
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

	while(1){

	}

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

