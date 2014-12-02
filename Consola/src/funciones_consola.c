/*
 * funciones_consola.c
 *
 *  Created on: 18/10/2014
 *      Author: utnso
 */

#include "funciones_consola.h"



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

void llegoSenialParaTerminar(int n){

	switch(n){
	case SIGINT:
		log_destroy(logger);
		config_destroy(config_file);
		exit(EXIT_SUCCESS);
		break;
	}
}

void escuchar_mensajes(int sockKernel){
	t_tipoEstructura tipoRecibido;
	t_tipoEstructura tipoRecibido2;
	void* structRecibido;
	void* structRecibido2;
	int num;
	int maximo;
	char* texto;

	while(1){

		if(socket_recibir(sockKernel, &tipoRecibido, &structRecibido)!=-1){

		switch(tipoRecibido){
		case D_STRUCT_NUMERO://Recibo la orden de hacer un imput de un número
			printf("Ingrese un número:\n");
			scanf("%d", &num);
			t_struct_numero* innn = malloc(sizeof(t_struct_numero));
			innn->numero = num;
			socket_enviar(sockKernel, D_STRUCT_INNN, innn);
			free(innn);
			break;

		case D_STRUCT_INNC:
			maximo = ((t_struct_numero*)structRecibido)->numero;
			texto = malloc(maximo);
			printf("Ingrese una cadena de texto no mayor a %d caracteres:\n", maximo);
			scanf("%s", texto);
			t_struct_string* innc = malloc(sizeof(t_struct_string));
			innc->string = malloc(strlen(texto)+1);
			memcpy(innc->string,texto, maximo);
			(innc->string)[maximo] = '\0';
			socket_enviar(sockKernel, D_STRUCT_STRING, innc);
			free(innc->string);
			free(innc);
			free(texto);
			break;

		case D_STRUCT_OUTN:
			printf("%d\n", ((t_struct_numero*)structRecibido)->numero);
			break;

		case D_STRUCT_OUTC:
			socket_recibir(sockKernel, &tipoRecibido2, &structRecibido2);
			int length = ((t_struct_numero*) structRecibido2)->numero;
			texto = ((t_struct_string*)structRecibido)->string;
			texto[length] = '\0';
			printf("%s\n", texto);
			free(structRecibido2);
			break;
		}
		}
		free(structRecibido);
		signal(SIGINT, llegoSenialParaTerminar);
	}
}

//HACER FREES


