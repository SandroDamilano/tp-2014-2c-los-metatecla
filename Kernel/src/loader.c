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

// Variables para socket Cliente MSP
int sockfd_cte, numbytes_clt;
struct addrinfo hints_clt, *servinfo_clt, *p_clt;
int rv_clt;
char s_clt[INET6_ADDRSTRLEN];

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

int atender_Proceso(uint32_t socket,uint32_t tamanio_stack)
{
	/*	PASOS
	* 2- Recibir BESO file
	* 4- Solicitar memoria a la UMV
	* 5- Crear TCB
	* 6- Encolar proceso.
	*/

	return 0;
}

void* main_LOADER(void* parametros)
{
/* El LOADER siempre acepta procesos.
 * TODO los programas nuevos se rechazan SOLAMENTE cuando falla un intento de reservarle un espacio de memoria.
 * CICLO
 *
 * 1- CONECTAR A MSP
 * 2- PONER SOCKETS A LA ESCUCHA DE PROGRAMAS
 * 3- ATENDER PROCESO
 * 3.1- Asignar nuevo PID
 * 3.2- Recibir BESO Code
 * 3.4- Solicitar memoria a la MSP
 * 3.5- Crear TCB e Init del PC
 * 3.6- Encolar proceso (segun BBNP)
*/

	// parametros recibidos del Kernel
	logger = ((arg_LOADER *)parametros)->ptrLogger;
//	ip_kernel = ((arg_LOADER *)parametros)->ipKernel;
//	puerto_consola = ((arg_LOADER *)parametros)->puertoConsola;
	ip_msp = ((arg_LOADER *)parametros)->ipMSP;
	puerto_msp = ((arg_LOADER *)parametros)->puertoMSP;
	tamanio_stack = ((arg_LOADER *)parametros)->tamanioStack;


	log_trace(logger,"INICIO de registro de actividades del Hilo PLP.");

	// creacion de estructuras
	cola_new = queue_create();
	cola_ready = queue_create();
	cola_exit = queue_create();

	log_info(logger,"Se crean las estructuras necesarias del hilo.");

	int select_result;
	struct timeval tv; //Timeout del Select
	tv.tv_sec = DELAY_CHECK_NEW_READY_SEC;
	tv.tv_usec = DELAY_CHECK_NEW_READY_USEC;

	//1- Conectarse a MSP
	conectar_a_UMV(ip_msp, puerto_msp);

	//2- Socket de escucha para Programas
	crear_listener(ip_kernel, puerto_consola);

	for(;;)
	{
		read_fds = master; // copy it (copiamos el master al read_fds)
		while((select_result = select(fdmax+1, &read_fds, NULL, NULL, &tv)) <= 0)
		{
			if (select_result == -1) {
				perror("[Kernel-PLP]: Error on 'select()' function.");
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


