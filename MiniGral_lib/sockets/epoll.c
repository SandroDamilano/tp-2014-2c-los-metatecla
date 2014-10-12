/*
 * epoll.c
 *
 *  Created on: 11/10/2014
 *      Author: utnso
 */

#include "epoll.h"

/*
 * Nombre: socket_crearEpoll/0
 * Argumentos:
 * 		- NUNGUNA
 *
 * Devuelve:
 * 		int (Descriptor al epoll).
 *
 * Funcion: Crea un file descriptor de un epoll.
 */
int epoll_crear(void) {
	int epollfd = epoll_create(MAX_EVENTS_EPOLL);
	if (epollfd == -1) {
		perror("Creacion de epoll"); 	// CAMBIAR POR UN LOG
		return -1;
	}

	return epollfd;
}

/*
 * Nombre: epoll_agregarSocketServidor/2
 * Argumentos:
 * 		- epollfd (int) (file descriptor de un epoll)
 * 		- socketEscucha (int) (file descripor de un socket a agregar al epoll)
 *
 * Devuelve:
 * 		int (-1 si hay error, 0 si no hay error).
 *
 * Funcion: Agrega el socket a un epoll con evento tipo EPOLLIN.
 */
int epoll_agregarSocketServidor(int epollfd, int fdInotify) {
	struct epoll_event ev;
	int ret;
	memset(&ev,'\0',sizeof(struct epoll_event));
	ev.events = EPOLLIN;
	ev.data.fd = fdInotify;

	ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fdInotify, &ev);
	if (ret == -1) {
		perror("epoll_ctl: socket servidor");	// CAMBIAR POR UN LOG
		return -1;
	}
	return 0;
}

/*
 * Nombre: epoll_agregarFdInotify/2
 * Argumentos:
 * 		- epollfd (int) (file descriptor de un epoll)
 * 		- fdInotify (int) (file descripor de un inotify a agregar al epoll)
 *
 * Devuelve:
 * 		int (-1 si hay error, 0 si no hay error).
 *
 * Funcion: Agrega el socket a un epoll con evento tipo EPOLLIN y EPOLLPRI.
 */
int epoll_agregarFdInotify(int epollfd, int fdInotify) {
	struct epoll_event ev;
	int ret;
	memset(&ev,'\0',sizeof(struct epoll_event));
	ev.events = EPOLLIN | EPOLLPRI;
	ev.data.fd = fdInotify;

	ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fdInotify, &ev);
	if (ret == -1) {
		perror("epoll_ctl: socket servidor");
		return -1;
	}
	return 0;
}

/*
 * Nombre: epoll_agregarSocketCliente/2
 * Argumentos:
 * 		- epollfd (int) (file descriptor de un epoll)
 * 		- socketEscucha (int) (file descripor de un socket a agregar al epoll)
 *
 * Devuelve:
 * 		int (-1 si hay error, 0 si no hay error).
 *
 * Funcion: Agrega el socket a un epoll con evento tipo EPOLLIN y EPOLLET.
 */
int epoll_agregarSocketCliente(int epollfd, int socketCliente) {
	struct epoll_event ev;
	int ret;
	memset(&ev,'\0',sizeof(struct epoll_event));
	ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
	ev.data.fd = socketCliente;

	ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, socketCliente, &ev);
	if (ret == -1) {
		perror("epoll_ctl: socket cliente");	// CAMBIAR POR UN LOG
		return -1;
	}
	return 0;
}

/*
 * Nombre: epoll_escucharBloqueante/2
 * Argumentos:
 * 		- epollfd (int) (file descriptor de un epoll)
 * 		- events (struct epoll_event *) (puntero a una estructura de evento epoll, va a ser modificado)
 *
 * Devuelve:
 * 		int (cantidad de eventos detectados, -1 si hay error, 0 si no hubo ninguno).
 * 		--> modifica events (pasa todos los file descriptors que se tubieron evento).
 *
 * Funcion: Invoca a la funcion epoll_wait y se fija si hubo conexiones en algun file descriptor del epoll.
 */
int epoll_escucharBloqueante(int epollfd, struct epoll_event * events){
	int numeroFd;
	numeroFd = epoll_wait(epollfd, events, MAX_EVENTS_EPOLL, -1);

	if (numeroFd == -1) {
		perror("epoll_wait");	// CAMBIAR POR UN LOG
	}

	return numeroFd;
}

/*
 * Nombre: epoll_escucharTimeOut/3
 * Argumentos:
 * 		- epollfd (int) (file descriptor de un epoll)
 * 		- events (struct epoll_event *) (puntero a una estructura de evento epoll, va a ser modificado)
 * 		- timeout (int) (tiempo que va a ser bloqueante la funcion)
 *
 * Devuelve:
 * 		int (cantidad de eventos detectados, -1 si hay error, 0 si no hubo ninguno).
 * 		--> modifica events (pasa todos los file descriptors que se tubieron evento).
 *
 * Funcion: Invoca a la funcion epoll_wait y se fija si hubo conexiones en algun file descriptor del epoll.
 */
int epoll_escucharTimeOut(int epollfd, struct epoll_event * events, int timeout){
	int numeroFd;
	numeroFd = epoll_wait(epollfd, events, MAX_EVENTS_EPOLL, timeout);

	if (numeroFd == -1) {
		perror("epoll_wait");	// CAMBIAR POR UN LOG
	}

	return numeroFd;
}


