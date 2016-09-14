/*
 * tp.h
 *
 *  Created on: 02/09/2016
 *      Author: juan
 */

#ifndef TP_H_
#define TP_H_

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <errno.h>
#include "lista.h"
#include "debug.h"
#include "semaforos.h"

#define ERROR 				-1
#define DIV_DEMORA 			10
#define T_SIGALRM_USEG		1000
#define	DIV_VIDA            4
#define NOM_ARCHIVO			"registro.log"
#define BUFF_SIZE 			1024

void sigchld_handler();
void  sigalrm_handler();
void sigusr1_handler();
void sigalrm_handler_hijos();
void int_signals();
void validar_arg(int argc, char const *argv[]);
pid_t crear_hijo(int);
void relanzar_procesos(void);

/*Varibales globales*/



#endif /* TP_H_ */
