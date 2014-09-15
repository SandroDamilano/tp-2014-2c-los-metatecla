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
	#include <string.h>
	#include <netdb.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <errno.h>

	#include <commons/string.h>
	#include <estructuras_auxiliares.h>
	#include <sockets/enviar_recibir_paquete.h>

	// BESO file vars
	FILE *file;
	char *file_name;
	uint32_t esBESO;

	// config file vars
	char* ip_kernel;
	char* puerto_kernel;

	typedef enum Operaciones {
		HANDSHAKE_SUCCESS = 1,
		HANDSHAKE_FAIL,
		FILE_LINE,
		FILE_EOF,
		FILE_RECV_SUCCESS,
		FILE_RECV_FAIL,
		ENVIAR_IMPRIMIR_TEXTO = 102,
		END_PROGRAM
	} t_operaciones;

	char *buffer;
	char bufferLog[80];
	char* fileBody;
	int sizeOfFile;
	int bytesReadFile;
	t_operaciones idOperacion;

	// Variables para Socket Cliente
	int sockfd, numbytes;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];


	// FUNCIONES Grales
	void leer_config();

	//Funciones para Handshake
	int handleHandshake(char *datos);
	int handlePIDResult(char* datos);
	int analizar_paquete(char *paquete, t_operaciones *id);
	void conectar_a_Kernel(char *, char *);

	//Funciones para Sockets
	void *get_in_addr(struct sockaddr *); 					// Devuelve estructura sockaddr.
	int crear_socket_cliente (const char *, const char *);	// Crea un socket cliente para conectar al Kernel.
	void cerrar_socket_cliente();							// Cierra el socket cliente.

#endif /* CONSOLA_H_ */
