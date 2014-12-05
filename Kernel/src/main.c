/*
 ============================================================================
 PROCESO KERNEL
 	 	 	 	.
 ============================================================================
 *	CICLO DEL PROCESO KERNEL
 *
 * 1- Inicializar panel ansisop
 * 2- Crear/Abrir file log del programa para el registro de actividades
 * 3- Leer archivo de configuracion y asignar valores
 * 4- Cargar argumentos para hilos LOADER y PLANIFICADOR
 * 5- Conectar a MSP
 * 6- Levantar HILOS
 * 	6.1- Hilo KERNEL
 * 	6.2- Hilo LOADER
 * 	6.3- Hilo PLANIFICADOR
 *
 *	TODO VER necesidad de almacenar vars en contenido del config_file (dictionary) PARECE SER INNECESARIO
 *
 */

#include "main.h"

// Variables para socket Cliente MSP
int socket_MSP, numbytes_clt;
struct addrinfo hints_clt, *servinfo_clt, *p_clt;
int rv_clt;
char s_clt[INET6_ADDRSTRLEN];

int main(int argc, char **argv){

	// 1- Inicializar panel ansisop
	logger = malloc(sizeof(t_log));
	inicializar_panel(KERNEL, "/home/utnso/tp-2014-2c-los-metatecla/");

	// 3- Leer archivo de configuracion y asignar valores
	leer_config();

	// 4- Cargar argumentos para hilos LOADER y PLANIFICADOR
	cargar_arg_LOADER(&param_LOADER);
	cargar_arg_PLANIFICADOR(&param_PLANIFICADOR);

//	// 5- Conectar a MSP
	conectar_a_MSP(ip_msp, puerto_msp);

	// 5.5- Inicializar semáforos de uso global y colas NEW y EXIT (y la de consolas)
	inicializar_semaforos();
	inicializar_colas_new_exit();

	inicializar_multiplex();

	// 6- Levantar HILOS
		//6.1- Levantar Hilo HANDSHAKE
	if((ret_KERNEL = pthread_create(&thread_KERNEL, NULL, (void*)&handshake_thread, NULL)))
	{
		fprintf(stderr, "[Kernel]: Error on 'pthread_create()' function - Hilo HANDSHAKE: %d\n", ret_KERNEL);
		sprintf(bufferLog,"[Kernel]: Error on 'pthread_create()' function - Hilo HANDSHAKE: %d", ret_KERNEL);
		log_error(logger, bufferLog);
		exit(EXIT_FAILURE);
	}

		//6.3- Levantar Hilo PLANIFICADOR
	if((ret_PLANIFICADOR = pthread_create(&thread_PLANIFICADOR, NULL, (void*)&main_PLANIFICADOR, &param_PLANIFICADOR)))
	{
		fprintf(stderr, "[Kernel]: Error on 'pthread_create()' function - Hilo PLANIFICADOR: %d\n", ret_PLANIFICADOR);
		sprintf(bufferLog,"[Kernel]: Error on 'pthread_create()' function - Hilo PLANIFICADOR: %d", ret_PLANIFICADOR);
		log_error(logger, bufferLog);
		exit(EXIT_FAILURE);
	}

	signal(SIGINT, llegoSenialParaTerminar);

	pthread_join(thread_KERNEL, NULL);
	pthread_join(thread_PLANIFICADOR, NULL);

	return 0;
}

void llegoSenialParaTerminar(int n){

	void destruir_block(t_data_nodo_block* data){
		free(data->tcb);
	}

	void destruir_exec(t_data_nodo_exec* data){
		free(data->tcb);
	}

	void destruir_exit(t_data_nodo_exit* data){
		free(data->tcb);
	}

	t_struct_numero* fin;
	switch(n){
	case SIGINT:
		fin = malloc(sizeof(t_struct_numero));
		fin->numero = 0;
		socket_enviar(socket_MSP, D_STRUCT_NUMERO, fin);
		free(fin);

		//printf("CTRL C!!!!!\n");
		list_destroy(cola_ready);
		list_destroy_and_destroy_elements(cola_block, (void*)destruir_block);
		list_destroy_and_destroy_elements(cola_exec, (void*)destruir_exec);
		queue_destroy(cola_new);
		queue_destroy_and_destroy_elements(cola_exit, (void*)destruir_exit);
		list_destroy(solicitudes_tcb);
		list_destroy(consolas);
		list_destroy(terminados);
		list_destroy(lista_abortar);

		FD_ZERO(&master_cpus);
		FD_ZERO(&master_consolas);

		log_destroy(logger);
		config_destroy(config_file);

		pthread_cancel(thread_KERNEL);
		pthread_cancel(thr_consumidor_new);
		pthread_cancel(thr_parca);
		pthread_cancel(thr_atencion_CPUs);
		pthread_cancel(thread_PLANIFICADOR);
		exit(EXIT_SUCCESS);

		break;
	}
}

