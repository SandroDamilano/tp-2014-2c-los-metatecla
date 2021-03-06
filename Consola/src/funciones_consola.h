/*
 * funciones_consola.h
 *
 *  Created on: 18/10/2014
 *      Author: utnso
 */

#ifndef FUNCIONES_CONSOLA_H_
#define FUNCIONES_CONSOLA_H_

#define MAX_COUNT_OF_CONFIG_KEYS 2

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <netdb.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <errno.h>
	#include <signal.h>

	#include <commons/string.h>
	#include <estructuras_auxiliares.h>
	#include <sockets/enviar_recibir_paquete.h>

	// BESO file vars
	FILE *file;
	char *file_name;
	uint32_t esBESO;

	// config file vars
	char* ip_kernel, *config_path;
	int puerto_kernel;

	//char *buffer;
	void* buffer;
	char bufferLog[80];
	char* fileBody;
	int sizeOfFile;
	int bytesReadFile;
	t_tipoEstructura idOperacion;

	// Variables para Socket Cliente
	int sockfd, numbytes;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	// FUNCIONES Grales
	void leer_config(char* config_path);
	void liberarMemoria();
	void escuchar_mensajes(int sockKernel);

	//Funciones para Handshake
	int handleHandshake(char *datos);
	int handlePIDResult(char* datos);
	int preparar_paquete(u_int32_t socket, t_tipoEstructura op, void* estructura);
	int analizar_paquete(void *paquete, t_tipoEstructura *id);
	int conectar_a_Kernel(char *, int);

	//Funciones para Sockets
	void *get_in_addr(struct sockaddr *); 					// Devuelve estructura sockaddr.
	int crear_socket_cliente (const char *, const char *);	// Crea un socket cliente para conectar al Kernel.

#endif /* FUNCIONES_CONSOLA_H_ */
