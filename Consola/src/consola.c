/*
 ============================================================================
 Name        : consola.c
 Author      : los-metatecla
 Version     :
 Copyright   :
 Description :	PROCESO PROGRAMA
 	 	 	 	NO sera el encargado de compilar el codigo, ya vendra compilado.
 ============================================================================
 *	CICLO DEL PROCESO PROGRAMA
 *
 * 1- Recibir file.eso a ejecutarse como param
 * 2- Crear/Abrir file log del programa para el registro de actividades
 * 3- Leer archivo de configuracion y asignar valores
 * 4- Conectar a Kernel
 *	4.1- Analiza el primer mensaje (handshake con Kernel)
 * 	4.2 Envio del PID al Proceso Kernel
 * 5- Abrir file (pasado explicita o implicitamente como param)
 * 6- Enviar file.eso al Kernel
 * 7- Quedar a la espera de mensajes para imprimir por stdout
 */

#include "consola.h"

void cerrar_socket_cliente() {
	close(sockfd);
}

void leer_config()
{
    char *config_path = getenv("ESO_CONFIG");

    config_file = config_create(config_path);
 //   log_info(logger,"Parseo y Extraccion de valores de archivo de configuracion");

	if (config_keys_amount(config_file) != MAX_COUNT_OF_CONFIG_KEYS)
	{
		fprintf(stderr,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco.\n");
 //   	log_error(logger,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco.");
		exit(1);
	}

    //IP_KERNEL
    if(config_has_property(config_file, "IP_KERNEL"))
    {
    	ip_kernel = config_get_string_value(config_file, "IP_KERNEL");
//    	sprintf(bufferLog,"IP_KERNEL = [%s]",ip_kernel);
//    	log_debug(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'IP_KERNEL' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
//		log_error(logger,"Falta key 'IP_KERNEL' en archivo de configuracion. Programa abortado.");
		exit(1);
	}
    //PUERTO_KERNEL
    if(config_has_property(config_file, "PUERTO_KERNEL"))
    {
    	puerto_kernel = config_get_string_value(config_file,"PUERTO_KERNEL");
//    	sprintf(bufferLog,"PUERTO_KERNEL = [%s]",puerto_kernel);
//    	log_debug(logger,bufferLog);
    } else {
		fprintf(stderr, "Falta key 'PUERTO_KERNEL' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
//		log_error(logger,"Falta key 'PUERTO_KERNEL' en archivo de configuracion. Programa abortado.");
		exit(1);
	}
}

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
//	sprintf(bufferLog,"Probando de conectar a Kernel por host [%s] y port [%s]", ip, puerto);
//	log_debug(logger,bufferLog);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(ip, puerto, &hints, &servinfo)) != 0)	{
		fprintf(stderr, "[Programa]: Error on 'getaddrinfo' function -> %s\n", gai_strerror(rv));
//		sprintf(bufferLog, "Error on 'getaddrinfo' function -> %s\n", gai_strerror(rv));
//		log_debug(logger,bufferLog);

		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)	{
			perror("[Programa]: Error on 'socket()' function.");
//			log_error(logger,"Error on 'socket()' function.");

			continue;
		}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)	{
			cerrar_socket_cliente();
			perror("[Programa]: Error on 'connect()' function.");
//			log_error(logger,"Error on 'connect()' function.");

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
//	log_destroy(logger);
	cerrar_socket_cliente();
//	free(buffer);
	exit(1);
}

void conectar_a_Kernel(char *ip, char *puerto)
{
	if(crear_socket_cliente(ip,puerto))
	{
		fprintf(stderr,"[Programa]: No se pudo contactar al Kernel. Programa abortado.\n");
//		log_error(logger,"No se pudo conectar al Kernel. Programa abortado.");
		exit(1);
	}
}

int main(int argc, char	**argv){

	char* script_name = argv[1];

	// Control de argumentos
	if(argc == 1 || argc > 2)
	{
		printf("[Error] Usage:\n> ./Consola file.eso\n");
		exit(1);
	} else {
		u_int32_t esESO = string_ends_with(script_name,".eso");
		if(!esESO)
		{
			printf("[Error]: Tu script no es un autentico archivo ejecutable (.eso). Programa abortado.\n");
			exit(1);
		}
	}

	leer_config();
	conectar_a_Kernel(ip_kernel,puerto_kernel);


	return EXIT_SUCCESS;
}
