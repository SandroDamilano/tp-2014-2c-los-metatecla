/*
 * consola.h
 *
 *  Created on: 11/09/2014
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

	#define MAX_COUNT_OF_CONFIG_KEYS 2

	#include <stdio.h>
	#include <stdlib.h>
	//#include <stdint.h>
	#include <netdb.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>

	#include <unistd.h>
	#include <errno.h>
	#include <string.h>


	#include <commons/log.h>
	#include <commons/config.h>
	#include <commons/string.h>

	// vars de config
	t_config* config_file;
	u_int32_t esEso;

	char* ip_kernel;
	char* puerto_kernel;

	// Variables para Socket Cliente
	int sockfd, numbytes;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	// FUNCIONES PPALES
	void leer_config();

	//Funciones para Sockets
void *get_in_addr(struct sockaddr *); 					// Devuelve estructura sockaddr.
int crear_socket_cliente (const char *, const char *);	// Crea un socket cliente para conectar al PLP.
void cerrar_socket_cliente();							// Cierra el socket cliente.


#endif /* CONSOLA_H_ */
