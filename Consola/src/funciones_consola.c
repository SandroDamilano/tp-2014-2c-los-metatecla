/*
 * funciones_consola.c
 *
 *  Created on: 18/10/2014
 *      Author: utnso
 */

#include "funciones_consola.h"

// --------------------------------------
// to RECIEVE from KERNEL

int analizar_paquete(void *paquete, t_tipoEstructura *operacion)
{
	int res; // Resultado de cada handler

	res = socket_recibir(sockfd, operacion, paquete); //devuelve 1 si se recibio todoo bien
//printf("id op [%d]\n",*op);	// ONLY for debug purpose
	switch(*operacion)
	{
		case HANDSHAKE_SUCCESS: case HANDSHAKE_FAIL:
//printf("HANDSHAKE_SUCCESS o HANDSHAKE_FAIL\n");
			log_trace(logger, "Respuesta Handshake, recibida.");
			res = print_package_to_output(paquete);
			break;
		case FILE_RECV_SUCCESS: case FILE_RECV_FAIL:
//printf("FILE_SUCCESS o FILE_FAIL\n");
			log_trace(logger, "Kernel recibio envio del BESO file.");
			res = print_package_to_output(paquete);
			break;
		case ENVIAR_IMPRIMIR_TEXTO:
//printf("ENVIAR_IMPRIMIR_TEXTO\n");
			log_trace(logger, "Solicitud de Impresion por STDOUT, recibida.");
			res = print_package_to_output(paquete);;
			break;
		case END_PROGRAM:
			log_trace(logger, "Respuesta de FIN de programa, recibida.");
			res = print_package_to_output(paquete);
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

int preparar_paquete(u_int32_t socket, t_tipoEstructura op, void* estructura)
{
	t_stream *paquete = NULL;
//printf("id op [%d]\n",op);	// ONLY for debug purpose
	switch(op)
	{
		case FILE_LINE: case FILE_EOF:
//printf("FILE_LINE o FILE_EOF\n");
			paquete = serializeStruct_string((t_struct_string*)estructura);
			break;
		default:
//printf("default\n");
			return -1;
			break;
	}

	socket_enviar(socket, op, paquete->data);

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
			socket_cerrarConexion(sockfd);
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
	socket_cerrarConexion(sockfd);
	free(buffer);
	exit(1);
}

int conectar_a_Kernel(char *ip, int puerto)
{
	int sockKernel;
	/*if((sockKernel=crear_socket_cliente(ip,puerto)))
	{
		fprintf(stderr,"[Programa]: No se pudo contactar al Kernel. Programa abortado.\n");
		log_error(logger,"No se pudo conectar al Kernel. Programa abortado.");
		exit(1);
	}*/

	if((sockKernel=socket_crearYConectarCliente(ip, puerto)))
	{
		fprintf(stderr,"[Programa]: No se pudo contactar al Kernel. Programa abortado.\n");
		log_error(logger,"No se pudo conectar al Kernel. Programa abortado.");
		exit(1);
	}
	return sockKernel;
}

void leer_config()
{
    char *config_path = getenv("ESO_CONFIG");

    printf("CONFIG PATH %s\n", config_path);

    config_file = config_create(config_path);
    log_info(logger,"Parseo y Extraccion de valores de archivo de configuracion");

	/*if (config_keys_amount(config_file) != MAX_COUNT_OF_CONFIG_KEYS)
	{
		fprintf(stderr,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco.\n");
		log_error(logger,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco.");
		exit(1);
	}*/

    //PUERTO_KERNEL
     if(config_has_property(config_file, "PUERTO_KERNEL"))
     {
      	puerto_kernel = config_get_int_value(config_file,"PUERTO_KERNEL");
      	printf("Se leyo puerto_kernel %d\n", puerto_kernel);
      	sprintf(bufferLog,"PUERTO_KERNEL = [%d]",puerto_kernel);
      	log_debug(logger,bufferLog);
      } else {
    fprintf(stderr, "Falta key 'PUERTO_KERNEL' en archivo de configuracion. Chequear.\n");
    fprintf(stderr, "Programa abortado.\n");
    log_error(logger,"Falta key 'PUERTO_KERNEL' en archivo de configuracion. Programa abortado.");
    exit(1);
    }

    //IP_KERNEL
    if(config_has_property(config_file, "IP_KERNEL"))
    {
    	ip_kernel = config_get_string_value(config_file, "IP_KERNEL");
    	printf("Se leyo ip_kernel %s\n", ip_kernel);
    	sprintf(bufferLog,"IP_KERNEL = [%s]",ip_kernel);
    	log_debug(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'IP_KERNEL' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'IP_KERNEL' en archivo de configuracion. Programa abortado.");
		exit(1);
	}


}

void escuchar_mensajes(int sockKernel){
	t_tipoEstructura tipoRecibido;
	void* structRecibido;
	int num;
	int maximo;
	char* texto;

	while(1){
		socket_recibir(sockKernel, &tipoRecibido, &structRecibido);

		switch(tipoRecibido){
		case D_STRUCT_NUMERO://Recibo la orden de hacer un imput de un número
			scanf("Ingrese un número: %d\n", num);
			t_struct_numero* innn = malloc(sizeof(t_struct_numero));
			innn->numero = num;
			socket_enviar(sockKernel, D_STRUCT_INNN, innn);
			free(innn);
			break;

		case D_STRUCT_INNC:
			maximo = ((t_struct_numero*)structRecibido)->numero;
			texto = malloc(maximo);
			printf("Ingrese una cadena de texto no mayor a %d caracteres: ", maximo);
			scanf("%s\n", texto);//FIXME Habrá que validar la longitud?
			t_struct_string* innc = malloc(sizeof(t_struct_string));
			innc->string = malloc(strlen(texto)+1);
			memcpy(innc->string, texto, strlen(texto)+1);
			socket_enviar(sockKernel, D_STRUCT_INNC, innc);
			free(innc);
			break;

		case D_STRUCT_OUTN:
			printf("%d\n", ((t_struct_numero*)structRecibido)->numero);
			break;

		case D_STRUCT_OUTC:
			printf("%s\n", ((t_struct_string*)structRecibido)->string);
			break;
		}
	}
}



