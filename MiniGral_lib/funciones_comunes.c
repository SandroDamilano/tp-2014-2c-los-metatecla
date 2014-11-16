/*
 * funciones_comunes.c
 *
 *  Created on: 24/10/2014
 *      Author: utnso
 */

#include "sockets/enviar_recibir_paquete.h"

char* leer_archivo(FILE* archivo, long int tamanio_archivo){
	char* codigo = malloc(tamanio_archivo);

	if(archivo == NULL){
		return codigo; //Error
	}

	fseek(archivo, 0L, SEEK_SET);

	char buffer[tamanio_archivo];//Copio el contenido del archivo al buffer

	/*while(!feof(archivo)){
		fread(buffer, 1, tamanio_archivo, archivo);
	}*/

	fread(buffer, 1, tamanio_archivo, archivo);

	buffer[tamanio_archivo]= '\0';

	memcpy(codigo, buffer, tamanio_archivo);

	return codigo;
}

long int calcular_tamanio_archivo(FILE* archivo){
	fseek(archivo, 0L, SEEK_END); //Averiguo tamaño del archivo
	return ftell(archivo);
}

FILE* abrir_archivo(char* nombre_archivo, t_log* log, pthread_mutex_t *mutex){
	FILE* archivo = fopen(nombre_archivo, "r");

	if(archivo == NULL){
		//pthread_mutex_lock(mutex);
		printf("Error al abrir el archivo %s\n", nombre_archivo);
		//log_error(log, "Error al abrir el archivo %s",nombre_archivo);
		//pthread_mutex_unlock(mutex);
		free(nombre_archivo);
	}

	return archivo;
}

void copiar_structRecibido_a_tcb(t_hilo* tcb, void* structRecibido){
	tcb->base_stack = ((t_struct_tcb*) structRecibido)->base_stack;
	tcb->cola = ((t_struct_tcb*) structRecibido)->cola;
	tcb->cursor_stack = ((t_struct_tcb*) structRecibido)->cursor_stack;
	tcb->kernel_mode = ((t_struct_tcb*) structRecibido)->kernel_mode;
	tcb->pid = ((t_struct_tcb*) structRecibido)->pid;
	tcb->puntero_instruccion = ((t_struct_tcb*) structRecibido)->puntero_instruccion;
	tcb->registros[0] = ((t_struct_tcb*) structRecibido)->registros[0];
	tcb->registros[1] = ((t_struct_tcb*) structRecibido)->registros[1];
	tcb->registros[2] = ((t_struct_tcb*) structRecibido)->registros[2];
	tcb->registros[3] = ((t_struct_tcb*) structRecibido)->registros[3];
	tcb->registros[4] = ((t_struct_tcb*) structRecibido)->registros[4];
	tcb->segmento_codigo = ((t_struct_tcb*) structRecibido)->segmento_codigo;
	tcb->segmento_codigo_size = ((t_struct_tcb*) structRecibido)->segmento_codigo_size;
	tcb->tid = ((t_struct_tcb*) structRecibido)->tid;
}

void copiar_tcb_a_structTcb(t_hilo* tcb, t_struct_tcb* tcb_enviar){
	tcb_enviar->base_stack = tcb->base_stack;
	tcb_enviar->cola = tcb->cola;
	tcb_enviar->cursor_stack = tcb->cursor_stack;
	tcb_enviar->kernel_mode = tcb->kernel_mode;
	tcb_enviar->pid = tcb->pid;
	tcb_enviar->puntero_instruccion = tcb->puntero_instruccion;
	tcb_enviar->registros[0] = tcb->registros[0];
	tcb_enviar->registros[1] = tcb->registros[1];
	tcb_enviar->registros[2] = tcb->registros[2];
	tcb_enviar->registros[3] = tcb->registros[3];
	tcb_enviar->registros[4] = tcb->registros[4];
	tcb_enviar->segmento_codigo = tcb->segmento_codigo;
	tcb_enviar->segmento_codigo_size = tcb->segmento_codigo_size;
	tcb_enviar->tid = tcb->tid;
}

