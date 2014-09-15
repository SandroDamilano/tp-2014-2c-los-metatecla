/*
 ============================================================================
 PROCESO PROGRAMA
 	 	 	 	NO sera el encargado de compilar el codigo, ya vendra compilado.
 ============================================================================
 *	CICLO DEL PROCESO PROGRAMA
 *
 * 1- Recibir file.beso a ejecutar.
 * 2- Crear/Abrir file log del programa para el registro de actividades.
 * 3- Leer archivo de configuracion y asignar valores.
 * 4- Conectar a Proceso Kernel.
 *	4.1- Analiza el primer mensaje (handshake con Kernel) y esperar por respuesta.
 * 5- Abrir BESO file.
 * 6- Enviar BESO file al Kernel.
 * 7- Quedar a la espera de mensajes para imprimir por STDOUT.
 * 8- Destruimos la estructura config, cerramos socket y liberamos memoria.
 *
 * TODO limpiar un poco mas el codigo para independizar funcionalidades como deserializacion, handshake, operaciones de sockets..
 */

#include "consola.h"

// --------------------------------------
// to RECIEVE

stream_t *deserialize_struct_mensaje(char *datos) {
	stream_t * self= malloc(sizeof(stream_t));
	int offset = 0, tmp_size = 0;

	offset += tmp_size;
	for(tmp_size = 1; (datos + offset)[tmp_size-1] != '\0'; tmp_size++);
	self->data = malloc(tmp_size);
	memcpy(self->data, datos + offset, tmp_size);

	return self;
}

void undo_struct_mensaje(char* datos) {
	stream_t *deserializado = deserialize_struct_mensaje(datos);

	printf("%s\n", deserializado->data);
	free(deserializado->data);
	free(deserializado);
}

int handleHandshake(char *datos) {
	undo_struct_mensaje(datos);
	return 0;
}

int handlePrintMsg(char *datos) {
	undo_struct_mensaje(datos);
	return 0;
}

int handleEndProgramm(char *datos) {
	undo_struct_mensaje(datos);
	return 0;
}

int handleFileResult(char *datos) {
	undo_struct_mensaje(datos);

	return 0;
}

int analizar_paquete(char *paquete, t_operaciones *op)
{
	int res; // Resultado de cada handler

	recv_msg_from(sockfd, (int*)op, &paquete);
//printf("id op [%d]\n",*op);	// ONLY for debug purpose
	switch(*op)
	{
		case HANDSHAKE_SUCCESS: case HANDSHAKE_FAIL:
//printf("HANDSHAKE_SUCCESS o HANDSHAKE_FAIL\n");
			log_trace(logger, "Respuesta Handshake, recibida.");
			res = handleHandshake(paquete);
			break;
		case FILE_RECV_SUCCESS: case FILE_RECV_FAIL:
//printf("FILE_SUCCESS o FILE_FAIL\n");
			log_trace(logger, "Kernel recibio envio del BESO file.");
			res = handleFileResult(paquete);
			break;
		case ENVIAR_IMPRIMIR_TEXTO:
//printf("ENVIAR_IMPRIMIR_TEXTO\n");
			log_trace(logger, "Solicitud de Impresion por STDOUT, recibida.");
			res = handlePrintMsg(paquete);;
			break;
		case END_PROGRAM:
			log_trace(logger, "Respuesta de FIN de programa, recibida.");
			res = handleEndProgramm(paquete);
//printf("END_PROGRAM\n");
			break;
		default:
//printf("default\n");
			log_error(logger, "Kernel se cerro inesperadamente. Abortando ejecucion de script");
			exit(EXIT_FAILURE);
			break;
	}

	// Se libera el mensaje  cuando ya no lo necesitamos
	if (paquete != NULL)	{
		free (paquete);
		paquete = NULL;
	}
	return res;
}

// --------------------------------------
// to SEND

stream_t *serialize_struct_mensaje (stream_t *msg, int *tamanho) {
	*tamanho = 	strlen(msg->data) + 1;

	char *data = (char*)malloc(*tamanho);
	stream_t *stream = malloc(sizeof(stream_t));
	int offset = 0, tmp_size = 0;

	offset += tmp_size;
	memcpy(data + offset, msg->data, tmp_size = strlen(msg->data) + 1);

	stream->data = data;

	return stream;
}