/*
 * Nombre: epoll_escucharGeneral/5
 * Argumentos:
 * 		- epollfd (int) (file descriptor de un epoll)
 * 		- uniqueSocket (int) (si no se requiere, poner -1)
 * 		- uniqueHandler (funcion que le pasas epoll_data_t y devuelve void) (si uniqueSocket == -1, poner NULL)
 * 		- normalHandler (funcion que le pasas epoll_data_t y devuelve void) (NECESARIO 100%)
 * 		- closeHandler (funcion que le pasas epoll_data_t y devuelve void) (Puede ser NULL)
 *
 * Devuelve:
 * 		int (-1 si hay error, 0 si no hubo ninguno).
 *
 * Funcion: Invoca a la funcion epoll_wait y se fija si hubo conexiones en algun file descriptor del epoll.
 * Trata a cada fd segun las funciones.
 * uniqueHandler trata si uniqueSocket tubo actividad. Generalmente usado para servidores. Si no se quiere utilizar, uniqueSocket poner -1 y uniqueHandler NULL.
 * closeHandler trata si un socket se cerro. Puede ser NULL (lo que hace es cerrar al socket nada mas). Si se tiene un puntero asociado, hacer una funcion que haga algo.
 * normalHandler trata a cualquier socket que no haya caido en ninguna de las otras... Este es 100% necesario.
 */
int epoll_escucharGeneral(int epollfd, int serverSocket, void(*serverHandler)(epoll_data_t), void(*normalHandler)(epoll_data_t), void(*closeHandler)(epoll_data_t)){
	struct epoll_event events[MAX_EVENTS_EPOLL];
	int nfds, n;
	nfds = epoll_escucharBloqueante(epollfd, events);
	if (nfds == -1) {
		if (errno == EINTR ){
			return epoll_escucharGeneral(epollfd, serverSocket, serverHandler, normalHandler, closeHandler);
		}
		return -1;
	}

	for (n = 0; n < nfds; ++n) {
		if (events[n].data.fd == serverSocket) { 	// Si uniqueSocket == -1, nunca va a entrar porque los fd son >1.
			serverHandler(events[n].data);	// Si uniqueSocket == -1. Esto no se va a usar. Asi que puede ser NULL.
		}
		else {
			if( events[n].events & EPOLLRDHUP) {
				socket_cerrarConexion(events[n].data.fd); //No hace falta sacarlo de epoll, este lo hace solo.
				if (closeHandler != NULL) { //Si closeHandler era NULL automaticamente cierro la coneccion, sino invoco a la funcion.
					closeHandler(events[n].data);
				}
			}
			else {
				normalHandler(events[n].data);	// SI no cae en los dos casos anteriores, si o si cae aca.
			}
		}
	}

	return 0;
}


/*
 * Nombre: epoll_escucharGeneral/6
 * Argumentos:
 * 		- epollfd (int) (file descriptor de un epoll)
 * 		- uniqueSocket (int) (si no se requiere, poner -1)
 * 		- uniqueHandler (funcion que le pasas epoll_data_t y devuelve void) (si uniqueSocket == -1, poner NULL)
 * 		- normalHandler (funcion que le pasas epoll_data_t y devuelve void) (NECESARIO 100%)
 * 		- closeHandler (funcion que le pasas epoll_data_t y devuelve void) (Puede ser NULL)
 * 		- timeOut cuanto tiempo tarda hasta que se desbloquee
 *
 * Devuelve:
 * 		int (-1 si hay error, 0 si no hubo ninguno).
 *
 * Funcion: Invoca a la funcion epoll_wait y se fija si hubo conexiones en algun file descriptor del epoll.
 * Trata a cada fd segun las funciones.
 * uniqueHandler trata si uniqueSocket tubo actividad. Generalmente usado para servidores. Si no se quiere utilizar, uniqueSocket poner -1 y uniqueHandler NULL.
 * closeHandler trata si un socket se cerro. Puede ser NULL (lo que hace es cerrar al socket nada mas). Si se tiene un puntero asociado, hacer una funcion que haga algo.
 * normalHandler trata a cualquier socket que no haya caido en ninguna de las otras... Este es 100% necesario.
 */
int epoll_escucharGeneralTimeOut(int epollfd, int serverSocket, void(*serverHandler)(epoll_data_t), void(*normalHandler)(epoll_data_t), void(*closeHandler)(epoll_data_t), int timeOut){
	struct epoll_event events[MAX_EVENTS_EPOLL];
	int nfds, n;
	nfds = epoll_escucharTimeOut(epollfd, events, timeOut);
	if (nfds == -1) {
		if (errno == EINTR ){
			return epoll_escucharGeneralTimeOut(epollfd, serverSocket, serverHandler, normalHandler, closeHandler, timeOut);
		}
		return -1;
	}

	for (n = 0; n < nfds; ++n) {
		if (events[n].data.fd == serverSocket) { 	// Si uniqueSocket == -1, nunca va a entrar porque los fd son >1.
			serverHandler(events[n].data);	// Si uniqueSocket == -1. Esto no se va a usar. Asi que puede ser NULL.
		}
		else {
			if( events[n].events & EPOLLRDHUP) {
				socket_cerrarConexion(events[n].data.fd); //No hace falta sacarlo de epoll, este lo hace solo.
				if (closeHandler != NULL) { //Si closeHandler era NULL automaticamente cierro la coneccion, sino invoco a la funcion.
					closeHandler(events[n].data);
				}
			}
			else {
				normalHandler(events[n].data);	// SI no cae en los dos casos anteriores, si o si cae aca.
			}
		}
	}

	return 0;
}

