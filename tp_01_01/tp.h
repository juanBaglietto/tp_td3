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
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <errno.h>
#include <sys/queue.h>

#define ERROR -1

void sigchld_handler();
void  sigalrm_handler();
void int_signals();
void validar_arg(int argc, char const *argv[]);
void set_t_repeticon(struct timeval *t_rep, int sec,int usec);

/*Varibales globales*/
typedef struct {
	pid_t pid_hijo;
	int tiempo_vida;
	int demora;
} Info_hijo;

#endif /* TP_H_ */
