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
 */

#include "funciones_consola.h"

int main(int argc, char	**argv){

	// 1- Recibir BESO FILE a ejecutar
	char* beso_file = argv[1];

	// 2- Crear/Abrir file log del programa para el registro de actividades
	logger = log_create("/home/utnso/tp-2014-2c-los-metatecla/Consola.log", "CONSOLA", 0, LOG_LEVEL_TRACE);
	log_info(logger,"Inicio de registro de actividades del Proceso CONSOLA.");

	// 3- Leer archivo de configuracion y asignar valores
	leer_config();

	// 4- Conectar a Proceso KERNEL
	//int sockKernel = conectar_a_Kernel(ip_kernel,puerto_kernel);
	int sockKernel = socket_crearYConectarCliente(ip_kernel, puerto_kernel);

	if(sockKernel == -1){
		printf("no se pudo conectar al kernel\n");
	}


	t_struct_numero* es_consola = malloc(sizeof(t_struct_numero));
	es_consola->numero = ES_CONSOLA;
	socket_enviar(sockKernel, D_STRUCT_NUMERO, es_consola);

	t_struct_env_bytes* beso_code = malloc(sizeof(t_struct_env_bytes));

	FILE* BESO = abrir_archivo(beso_file, logger, NULL);
	int sizeOfBody = calcular_tamanio_archivo(BESO);
	char* fileBody =leer_archivo(BESO, sizeOfBody);

	sizeOfBody = calcular_tamanio_archivo(BESO); //REPITO ESTO PORQUE ME LO PONIA EN 0... NI IDEA
	beso_code->tamanio = sizeOfBody;
	beso_code->buffer = malloc(sizeOfBody);
	memcpy(beso_code->buffer, fileBody, sizeOfBody);
	beso_code->base = 1; //No importa
	beso_code->PID = 1; //No importa

	int j = socket_enviar(sockKernel, D_STRUCT_ENV_BYTES, beso_code);

	if(j == -1){
		printf("No se envio bien el codigo\n");
	}

	free(beso_code->buffer);
	free(fileBody);
	free(beso_code);
	fclose(BESO);

	escuchar_mensajes(sockKernel);

	return 0;
}
