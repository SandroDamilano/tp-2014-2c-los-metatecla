/*
 ============================================================================
 LOADER 	 	 	 	.
 ============================================================================
 *
 */

#include "loader.h"

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
	t_struct_string *msg = undo_struct_mensaje(datos);
	char* dirty_file;
	char* file = "";

	dirty_file = msg->out;
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

	free(msg->out);
	free(msg);

	return 0;
}

int handleMemoriaSuccess(char* datos)
{
	paq_res_SolicitudMemoria *deserializado = deserialize_struct_res_SolicitudMemoria(datos);

	return deserializado->posicion;
}

int handleMemoriaFail(char* datos)
{	//TODO: ver que hacer aca cuando la MSP no da memoria.

	return 0;
}

int analizar_paquete(u_int32_t socket, char *paquete, t_operaciones *op)
{
	int res; // Resultado de cada handler

	recv_msg_from(socket, (int*)op, &paquete);

	switch(*op) {
		case FILE_LINE:
			res = handleFileLine(paquete);
			break;
		case FILE_EOF:
			res = 0;
			break;
		case MEMORIA_MSP_SUCCESS:
			res = handleMemoriaSuccess(paquete);
			return res;
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

stream_t* do_struct_mensaje(stream_t* datos, int* size)
{
	stream_t *serializado = serialize_struct_mensaje(datos, size);

	return serializado;
}

stream_t* do_struct_solicitud_memoria(paq_SolicitudMemoria* datos, int *size)
{
	stream_t *serializado = serialize_struct_solicitud_memoria(datos,size);

	return serializado;
}

stream_t *handleHandshake(stream_t* datos, int* size)
{
	return do_struct_mensaje(datos, size);
}

stream_t *handleFileRecvResult(stream_t *datos, int *size)
{
	return do_struct_mensaje(datos, size);
}

stream_t *handleEndProgram(stream_t *datos, int *size)
{
	 return do_struct_mensaje(datos, size);
}

stream_t *handleSolicitudMemoria(paq_SolicitudMemoria* datos, int *size)
{
	return do_struct_solicitud_memoria(datos, size);
}

int preparar_paquete(u_int32_t socket, t_operaciones op, void* estructura)
{
	int tamanho = 0;
	stream_t *paquete = NULL;

	switch(op)
	{
		case HANDSHAKE_SUCCESS: case HANDSHAKE_FAIL:
			paquete = handleHandshake((stream_t*)estructura, &tamanho);
			break;
		case FILE_RECV_SUCCESS: case FILE_RECV_FAIL:
			paquete = handleFileRecvResult((stream_t*)estructura, &tamanho);
			break;
		case END_PROGRAM:
			paquete = handleEndProgram((stream_t*)estructura, &tamanho);
			break;
		case SOLICITAR_MEMORIA_MSP:
			paquete = handleSolicitudMemoria((paq_SolicitudMemoria*)estructura, &tamanho);
			break;
		default:
			break;
	}

	send_msg_to(socket, op, paquete->data , tamanho);

	free(paquete->data);
	free(paquete);

	return 0;
}

int recibir_BESO_file(u_int32_t socket)
{
	mensaje msg;
	t_operaciones operacion;
	char *buffer = NULL;

	while (analizar_paquete(socket, buffer, &operacion) == 0)
	{
		if (operacion == FILE_EOF) {
			break;
		}
	}

	msg.out = "[Kernel-LDR]: BESO file recibido correctamente.";
	log_info(logger,"BESO file recibido correctamente.");

	preparar_paquete(socket,FILE_RECV_SUCCESS, &msg);

	return 0;
}

int solicitarMemoria(u_int32_t pid, u_int32_t size, char* datos)
{
	int res;
	paq_SolicitudMemoria solicitud;
	t_operaciones resultadoSolicitudMemoria;

	printf("\n================\n pid [%d], size [%d], datos[%s]\n",pid,size,datos);	// only for debug purpose
	solicitud.id_proceso = pid;
	solicitud.size = size;
	//Falta el malloc?
	solicitud.datos = datos;

	if((preparar_paquete(sockfd_cte, SOLICITAR_MEMORIA_MSP, &solicitud)) != 0) {	// Envia los datos para solicitar memoria
		perror("[Kernel-LDR]: Error al enviar solicitud de memoria a la MSP.");
		return 1;
	}

	res = analizar_paquete(sockfd_cte, bufferMSP, &resultadoSolicitudMemoria);

	if(resultadoSolicitudMemoria == MEMORIA_MSP_FAIL) {
		perror("[Kernel-LDR]: la MSP nego espacio en memoria.");
		return 1;
	}

	return res;
}

int atender_Proceso(uint32_t socket,uint32_t tamanio_stack)
{
	/*	PASOS
	* 1- Recibir BESO file
	* 2- Asignar nuevo PID
	* 3- Asignar nuevo TID
	* 3- Solicitar memoria a la MSP
	* 4- Crear TCB e Init del PC (IP)
	* 5- Encolar proceso (segun BBNP)
	*/

	u_int32_t pid_Proceso, long_beso_file;
	char* beso_file = "";

	//1- Recibir BESO file

	if(recibir_BESO_file(socket) != 0) {
		msg.out = "[Kernel-LDR]: Fallo la recepcion del BESO code. Se cierra la conexion.";
		preparar_paquete(socket, FILE_RECV_FAIL, &msg);
		free(msg.out);
		FD_CLR(socket, &master);
		close(socket);
		return 1;
	}

	//2- Asignar nuevo PID
	pid.identificador = process_getpid();	// returns the process ID of the calling process. These functions are always successful.

	//2- Asignar TID
	tid.identificador = process_get_thread_id();	// In single threaded code both PID & TID are same.

	//3- Solicitar memoria a la MSP
	// TODO: Evaluar si hay necesidad de contar con un dictionary de procesos(programas)
	pid_Proceso = pid.identificador;
	long_beso_file = strlen(beso_file);

		// CODE segment
	if((info_mem_MSP.segmento_codigo = solicitarMemoria(pid_Proceso, long_beso_file, beso_file)) < 0)	{
		msg.out = "[Kernel-LDR]: Fallo en la solicitud de memoria a la MSP. Se cierra la conexion.";
		preparar_paquete(socket, END_PROGRAM, &msg);
		free(msg.out);
		FD_CLR(socket, &master);
		close(socket);
		return 2;
	}

		// STACK segment
	if((info_mem_MSP.base_stack = solicitarMemoria(pid_Proceso, tamanio_stack, "")) < 0) {
		msg.out = "[Kernel-LDR]: Fallo en la solicitud de memoria a la UMV. Se cierra la conexion.";
		preparar_paquete(socket, END_PROGRAM, &msg);
		free(msg.out);
		FD_CLR(socket, &master);
		close(socket);
		return 2;
	}

	//4- Crear TCB e Init del PC (IP)
	t_hilo *crear_TCB(u_int32_t pid, u_int32_t tid, t_hilo *mem_result)

	//5- Encolar proceso (segun BBNP)

	return 0;
}

void* main_LOADER(void* parametros) {
/* El LOADER siempre acepta procesos, JAMAS los rechaza. Otra historia es si despues no se pueden alojar en la MSP.
 * CICLO
 *
 * 1- PONER SOCKETS A LA ESCUCHA DE PROGRAMAS
 * 2- ATENDER PROCESO
 * 	2.1- Recibir BESO file
 * 	2.2- Asignar nuevo PID
 * 	2.4- Solicitar memoria a la MSP
 * 	2.5- Crear TCB e Init del PC (IP)
 * 	2.6- Encolar proceso (segun BBNP)
*/

	// parametros recibidos del Kernel
	logger = ((arg_LOADER *)parametros)->logger;
//	ip_kernel = ((arg_LOADER *)parametros)->ipKernel;
	char *ip_kernel = "127.0.0.1";	// harcodeo..de momento
	puerto_kernel = ((arg_LOADER *)parametros)->puerto_kernel;
	ip_msp = ((arg_LOADER *)parametros)->ip_msp;
	puerto_msp = ((arg_LOADER *)parametros)->puerto_msp;
	tamanio_stack = ((arg_LOADER *)parametros)->tamanio_stack;

	log_trace(logger,"INICIO de registro de actividades del Hilo LOADER.");

	// creacion de estructuras
	cola_new = queue_create();
	cola_ready = queue_create();
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
						msg.out = "[Kernel-LDR] Handshake con Consola, exitoso!";
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
}