// para el caso de debug del paquete por pantalla de consola
int print_package_to_output(char* datos) {
	t_struct_string *deserializado = deserializeStruct_string(datos, 0);

	printf("%s\n", deserializado->string);
	free(deserializado->string);
	free(deserializado);

	return 0;
}

/*********************************************** DIRECCIONES *********************************************************/

uint32_t elevar(uint32_t numero, uint32_t elevado){
	int i;
	uint32_t resultado;
	resultado=numero;
	switch(elevado){
	case 0: resultado=1; break;
	case 1: resultado=numero; break;
	default: for(i=1;i<elevado;i++){resultado=resultado*numero;};

	}
return resultado;
}

// FIXME porque chilla
char *traducirABinario(uint32_t direccion, int cantidad_bits) {

	uint32_t binNumInv[cantidad_bits];
	uint32_t counter;

	uint32_t i;
	uint32_t aux = direccion;


	char bina[cantidad_bits];
	char bina_inv[cantidad_bits];

	for ( counter = 0 ; counter < cantidad_bits; counter++ ) {
	binNumInv[counter] = aux % 2;


	aux = aux / 2;

	if(binNumInv[counter] == 0){

	bina[counter] = '0'; } else { bina[counter] = '1';}
	}

	for (i=0;i<cantidad_bits;i++){
		counter --;
		bina_inv[counter]=bina[i];

	}
	return bina_inv;
	}


uint32_t traducirADecimal(char *binario, int cantidad_bits){

	int i;
	int j;
	uint32_t decimal = 0;
	for(i=cantidad_bits-1; i >= 0; i--){
		j= cantidad_bits-1-i;
		switch(binario[i]){
		case '0':
			break;
		case '1':
			decimal = elevar(2,j)+decimal;
			break;
		default:
			break;
		}
	}
	return decimal;
}

uint32_t crearDireccion(uint32_t segmento, uint32_t pagina, uint32_t desplazamiento){ //FIXME: seria copado que reciba el desplazamiento por parametro
	uint32_t direccionCreada;
	char *binSegmento;
	char *binPagina;
	char *binDesplazamiento;
	char *direccionCreadaBin = malloc(32);

	binSegmento=malloc(12);
	binPagina=malloc(13);
	binDesplazamiento=malloc(8);
	memcpy(binSegmento, traducirABinario(segmento, 12), 12);
	memcpy(binPagina, traducirABinario(pagina, 12), 12);
	memcpy(binDesplazamiento, traducirABinario(desplazamiento, 8), 8);

	memcpy(direccionCreadaBin, binSegmento, 12);
	memcpy(direccionCreadaBin+12, binPagina, 12);
	memcpy(direccionCreadaBin+24, binDesplazamiento, 8);

	direccionCreada=traducirADecimal(direccionCreadaBin, 32);

	free(binSegmento);
	free(binPagina);
	free(binDesplazamiento);
	free(direccionCreadaBin);
	return direccionCreada;
}

 t_direccion traducirDireccion(uint32_t unaDireccion){
	t_direccion direccionTraducida;
	char direccionEnBinario[32];
	memcpy(direccionEnBinario,traducirABinario(unaDireccion,32),32);

	char segmento[12];
	char pagina[12];
	char desplazamiento[8];

	memcpy(segmento, direccionEnBinario, 12);
	memcpy(pagina, direccionEnBinario + 12, 12);
	memcpy(desplazamiento, direccionEnBinario + 24, 8);

	(direccionTraducida).segmento=traducirADecimal(segmento, 12);
	(direccionTraducida).pagina=traducirADecimal(pagina, 12);
	(direccionTraducida).desplazamiento=traducirADecimal(desplazamiento, 8);

	return direccionTraducida;
}

uint32_t sumar_desplazamiento(uint32_t direccion, uint32_t desplazamiento){
	t_direccion direccion_traducida= traducirDireccion(direccion);
	direccion_traducida.desplazamiento += desplazamiento;
	while(direccion_traducida.desplazamiento>=256){
		direccion_traducida.pagina=direccion_traducida.pagina+1;
		direccion_traducida.desplazamiento=direccion_traducida.desplazamiento-256;
	}

	return crearDireccion(direccion_traducida.segmento, direccion_traducida.pagina, direccion_traducida.desplazamiento);
}