stream_t* do_struct_mensaje(stream_t* datos, int* size) {
	stream_t *serializado = serialize_struct_mensaje(datos, size);

	return serializado;
}

stream_t *handleFileLine(stream_t* datos, int* size) {
	return do_struct_mensaje(datos, size);
}

int preparar_paquete(u_int32_t socket, t_operaciones op, void* estructura)
{
	int tamanho = 0;
	stream_t *paquete = NULL;
//printf("id op [%d]\n",op);	// ONLY for debug purpose
	switch(op)
	{
		case FILE_LINE: case FILE_EOF:
//printf("FILE_LINE o FILE_EOF\n");
			paquete = handleFileLine((stream_t*)estructura, &tamanho);
			break;
		default:
//printf("default\n");
			return -1;
			break;
	}

	send_msg_to(socket, op, paquete->data , tamanho);

	free(paquete->data);
	free(paquete);

	return 0;
}

// --------------------------------------

void *get_in_addr_cliente(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void cerrar_socket_cliente() {
	close(sockfd);
}

int crear_socket_cliente (const char *ip, const char *puerto)
{
	printf("[Programa]: Probando de conectar a Kernel por host [%s] y port [%s]\n", ip, puerto);
	sprintf(bufferLog,"Probando de conectar a Kernel por host [%s] y port [%s]", ip, puerto);
	log_debug(logger,bufferLog);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(ip, puerto, &hints, &servinfo)) != 0)	{
		fprintf(stderr, "[Programa]: Error on 'getaddrinfo' function -> %s\n", gai_strerror(rv));
		sprintf(bufferLog, "Error on 'getaddrinfo' function -> %s\n", gai_strerror(rv));
		log_debug(logger,bufferLog);

		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)	{
			perror("[Programa]: Error on 'socket()' function.");
			log_error(logger,"Error on 'socket()' function.");

			continue;
		}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)	{
			cerrar_socket_cliente();
			perror("[Programa]: Error on 'connect()' function.");
			log_error(logger,"Error on 'connect()' function.");

			continue;
		}
		break;
	}

	if (p == NULL)	{
		fprintf(stderr, "[Programa]: Falla en la conexion al Kernel\n");
		return 1;
	}
	inet_ntop(p->ai_family, get_in_addr_cliente((struct sockaddr *)p->ai_addr), s, sizeof s);
	freeaddrinfo(servinfo);

	return 0;
}

void liberarMemoria()
{
	config_destroy(config_file);
	log_destroy(logger);
	cerrar_socket_cliente();
	free(buffer);
	exit(1);
}

void conectar_a_Kernel(char *ip, char *puerto)
{
	if(crear_socket_cliente(ip,puerto))
	{
		fprintf(stderr,"[Programa]: No se pudo contactar al Kernel. Programa abortado.\n");
		log_error(logger,"No se pudo conectar al Kernel. Programa abortado.");
		exit(1);
	}
}

