/*
 * funcionesMSP.c
 *
 *  Created on: 24/09/2014
 *      Author: utnso
 */
#include "funcionesMSP.h"

t_config* archConfigMSP;
int puertoMSP, tamanio_mem_ppal, cant_mem_swap;
char* alg_sustitucion;

void leerConfiguracion(){


	archConfigMSP= config_create("config.cfg");


	if(config_has_property(archConfigMSP, "CANTIDAD_MEMORIA")){
			tamanio_mem_ppal = config_get_int_value(archConfigMSP,"CANTIDAD_MEMORIA");}
	else {
				printf("No esta definida la cantidad de memoria principal en el archivo\n");
				exit(0);
			}
	if(config_has_property(archConfigMSP, "PUERTO")){
			puertoMSP = config_get_int_value(archConfigMSP, "PUERTO");}
	else {
			printf("No esta definido el puerto en el archivo\n");
			exit(0);
		}

	if(config_has_property(archConfigMSP, "CANTIDAD_SWAP")){
			cant_mem_swap = config_get_int_value(archConfigMSP,"CANTIDAD_SWAP");}
	else {
				printf("No esta definido la cantidad de memoria swap en el archivo\n");
				exit(0);
			}

	if(config_has_property(archConfigMSP, "SUST_PAGS")){
			alg_sustitucion = config_get_string_value(archConfigMSP,"SUST_PAGS");}
	else {
					printf("No esta definido el algoritmo de sustitucion de paginas en el archivo\n");
					exit(0);
			}

	//Faltan logs y alguna que otra validacion


}
void *reservarBloquePpal(int tamanioMemoria){

     void *unaMemoria = malloc(tamanioMemoria);
     if(NULL == unaMemoria){
    	 //log "no se pudo crear memoria principal"
         //liberar recursos
    	 exit(0);
     }
     return unaMemoria;
}
t_list *dividirMemoriaEnMarcos(void *memoria, int tamanioMemoria){
	t_marco *marco;
	(*marco).memoria=NULL;
	(*marco).marco_libre= false;
	t_list *lista_marcos = list_create();
	int cant_marcos = (tamanioMemoria*1024)/256;
	int i ;
	for(i=0; i<cant_marcos; i++)
	{
		(*marco).memoria = memoria + i*256;
		list_add(lista_marcos, marco);
		(*marco).memoria = NULL;
	}
return lista_marcos;
}
