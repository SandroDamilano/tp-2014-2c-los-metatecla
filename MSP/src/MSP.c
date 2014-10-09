/*
 ============================================================================
 Name        : MSP.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "MSP.h"
#include "ConsolaMSP.h"
#include "funcionesMSP.h"




int main(int argc, char *argv[]) {

	//Variables de Memoria Principal
	void *memoria_ppal = NULL;

	//Variables de estructuras administrativas
	t_list *listaProcesos = list_create();
	t_list *lista_marcos = NULL;

	//Variables para hilos
	pthread_t consola;

	//Variables para Sockets
	int socketServidorMSP;
	bool escuchandoConexiones = true;

	//1.Leer archivo de configuracion

	leerConfiguracion();

	//2. Reservar bloque de memoria principal

	memoria_ppal=reservarBloquePpal(tamanio_mem_ppal);

	//TODO log "se creo memoria princial y abaca desde memoria_ppal hasta memoria_ppal+tamanio

	//3. Generar estructuras administrativas

	lista_marcos=dividirMemoriaEnMarcos(memoria_ppal, tamanio_mem_ppal);

	//TODO log lista de procesos, tabla de paginas y tabla de segmentos

	//4. Abrir conexiones con Kernel y CPU, y levantar Consola MSP

	//Se crea el hilo para la consola
		pthread_create(&consola, NULL, inciarConsola, NULL); //TODO pasar info de la memoria a la consola (4 parametro diferente a null)

		//Se crea socket servidor de la MSP

		socketServidorMSP= socket_crearServidor("127.0.0.1", puertoMSP);

		//TODO LOG Se abren conexiones por sockets para Kernel y CPU

		//Msp a la escucha de nuevas conexiones

	while(escuchandoConexiones){ //TODO log esperando nuevas conexiones

		pthread_t *hiloDeConexion = malloc (sizeof(pthread_t)); // Hilo que va a tratar la nueva conexion
		int *nuevoSocket = malloc(sizeof(int)); //Puntero al socket que va a atender la nueva conexion

		t_info_mem *informacion_memoria= malloc(sizeof(t_info_mem)); //Informacion de memoria que va a utilizar el hilo
		(*informacion_memoria).algoritmo_sust=alg_sustitucion;
		(*informacion_memoria).lista_marcos=lista_marcos;
		(*informacion_memoria).tamanio_mem_ppal=tamanio_mem_ppal;
		(*informacion_memoria).tamanio_swap=cant_mem_swap;

		t_conexion_entrante *nuevaConexion = malloc(sizeof(t_conexion_entrante)); //Informacion para el hilo de conexion
		(*nuevaConexion).hiloDeConexion=hiloDeConexion;
		(*nuevaConexion).informacion_memoria=informacion_memoria;
		(*nuevaConexion).socket=nuevoSocket;

		*nuevoSocket=socket_aceptarCliente(socketServidorMSP);

		pthread_create(hiloDeConexion,NULL,inciarConsola,(void *) nuevaConexion); //TODO donde dice inciarConsola va una funcion que maneje el pedido de la conexion
		pthread_detach(*hiloDeConexion);
	}

		//Espera que se termine el hilo consola
		pthread_join(consola, NULL);

		//TODO Liberar recursos

	return EXIT_SUCCESS;
}