void leer_config()
{
    char *config_path = getenv("ESO_CONFIG");

    config_file = config_create(config_path);
    log_info(logger,"Parseo y Extraccion de valores de archivo de configuracion");

	if (config_keys_amount(config_file) != MAX_COUNT_OF_CONFIG_KEYS)
	{
		fprintf(stderr,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco.\n");
		log_error(logger,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco.");
		exit(1);
	}

    //IP_KERNEL
    if(config_has_property(config_file, "IP_KERNEL"))
    {
    	ip_kernel = config_get_string_value(config_file, "IP_KERNEL");
    	sprintf(bufferLog,"IP_KERNEL = [%s]",ip_kernel);
    	log_debug(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'IP_KERNEL' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'IP_KERNEL' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

    //PUERTO_KERNEL
    if(config_has_property(config_file, "PUERTO_KERNEL"))
    {
    	puerto_kernel = config_get_string_value(config_file,"PUERTO_KERNEL");
    	sprintf(bufferLog,"PUERTO_KERNEL = [%s]",puerto_kernel);
    	log_debug(logger,bufferLog);
    } else {
		fprintf(stderr, "Falta key 'PUERTO_KERNEL' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'PUERTO_KERNEL' en archivo de configuracion. Programa abortado.");
		exit(1);
	}
}

int main(int argc, char	**argv){

	// 1- Recibir BESO FILE a ejecutar
	char* beso_file = argv[1];

	// Control de argumentos
	if(argc == 1 || argc > 2)
	{
		printf("[Error] Usage:\n> ./Consola fileIN.bc\n");
		exit(1);
	} else {
		esBESO = string_ends_with(beso_file,".bc");
		if(!esBESO)
		{
			printf("[Error]: Tu script no es un autentico archivo ejecutable ESO (.bc). Programa abortado.\n");
			exit(1);
		}
	}

	// 2- Crear/Abrir file log del programa para el registro de actividades
	logger = log_create("consola.log", "CONSOLA", 0, LOG_LEVEL_TRACE);
	log_info(logger,"Inicio de registro de actividades del Proceso CONSOLA.");

	// 3- Leer archivo de configuracion y asignar valores
	leer_config();

	// 4- Conectar a Proceso KERNEL
	conectar_a_Kernel(ip_kernel,puerto_kernel);

	//4.1- Analiza el primer mensaje (handshake con Kernel) y esperar por respuesta
	analizar_paquete(buffer, &idOperacion);
	if(idOperacion == HANDSHAKE_FAIL) {
		printf("[Program]: Ocurrio un problema en el proceso Handshake. Programa abortado.\n");
		log_error(logger," Ocurrio un problema en el proceso Handshake. Programa abortado.");
		liberarMemoria();
		exit(1);
	}
	printf("[Programa]: Conexion a Kernel establecida.\n");
	log_info(logger,"Conexion a Kernel establecida.");

	//5- Abrir BESO file
	file = fopen(beso_file,"r");
	log_trace(logger,"Abriendo BESO FILE para enviarlo al Kernel.");

	if (!file || file == NULL)	{
		printf("[Programa]: No se pudo abrir el archivo. Programa abortado.");
		log_error(logger,"No se pudo abrir el archivo. Programa abortado.");
		liberarMemoria();
		exit(1);
	}

	fseek(file, 0L, SEEK_END);
	sizeOfFile = ftell(file) + 1;
	fseek(file, 0L, SEEK_SET);

	fileBody = (char*) malloc(sizeOfFile);

	bytesReadFile = fread(fileBody, 1, sizeOfFile, file);
	fileBody[bytesReadFile] = 0;	// Add terminating zero.

	if((bytesReadFile != sizeOfFile) && !feof(file)) {
		printf("[Programa]: Error al leer el archivo. Programa abortado.");
	    log_error(logger,"Error al leer el archivo. Programa abortado.");
		liberarMemoria();
		exit(1);
	}

	//6- Enviar BESO FILE al Kernel
	stream_t msg;
	msg.data = fileBody;
	preparar_paquete(sockfd, FILE_LINE, &msg);
	log_trace(logger,"Se envio BESO FILE al Kernel.");
	free(fileBody);

	msg.data = "";	// Senial de corte !!
	preparar_paquete(sockfd, FILE_EOF, &msg);
	fclose(file);

	analizar_paquete(buffer, &idOperacion); //Se queda a la espera de confirmacion de recepcion del archivo completo.
											//Y la posible negativa a la solicitud de memoria, a traves del LOADER, por parte de la MSP.
	if(idOperacion == FILE_RECV_FAIL)	{
		perror("[Programa]: Ocurrio un problema en la recepcion del BESO FILE. Programa abortado.");
		log_error(logger,"Ocurrio un problema en la recepcion del BESO FILE. Programa abortado.");
		liberarMemoria();
		exit(1);
	} else
	if(idOperacion == END_PROGRAM)	{
		perror("[Programa]: Ocurrio un problema en la solicitud de memoria a la MSP. Programa abortado.");
		log_error(logger,"Ocurrio un problema en la solicitud de memoria a la MSP. Programa abortado.");
		liberarMemoria();
		exit(1);
	}
	log_trace(logger,"BESO FILE recibido con exito por el Kernel.");

	//7- Quedar a la espera de mensajes para imprimir por STDOUT
	while(analizar_paquete(buffer, &idOperacion) == 0)
	{
		if(idOperacion == END_PROGRAM) {
			printf("[Programa]: Cerrando Programa...\n");
			log_trace(logger,"Cerrando Programa");
			break;
		}
	}

	// Destruimos la estructura config, cerramos socket y liberamos memoria
	liberarMemoria();

	return 0;
}
