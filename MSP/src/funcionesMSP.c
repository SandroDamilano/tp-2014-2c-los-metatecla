/*
 * funcionesMSP.c
 *
 *  Created on: 24/09/2014
 *      Author: utnso
 */
#include "funcionesMSP.h"

t_config* archConfigMSP;



void leerConfiguracion(){

	archConfigMSP= config_create("config.cfg");


	if(config_has_property(archConfigMSP, "CANTIDAD_MEMORIA")){
			tamanio_mem_ppal = config_get_int_value(archConfigMSP,"CANTIDAD_MEMORIA");}
	else {
				printf("No esta definida la cantidad de memoria principal en el archivo\n");
				exit(0);
			}
	if(config_has_property(archConfigMSP, "PUERTO")){
			puertoMSP = config_get_int_value(config_file, "PUERTO");}
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
     memset(unaMemoria, '\0', tamanioMemoria );
     return unaMemoria;
}


