/*
 * epoll.h
 *
 *  Created on: 11/10/2014
 *      Author: utnso
 */

#ifndef EPOLL_H_
#define EPOLL_H_

#include "enviar_recibir_paquete.h"

#include <sys/epoll.h>
#include <sys/ioctl.h>

#define MAX_EVENTS_EPOLL 60

int epoll_crear(void);
int epoll_agregarSocketServidor(int epollfd, int socketCliente);
int epoll_agregarSocketCliente(int epollfd, int socketCliente);
int epoll_agregarFdInotify(int epollfd, int fdInotify);
int epoll_escucharBloqueante(int epollfd, struct epoll_event * events);
int epoll_escucharTimeOut(int epollfd, struct epoll_event * events, int timeout);
int epoll_escucharGeneral(int epollfd, int uniqueSocket, void(*uniqueHandler)(epoll_data_t), void(*normalHandler)(epoll_data_t), void(*closeHandler)(epoll_data_t));
int epoll_escucharGeneralTimeOut(int epollfd, int uniqueSocket, void(*uniqueHandler)(epoll_data_t), void(*normalHandler)(epoll_data_t), void(*closeHandler)(epoll_data_t), int timeOut);

#endif /* EPOLL_H_ */
