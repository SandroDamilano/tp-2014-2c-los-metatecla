/*
 *      EJEMPLO DE USO:

        void *mensaje = NULL;
		uint32_t id;

// Se lee el mensaje
		read_msg_from (socket, &id, &mensaje);

//Se hace el tratamiento del mensaje según el identificador (id)
			switch (id)
			{
				case operacion1:
				{
					// Se hace un cast de void * a struc_operacion1 *, para tener más accesibles los datos recibidos
					Struct_operacion1 *datos = (Struct_operacion1 *)mensaje;
					... // tratamiento de la variables datos
					break;
				}

				case operacionN:
				{
					Struct_operacionN *datos = (Struct_operacionN *) mensaje;
					...//tratamiento de la variable datos (por ejemplo, desempaquetar)
					break;
				}
			}

// Se libera el mensaje  cuando ya no lo necesitamos
			if (mensaje != NULL)
			{
				free (mensaje);
				mensaje = NULL;
			}
*/

#include "enviar_recibir_paquete.h"

/*************************************** FUNCIONES PARA ENVIAR Y RECIBIR ***************************************************/

int socket_enviar(int socketReceptor, t_tipoEstructura tipoEstructura, void* estructura){
	int cantBytesEnviados;

	t_stream * paquete = serialize(tipoEstructura, estructura);

	cantBytesEnviados = send(socketReceptor, paquete->data, paquete->length, 0);
	free(paquete->data);
	free(paquete);
	if( cantBytesEnviados == -1){
		perror("Server no encontrado\n");
		return 0;
	}
	else {
		return 1;
	}
}

int socket_enviarSignal(int socketReceptor, uint32_t signal){
	t_struct_signal *structSignal = malloc(sizeof(t_struct_signal));

	structSignal->signal = signal;

	int seEnvio = socket_enviar(socketReceptor, D_STRUCT_SIGNAL, (void*)structSignal);

	free(structSignal);
	return seEnvio;
}

int socket_recibir(int socketEmisor, t_tipoEstructura * tipoEstructura, void** estructura){
	int cantBytesRecibidos;
	t_header header;
	char* buffer;
	char* bufferHeader;

	bufferHeader = malloc(sizeof(t_header));

	cantBytesRecibidos = recv(socketEmisor, bufferHeader, sizeof(t_header), MSG_WAITALL);	//Recivo por partes, primero el header.
	if(cantBytesRecibidos == -1){
		free(bufferHeader);
		perror("Error al recibir datos\n");
		return 0;
	}

	//Si se recibe 0, es porque se desconectó el socketEmisor
	if(cantBytesRecibidos == 0){
		free(bufferHeader);
		return -1;
	}

	header = despaquetizarHeader(bufferHeader);
	free(bufferHeader);

	if (tipoEstructura != NULL) {
		*tipoEstructura = header.tipoEstructura;
	}

	if(header.length == 0){	//Si recivo mensaje con length 0 retorno 1 y *estructura NULL.
		if (estructura != NULL) {
			*estructura = NULL;
		}
		return 1;
	}

	buffer = malloc(header.length);
	cantBytesRecibidos = recv(socketEmisor, buffer, header.length, MSG_WAITALL);	//Recivo el resto del mensaje con el tamaño justo de buffer.
	if(cantBytesRecibidos == -1){
		free(buffer);
		perror("Error al recibir datos\n");
		return 0;
	}

	//Si se recibe 0, es porque se desconectó el socketEmisor
	if(cantBytesRecibidos == 0){
		free(buffer);
		return -1;
	}

	if(estructura != NULL) {
		*estructura = deserialize(header.tipoEstructura, buffer, header.length);
	}

	free(buffer);

	return 1;
}

int socket_recibirSignal(int socketEmisor, uint32_t *signal){
	void * estructuraRecibida;
	t_tipoEstructura tipoRecibido;

	int recibio = socket_recibir(socketEmisor,&tipoRecibido, &estructuraRecibida);
	if(!recibio) {
		*signal = S_ERROR;
		return 0;
	}

	if (tipoRecibido != D_STRUCT_SIGNAL){
		*signal = S_ERROR;
		return 0;
	}

	*signal = ((t_struct_signal*) estructuraRecibida)->signal;
	free(estructuraRecibida);
	return 1;
}
/*********************************** FUNCIONES PARA CREAR Y CONECTAR CLIENTES Y SERVIDORES ***********************/
/*
 * Nombre: socket_crearCliente/0
 * Argumentos:
 * 		- NINGUNO
 *
 * Devuelve:
 * 		int (Descriptor al socket creado), en caso de error, devuelve -1.
 *
 *
 * Funcion: Crea el socket para un cliente.
 */
int socket_crearCliente(void){

	int sockfd;

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		perror("Error al crear socket");//Crear log para este error.
		return -1;
	}

	return sockfd;

}

