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

int endianess() {
	unsigned i = 1;
	return ((*(char*)&i) == 1)? 1 : 0;
}

//int recvall(int socket, void *buf, int *len)
//{
//
//	return 0;
//}

void recv_msg_from(int socket, int *idMensaje, char **mensaje) {
    cabecera_t cabecera;
    *mensaje = NULL;  /* Ponemos el mensaje a NULL por defecto */
    int nbytes;

    if((nbytes = recv(socket, &cabecera, sizeof(cabecera), 0)) == -1) /* Se lee la cabecera */
    {
		perror("Error on 'recv_msg_from()' function at MiniGral_lib");
		exit(1);
    }

    /* Rellenamos el identificador para devolverlo */
    *idMensaje = cabecera.id;

    /* Si hay que leer una estructura detrás */
    if (cabecera.longitud > 0) {
        *mensaje = malloc (cabecera.longitud);  /* Se reserva espacio para leer el mensaje */
        recv(socket, *mensaje, cabecera.longitud, 0);
    }
}

// Envia todos los bytes del buffer a traves de varios SEND's si es necesario.
int sendall(int socket, void *buf, int *len) {
	int n;
	int total = 0;			// how many bytes we've sent
	int bytesleft = *len;	// how many we have left to send

	while(total < *len) {
		n = send(socket, buf+total, bytesleft, 0);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}
	*len = total;			// return number actually sent here
	return n == -1 ? -1 : 0;// return -1 on failure, 0 on success
}

// Enviamos la cabecera (id,length) y luego el cuerpo (mensaje)
void send_msg_to(int socket, int idMensaje, char *mensaje, int tamanho) {

    cabecera_t cabecera;	//	Se declara y rellena la cabecera
    int bytesEnviados = 0;

    cabecera.id = idMensaje;
    cabecera.longitud = tamanho;
    bytesEnviados = sizeof(cabecera);

    /* Se envía la cabecera */
    if(sendall(socket, &cabecera, &bytesEnviados) == -1) {
    	perror("Error on 'send_msg_to()' function at MiniGral_lib.");
    	printf("Solo se enviaron [%d] bytes a causa del error.", bytesEnviados);
    	exit(1);
    }

    /* Si el mensaje no tiene cuerpo, hemos terminado */
    if ((mensaje == NULL) || (tamanho == 0))
        return;

    /* Se envía el cuerpo */
    bytesEnviados = tamanho;
    if(sendall(socket, mensaje, &bytesEnviados) == -1)
    {
    	perror("Error on 'send_msg_to()' function at MiniGral_lib.");
    	printf("Solo se enviaron [%d] bytes a causa del error.", bytesEnviados);
    	exit(1);
    }
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

