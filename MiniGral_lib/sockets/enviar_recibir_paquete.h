
#ifndef ENVIAR_RECIBIR_PAQUETE_H_
#define ENVIAR_RECIBIR_PAQUETE_H_

//Esto lo agrego para el kernel o la MSP. Se usa en socket_crearServidor
#define MAX_CONNECTION_SERVER 60 //VAMOS A ATENDER DE A 60 CONEXIONES COMO MAXIMO A LA VEZ


	#include <netinet/in.h>
	#include <sys/types.h>
	#include <arpa/inet.h>
	#include <unistd.h>

	#include "commons/string.h"

	#include "../estructuras_auxiliares.h"
	#include "../serialize/functions.h"

	typedef struct Cabecera {
		uint32_t id;		/* ID de operacion */
		uint32_t longitud; /* Longitud del mensaje, en bytes */
	} cabecera_t;

	extern int RETARDO;

	//Funciones de proposito Gral

	int socket_enviar(int socketReceptor, t_tipoEstructura tipoEstructura, void* estructura);
	int socket_enviarSignal(int socketReceptor, uint32_t signal);
	int socket_recibir(int socketEmisor, t_tipoEstructura * tipoEstructura, void** estructura);
	int socket_recibirSignal(int socketEmisor, uint32_t *signal);

	int socket_crearCliente(void);
	int socket_conectarCliente(int sockfd,char *serverIp, int serverPort);
	int socket_crearYConectarCliente(char *serverIp, int serverPort);
	int socket_crearServidor(char *ip, int port);
	int socket_aceptarCliente(int socketEscucha);
	int socket_cerrarConexion(int socket);

#endif /* ENVIAR_RECIBIR_PAQUETE_H_ */
