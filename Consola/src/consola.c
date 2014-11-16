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

	// Control de argumentos
	/*if(argc == 1 || argc > 2)
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
	}*/

	// 2- Crear/Abrir file log del programa para el registro de actividades
	logger = log_create("consola.log", "CONSOLA", 0, LOG_LEVEL_TRACE);
	log_info(logger,"Inicio de registro de actividades del Proceso CONSOLA.");

	// 3- Leer archivo de configuracion y asignar valores
	leer_config();

	// 4- Conectar a Proceso KERNEL
	//int sockKernel = conectar_a_Kernel(ip_kernel,puerto_kernel);
	int sockKernel = sockKernel=socket_crearYConectarCliente(ip_kernel, puerto_kernel);

	/*
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
	*/

	t_struct_numero* es_consola = malloc(sizeof(t_struct_numero));
	es_consola->numero = ES_CONSOLA;
	socket_enviar(sockKernel, D_STRUCT_NUMERO, es_consola);

	//5- Abrir BESO file
	/*file = fopen(beso_file,"r");
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

	*/

	FILE* BESO = abrir_archivo(beso_file, logger, NULL);
	long int tamanio = calcular_tamanio_archivo(BESO);
	char* fileBody =leer_archivo(BESO, tamanio);

	printf("sizeOfFile %d\n", tamanio);
	printf("tam real %d\n", strlen(fileBody));

	//6- Enviar BESO FILE al Kernel
	/*t_stream msg;
	msg.data = fileBody;
	preparar_paquete(sockfd, FILE_LINE, &msg);
	log_trace(logger,"Se envio BESO FILE al Kernel.");
	free(fileBody);

	msg.data = "";	// Senial de corte !!
	preparar_paquete(sockfd, FILE_EOF, &msg);

	t_struct_string* paquete_codigo_beso = malloc(sizeof(t_struct_string));
	paquete_codigo_beso->string = fileBody;

	int j;
	if((j=socket_enviar(sockfd, D_STRUCT_STRING, paquete_codigo_beso)) == 1){
	log_trace(logger,"Se envio BESO FILE al Kernel.");}

	free(fileBody);
	free(paquete_codigo_beso);

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
	log_trace(logger,"BESO FILE recibido con exito por el Kernel.");*/

	t_struct_string* beso_code = malloc(sizeof(t_struct_string));
	beso_code->string = fileBody;

	socket_enviar(sockKernel, FILE_RECV_SUCCESS, beso_code);

	//7- Quedar a la espera de mensajes para imprimir por STDOUT
	while(analizar_paquete(buffer, &idOperacion) == 1)
	{
		if(idOperacion == END_PROGRAM) {
			printf("[Programa]: Cerrando Programa...\n");
			log_trace(logger,"Cerrando Programa");
			break;
		}
	}

	// Destruimos la estructura config, cerramos socket y liberamos memoriaa
	liberarMemoria();

	return 0;
}

