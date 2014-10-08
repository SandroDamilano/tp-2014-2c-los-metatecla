
#ifndef ENVIAR_RECIBIR_PAQUETE_H_
#define ENVIAR_RECIBIR_PAQUETE_H_

//Esto lo agrego para el kernel o la MSP. Se usa en socket_crearServidor
#define MAX_CONNECTION_SERVER 60 //VAMOS A ATENDER DE A 60 CONEXIONES COMO MAXIMO A LA VEZ


	#include <netinet/in.h>
	#include <sys/types.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <sys/epoll.h>
	#include <sys/ioctl.h>

	#include "commons/string.h"

	#include "../estructuras_auxiliares.h"

	typedef struct Cabecera {
		uint32_t id;		/* ID de operacion */
		uint32_t longitud; /* Longitud del mensaje, en bytes */
	} cabecera_t;

	extern int RETARDO;

	//Funciones de proposito Gral

	int endianess();
	int sendall(int socket, void *buf, int *len);
	void recv_msg_from(int socket, int *idMensaje, char **mensaje);			//analizar_paquete()
	void send_msg_to(int socket, int idMensaje, char *mensaje, int tamanho);//preparar_paquete()

	int socket_crearCliente(void);
	int socket_conectarCliente(int sockfd,char *serverIp, int serverPort);
	int socket_crearYConectarCliente(char *serverIp, int serverPort);
	int socket_crearServidor(char *ip, int port);
	int socket_aceptarCliente(int socketEscucha);
	int socket_cerrarConexion(int socket);


#endif /* ENVIAR_RECIBIR_PAQUETE_H_ */
