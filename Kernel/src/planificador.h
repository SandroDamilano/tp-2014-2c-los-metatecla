/*
 * planificador.h
 *
 *  Created on: 06/10/2014
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

	#include <ansisop-panel/panel.h>
	#include <commons/log.h>
	#include "funciones_kernel.h"

	int sockfd_cte;//sockMSP;
	int cantidad_de_PIDs;
	int cantidad_de_TIDs;

	extern uint32_t direccion_codigo_syscalls;
	extern uint32_t direccion_stack_syscalls;

	t_queue* cola_new;
	sem_t sem_new;
	pthread_mutex_t mutex_new;

	t_list* consolas;
	pthread_mutex_t mutex_consolas;

	t_list* solicitudes_tcb;

	fd_set master_cpus;
	int cpus_fdmax;
	pthread_mutex_t mutex_master_cpus;

	typedef struct arg_PLANIFICADOR { // Estructura para pasar argumentos al hilo
		uint32_t quantum;
		char* syscalls_path;
		uint32_t puerto_kernel;
		t_log* logger;
	} arg_PLANIFICADOR;

	typedef enum {TCBKM, SYSTCALL, JOIN, SEM} t_evento;

	typedef struct data_nodo_block { //Estructura de la cola de BLOCK
		t_hilo * tcb;
		t_evento evento;
		uint32_t parametro;
	} t_data_nodo_block;

	typedef struct data_nodo_exec {
		t_hilo * tcb;
		int sock;
	} t_data_nodo_exec;

	//Por ahora estas van aca porque el planificador es el único que las usa
	t_list *cola_ready, *cola_block, *cola_exec; //Colas de planificación.

	pthread_mutex_t mutex_ready, mutex_block, mutex_exec;

	//Variables de búsqueda en las colas
	//block
	uint32_t tid_a_buscar; //Para encontrar al TCB que hizo la systcall
	uint32_t parametro_a_buscar; //Puede ser un tid o un recurso (semáforo)
	t_evento evento_a_buscar;

	//exec
	int sockCPU_a_buscar;

	//consolas
	uint32_t tid_de_consola;
	uint32_t pid_de_consola;
	uint32_t pid_a_eliminar;

	void encolar_en_ready(t_hilo* tcb);
	void sacar_de_new(t_hilo* tcb);
	void mandar_a_exit(t_hilo* tcb, t_fin fin);
	t_hilo* obtener_tcb_a_ejecutar();

	void desbloquear_por_semaforo(uint32_t sem);
	void desbloquear_por_join(uint32_t tid);
	t_hilo* desbloquear_tcbSystcall(uint32_t tid);
	t_hilo* desbloquear_tcbKernel();

	void bloquear_tcbSemaforo(t_hilo* tcb, uint32_t sem);
	void bloquear_tcbSystcall(t_hilo* tcb, uint32_t dir_systcall);
	void bloquear_tcbJoin(t_hilo* tcb, uint32_t tid);
	void bloquear_tcbKernel(t_hilo* tcb);

	bool es_el_tcbCPU(t_data_nodo_exec* data);

	void inicializar_ready_block();
	void inicializar_semaforos_colas();
	void boot(char* systcalls_path);

	void poner_new_a_ready();

	void terminar_TCBs();
	bool es_padre(t_hilo* tcb);
	void escribir_consola(uint32_t pid, char* mensaje);
	void terminar_proceso(t_hilo* tcb);
	void terminar_hilo(t_hilo* tcb);
	void liberar_memoria(t_hilo* tcb);
	void eliminar_ready(uint32_t pid);
	void eliminar_block(uint32_t pid);
	void eliminar_exec(uint32_t pid);
	void sacar_de_consolas(uint32_t pid);
	bool es_el_tid_consola(t_data_nodo_consolas* data);
	bool es_el_pid_consola(t_data_nodo_consolas* data);
	bool es_el_pid_ready(t_hilo* tcb);
	bool es_el_pid_block(t_data_nodo_block* data);
	bool es_el_pid_exec(t_data_nodo_exec* data);

	bool esta_por_systcall(t_data_nodo_block* data);

	void mandar_a_ejecutar(t_hilo* tcb, int sockCPU);
	void atender_solicitudes_pendientes();
	void atender_cpus();
	void handler_cpu(int sockCPU);

	void* main_PLANIFICADOR(arg_PLANIFICADOR* parametros);

#endif /* PLANIFICADOR_H_ */
