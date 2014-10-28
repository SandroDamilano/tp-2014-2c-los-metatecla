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

#include "kernel.h"


int main(int argc, char **argv){

	// 1- Inicializar panel ansisop
	inicializar_panel(KERNEL, "/home/utnso/git/tp-2014-2c-los-metatecla/Kernel");

	// 2- Crear/Abrir file log del programa para el registro de actividades
	logger = log_create("kernel.log", "KERNEL", false, LOG_LEVEL_TRACE);
	log_info(logger,"Inicio de registro de actividades del Proceso KERNEL.");

	// 3- Leer archivo de configuracion y asignar valores
	leer_config();

	// 4- Cargar argumentos para hilos LOADER y PLANIFICADOR
	cargar_arg_LOADER(&param_LOADER);
	cargar_arg_PLANIFICADOR(&param_PLANIFICADOR);

//	// 5- Conectar a MSP
//	conectar_a_MSP();

	// 5.5- Inicializar semáforos de uso global y colas NEW y EXIT
	inicializar_semaforos();
	inicializar_colas_new_exit();

	// 6- Levantar HILOS
		//6.1- Levantar Hilo KERNEL
/*	if((ret_KERNEL = pthread_create(&thread_KERNEL, NULL, (void*)&main_KERNEL, &param_KERNEL))) //TODO: definir main_KERNEL
	{
		fprintf(stderr, "[Kernel]: Error on 'pthread_create()' function - Hilo KERNEL: %d\n", ret_KERNEL);
		sprintf(bufferLog,"[Kernel]: Error on 'pthread_create()' function - Hilo KERNEL: %d", ret_KERNEL);
		log_error(logger, bufferLog);
		exit(EXIT_FAILURE);
	}
*/
		//6.2- Levantar Hilo LOADER
/*	if((ret_LOADER = pthread_create(&thread_LOADER, NULL, (void*)&main_LOADER, &param_LOADER)))
	{
		fprintf(stderr, "[Kernel]: Error on 'pthread_create()' function - Hilo LOADER: %d\n", ret_LOADER);
		sprintf(bufferLog,"[Kernel]: Error on 'pthread_create()' function - Hilo LOADER: %d", ret_LOADER);
		log_error(logger, bufferLog);
		exit(EXIT_FAILURE);
	}
*/
		//6.3- Levantar Hilo PLANIFICADOR
	if((ret_PLANIFICADOR = pthread_create(&thread_PLANIFICADOR, NULL, (void*)&main_PLANIFICADOR, &param_PLANIFICADOR)))
	{
		fprintf(stderr, "[Kernel]: Error on 'pthread_create()' function - Hilo PLANIFICADOR: %d\n", ret_PLANIFICADOR);
		sprintf(bufferLog,"[Kernel]: Error on 'pthread_create()' function - Hilo PLANIFICADOR: %d", ret_PLANIFICADOR);
		log_error(logger, bufferLog);
		exit(EXIT_FAILURE);
	}

	pthread_join(thread_KERNEL, NULL);
	pthread_join(thread_LOADER, NULL);
	pthread_join(thread_PLANIFICADOR, NULL);

	return 0;
}

void leer_config()
{
	config_file = config_create("config.cfg");
    log_info(logger,"Parseo y Extraccion de valores de archivo de configuracion");

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
		puerto = config_get_int_value(config_file, "PUERTO");

		sprintf(bufferLog, "PUERTO = [%d]", puerto);
		log_debug(logger, bufferLog);
	} else {
		fprintf(stderr, "Falta key 'PUERTO' en archivo de configuracion.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'PUERTO' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	//IP_MSP
	if(config_has_property(config_file, "IP_MSP"))
	{
		ip_msp = config_get_string_value(config_file, "IP_MPS");

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
	arg->puerto = puerto;
	arg->ip_msp = ip_msp;
	arg->puerto_msp = puerto_msp;
	arg->tamanio_stack = tamanio_stack;
	arg->logger = logger;
}

void cargar_arg_PLANIFICADOR(arg_PLANIFICADOR* arg)
{
	arg->quantum = quantum;
	arg->syscalls_path = syscalls_path;
	arg->puerto = puerto;	// parece que puede tratarse de cualquiera (CPU/Consola)
	arg->logger = logger;
}
