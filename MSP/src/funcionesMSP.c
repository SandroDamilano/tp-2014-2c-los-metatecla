/*
 * funcionesMSP.c
 *
 *  Created on: 24/09/2014
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>

t_config* archConfigMSP;

void leerConfiguracion(){

	archConfigMSP= config_create("config.cfg");
	int puertoMSP, cant_mem, cant_swap;
	char* sust_pags;


	if(config_has_property(archConfigMSP, "CANTIDAD_MEMORIA")){
			cant_mem_ppal = config_get_int_value(archConfigMSP,"CANTIDAD_MEMORIA");
	} else {
				printf("No esta definida la cantidad de memoria principal en el archivo\n");
				exit(0);
			}
	if(config_has_property(archConfigMSP, "PUERTO")){
			puertoMSP = config_get_int_value(config_file, "PUERTO");
	} else {
			printf("No esta definido el puerto en el archivo\n");
			exit(0);
		}

	if(config_has_property(archConfigMSP, "CANTIDAD_SWAP")){
			cant_mem_swap = config_get_int_value(archConfigMSP,"CANTIDAD_SWAP");
	} else {
				printf("No esta definido la cantidad de memoria swap en el archivo\n");
				exit(0);
			}

	if(config_has_property(archConfigMSP, "SUST_PAGS")){
			alg_sustitucion = config_get_string_value(archConfigMSP,"SUST_PAGS");
	} else {
					printf("No esta definido el algoritmo de sustitucion de paginas en el archivo\n");
					exit(0);
			}

	//Faltan logs y alguna que otra validacion


}