/*Nombre: socket_conectarCliente/3
 * Argumentos:
 * 		- sockfd (int), (descriptor del socket cliente).
 * 		- serverIp (char*),(IP del server a conectar)
 * 		- serverPort (int), (puerto del server a conectar)
 *
 * Devuelve:
 * 		int (Descriptor al socket que se va a conectar, devuelve -1 si hay error).
 *
 * Funcion: Conectarme a un server con su IP y puerto.
 *
 */
int socket_conectarCliente(int sockfd,char *serverIp, int serverPort){

		struct sockaddr_in socketInfo;

		//INICIALIZACION DE SOCKETINFO
		socketInfo.sin_family = AF_INET;
		socketInfo.sin_port = htons(serverPort); //host to network short
		socketInfo.sin_addr.s_addr = inet_addr(serverIp);
		memset(&(socketInfo.sin_zero),'\0',8); // PONGO A 0 EL RESTO DE LA ESTRUCTURA
		// ME CONECTO CON LA DIRECCIÓN DE SOCKETINFO
		//SIEMPRE VERIFICANDO QUE NO DEN -1 LAS FUNCIONES O 0 EN CASO DE RECV() -- SOLO PARA SERVER IGUAL :)

		if(connect(sockfd , (struct sockaddr *)&socketInfo , sizeof(socketInfo)) == -1){
			perror("Falló la conexión"); // Cambiar esto por un log.
			return -1;
		}

		return sockfd;
}

/*Nombre: socket_crearYConectarCliente/2
 * Argumentos:
 * 		- serverIp (char*),(IP del server a conectar)
 * 		- serverPort (int), (puerto del server a conectar)
 *
 * Devuelve:
 * 		int (Descriptor al socket que se va a conectar).
 *
 * Funcion: Crear y conectar un nuevo cliente a un server con su IP y puerto.
 *
 */
int socket_crearYConectarCliente(char *serverIp, int serverPort){
	int sockfd;
	sockfd = socket_crearCliente();
	if (sockfd < 0)
		return -1;

	sockfd = socket_conectarCliente( sockfd,(char*)serverIp, serverPort);

	return sockfd;
}

/*Nombre: socket_crearServidor/2
 * Argumentos:
 * 		- serverIp (char*),(IP del server)
 * 		- serverPort (int), (puerto del server)
 *
 * Devuelve:
 * 		int (Descriptor al socket del server).
 *
 * Funcion: Crear un nuevo servidor.
 *
 */
int socket_crearServidor(char *ip, int port){
	int socketEscucha;
	struct sockaddr_in miSocket;//ESTE ES EL SOCKET CON LA DRECCION IP

	if((socketEscucha = socket(AF_INET,SOCK_STREAM,0)) == -1){
		perror("Error al crear socket");
		return -1;
	}

	miSocket.sin_family = AF_INET;
	miSocket.sin_port = htons(port);
	miSocket.sin_addr.s_addr = inet_addr(ip);
	memset(&(miSocket.sin_zero),'\0',8); // PONGO A 0 EL RESTO DE LA ESTRUCTURA

	int yes = 1;
	if (setsockopt(socketEscucha, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	if(bind(socketEscucha,(struct sockaddr*)&miSocket, sizeof(miSocket)) == -1){
		perror ("Error al bindear el socket escucha");
		return -1;
	}

	if (listen(socketEscucha, MAX_CONNECTION_SERVER) == -1){
		perror("Error en la puesta de escucha");
		return -1;
	}

	return socketEscucha;

}

/*Nombre: socket_aceptarCliente/1
 * Argumentos:
 * 		- socketEscucha (int),(descriptor del socket del server para escuchar conexiones)
 *
 * Devuelve:
 * 		int (Descriptor al socket de la nueva conexión).
 *
 * Funcion: Aceptar un cliente que está siendo previamente escuchado.
 *
 */
int socket_aceptarCliente(int socketEscucha){
	int socketNuevaConexion;
	unsigned int size_sockAddrIn;
	struct sockaddr_in suSocket;
	size_sockAddrIn = sizeof(struct sockaddr_in);
	socketNuevaConexion = accept(socketEscucha, (struct sockaddr *)&suSocket, &size_sockAddrIn);
	if(socketNuevaConexion < 0) {
		perror("Error al aceptar conexion entrante");
		return -1;
	}
	return socketNuevaConexion;
}


/******************************** CERRAR CONEXION *************************************/
/*
 * Nombre: socket_cerrarConexion/1
 *
 * Argumentos:
 * 		- socket
 *
 * Devuelve:
 * 		int (-1-> si se cerro ok, 0-> si hubo problemas).
 *
 * Funcion: cerrar el socket
 */
int socket_cerrarConexion(int socket){
	return close(socket);
}