void leer_config()
{
	config_file = config_create("config.cfg");
	pthread_mutex_lock(&mutex_log);
    log_info(logger,"Parseo y Extraccion de valores de archivo de configuracion");
    pthread_mutex_unlock(&mutex_log);

	if (config_keys_amount(config_file) != MAX_COUNT_OF_CONFIG_KEYS)
	{
		fprintf(stderr,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco. Chequear esto !!\n");
		fprintf(stderr,"Usage:\n> ./Kernel [PATH/config.cfg]\n");
	    log_error(logger,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco. Chequear esto !!");
		exit(1);
	}

	//PUERTO de escucha KERNEL
	if(config_has_property(config_file, "PUERTO"))
	{
		puerto_kernel = config_get_int_value(config_file, "PUERTO");

		sprintf(bufferLog, "PUERTO = [%d]", puerto_kernel);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr, "Falta key 'PUERTO' en archivo de configuracion.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'PUERTO' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//IP_KERNEL
	if(config_has_property(config_file, "IP_KERNEL"))	{
			ip_kernel = config_get_string_value(config_file,"IP_KERNEL");
	    	sprintf(bufferLog,"IP_KERNEL = [%s]", ip_kernel);
	    	log_debug(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'IP_KERNEL' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'IP_KERNEL' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//IP_MSP
	if(config_has_property(config_file, "IP_MSP"))
	{
		ip_msp = config_get_string_value(config_file, "IP_MSP");

		sprintf(bufferLog, "IP_MSP = [%s]", ip_msp);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr, "Falta key 'IP_MSP' en archivo de configuracion.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'IP_MSP' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//PUERTO_MSP
	if(config_has_property(config_file, "PUERTO_MSP"))
	{
		puerto_msp = config_get_int_value(config_file, "PUERTO_MSP");

		sprintf(bufferLog, "PUERTO_MSP = [%d]", puerto_msp);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr, "Falta key 'PUERTO_MSP' en archivo de configuracion.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'PUERTO_MSP' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//QUANTUM
	if(config_has_property(config_file, "QUANTUM"))
	{
		quantum = config_get_int_value(config_file, "QUANTUM");

		sprintf(bufferLog, "QUANTUM = [%d]", quantum);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr, "Falta key 'QUANTUM' en archivo de configuracion.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'QUANTUM' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//TAMANIO_STACK
	if(config_has_property(config_file, "TAMANIO_STACK"))
	{
		tamanio_stack = config_get_int_value(config_file, "TAMANIO_STACK");

		sprintf(bufferLog, "TAMANIO_STACK = [%d]", tamanio_stack);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr, "Falta key 'TAMANIO_STACK' en archivo de configuracion.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'TAMANIO_STACK' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//SYSCALLS
	if(config_has_property(config_file, "SYSCALLS"))
	{
		syscalls_path = config_get_string_value(config_file, "SYSCALLS");

		sprintf(bufferLog, "SYSCALLS = [%s]", syscalls_path);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr, "Falta key 'SYSCALLS' en archivo de configuracion.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'SYSCALLS' en archivo de configuracion. Programa abortado.");
		exit(1);
	}
}

void cargar_arg_LOADER(arg_LOADER* arg)
{
	arg->puerto_kernel = puerto_kernel;
	arg->puerto_msp = puerto_msp;
	arg->ip_msp = ip_msp;
	arg->tamanio_stack = tamanio_stack;
	arg->logger = logger;
}

void cargar_arg_PLANIFICADOR(arg_PLANIFICADOR* arg)
{
	arg->quantum = quantum;
	arg->syscalls_path = syscalls_path;
	arg->puerto_kernel = puerto_kernel;
	arg->logger = logger;
}

int crear_cliente_MSP(char* ip, uint32_t puerto){
	printf("[Kernel]: Probando de conectar a MSP por host [%s] y port [%d]\n", ip, puerto);
	socket_MSP = socket_crearYConectarCliente(ip, puerto);
	t_struct_numero* es_kernel = malloc(sizeof(t_struct_numero));
	es_kernel->numero = ES_KERNEL;
	socket_enviar(socket_MSP, D_STRUCT_NUMERO, es_kernel);
	free(es_kernel);
	return socket_MSP;
}

void conectar_a_MSP(char *ip, uint32_t puerto)
{
	//t_tipoEstructura operacion;

	if(bufferMSP != NULL)	{
		free(bufferMSP);
		bufferMSP = NULL;
	}

	if(crear_cliente_MSP(ip, puerto) == -1)	{// antes: != 0
		perror("[Kernel]: No se puede conectar a la MSP. Abortando.");
		exit(1);
	}

	printf("[Kernel]: Conexion a MSP establecida.\n");
	log_info(logger, "Conexion a MSP establecida.");
}

void inicializar_multiplex(){
	//consolas_fdmax = 0;
	//cpus_fdmax = 0;
	pthread_mutex_init(&mutex_master_consolas, NULL);
	pthread_mutex_init(&mutex_master_cpus, NULL);
	FD_ZERO(&master_cpus);
	FD_ZERO(&master_consolas);
}


fd_set combinar_master_fd(fd_set* master1, fd_set* master2, int maxfd){
	fd_set combinado;
	FD_ZERO(&combinado);
	int i;

	for(i=0; i <= maxfd; i++){
		if(FD_ISSET(i, master1) || FD_ISSET(i, master2)){
			FD_SET(i, &combinado);
		}
	}

	return combinado;
}

void handshake_thread(){
	int socket_escucha = socket_crearServidor(ip_kernel, puerto_kernel);
	fd_set read_fd;
	int maxfd = socket_escucha;
	int i;

		while(1){
			read_fd = combinar_master_fd(&master_cpus, &master_consolas, maxfd);
			FD_SET(socket_escucha, &read_fd);

			if (select(maxfd+1, &read_fd, NULL, NULL, NULL) == -1) {
				perror("select");
				exit(1);
			}

			for(i=0; i<=maxfd; i++){
				//Verifico si es un socket
				if(FD_ISSET(i, &read_fd)){
					//Verifico si el socket es de una CPU
					if(FD_ISSET(i, &master_cpus)){
						handler_cpu(i);
					}
					//Verifico si el socket es de una Consola
					if(FD_ISSET(i, &master_consolas)){
						handler_consola(i);
					}
					//Verifico si es el socket que escucha nuevas conexiones
					if(i==socket_escucha){
						handler_nuevas_conexiones(i, &maxfd);
					}
				}
			}//Fin for
		}//Fin while
}

void handler_nuevas_conexiones(int socket_escucha, int* maxfd){

	int socket_atendido = socket_aceptarCliente(socket_escucha);

	void * structRecibido;
	t_tipoEstructura tipoStruct;
	int resultado = socket_recibir(socket_atendido, &tipoStruct, &structRecibido);
	if(resultado == -1 || tipoStruct != D_STRUCT_NUMERO){
		printf("No se recibio correctamente a quien atiendo en el kernel\n");
	}

	t_struct_numero* paquete_quantum;

	switch(((t_struct_numero *)structRecibido)->numero){
	case ES_CONSOLA:
		pthread_mutex_lock(&mutex_log);
		conexion_consola(socket_atendido);
		pthread_mutex_unlock(&mutex_log);

		// Si es una consola
		FD_SET(socket_atendido, &master_consolas);

		break;

	case ES_CPU:
		pthread_mutex_lock(&mutex_log);
		conexion_cpu(socket_atendido);
		pthread_mutex_unlock(&mutex_log);

		// Si es una cpu
		FD_SET(socket_atendido, &master_cpus);

		paquete_quantum = malloc(sizeof(t_struct_numero));
		paquete_quantum->numero = quantum;
		socket_enviar(socket_atendido, D_STRUCT_NUMERO, paquete_quantum);
		free(paquete_quantum);

		break;
	}

	if (socket_atendido>*maxfd){
		*maxfd = socket_atendido;
	}

	free(structRecibido);
}
