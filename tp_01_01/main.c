/*
 * main.c
 *
 *  Created on: 27/08/2016
 *      Author: juan
 */

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
void sigalrm_handler();

typedef struct {
	pid_t pid_hijo;
	int vida;
	int demora;
} Info_hijo;

pid_t pid;

int main(int argc, char const *argv[]) {

	int cant_proc = 0, t_vida = 0, t_demora = 0, cont_hijos = 0, fin_prog = 0;
	pid_t pid_chld , pid_c;
	Info_hijo *hijos;
	pid = getpid(); // pid padre

	struct sigaction ctrl_nuevo;
	struct sigaction ctrl_viejo;



	//validacion de argumentos
	if (argc != 4) {
		fprintf(stderr, "Argumento insuficiente \n");
		exit(1);
	}

	if (argv[1] < 0) {
		fprintf(stderr, "Error en la cantidad de procesos ingresados\n");
		exit(1);
	} else {
		cant_proc = atoi(argv[1]);
		printf("se van a crear %d procesos hijos\n", cant_proc);

		hijos = (Info_hijo *) malloc(sizeof(Info_hijo) * cant_proc);
	}

	if (atoi(argv[2]) < 100) {
		fprintf(stderr, "Debe ingresar un tiempo de vida mayor a 100 ms\n");
		exit(1);
	} else {
		t_vida = atoi(argv[2]);
	}

	if (atoi(argv[3]) < 100 && atoi(argv[3]) != 0) {
		fprintf(stderr, "Debe ingresar una demora mayor a 100 ms o igual a 0\n");
		exit(1);
	} else {
		t_demora = atoi(argv[3]);
	}

	/*Reemplazo del controlador de SIGCHLD*/
	if (sigemptyset(&ctrl_nuevo.sa_mask) == ERROR) {
		perror("sigemptyset-sig_block_mask");
		free(hijos);
		exit(EXIT_FAILURE);
	}
	if (sigaddset(&ctrl_nuevo.sa_mask, SIGSTOP) == ERROR) {
		perror("sigaddset-sig_block_mask");
		free(hijos);
		exit(EXIT_FAILURE);
	}
	ctrl_nuevo.sa_flags = SA_SIGINFO | SA_NOCLDSTOP | SA_RESTART;
	ctrl_nuevo.sa_sigaction = sigchld_handler;

	if (sigaction(SIGCHLD, &ctrl_nuevo, &ctrl_viejo) == ERROR) {
		perror("signal-SIGCHLD, sigchld_handler");
		free(hijos);
		exit(EXIT_FAILURE);
	}



	//while (fin_prog) //el fing_programa se pone en uno cuando la tabla con los hijos activos se termina
	//{


	do {

		pid_chld = fork();


		if (pid_chld != 0) {

			printf("(%d) se creo un nuevo hijo con numero de PID :%d\n", pid,
					pid_chld);
			(hijos+cont_hijos)->pid_hijo = pid_chld;
			(hijos+cont_hijos)->demora = t_demora;
			(hijos+cont_hijos)->vida = t_vida;


			cont_hijos++;
			usleep(t_demora * 1000); //useep toma el tiempo en microsegundos

		} else { /*Codigo del hijo*/

			pid_c=getpid();
			printf("(%d) Soy un nuevo hijo\n", pid_c);
			printf("de la lista el pid es %d \n",(hijos+cont_hijos)->pid_hijo);

			usleep(t_vida * 1000);
			printf("(%d) Adios mundo cruel!\n", pid_c);

		}
	} while (cont_hijos < cant_proc && pid_chld != 0);
	//	}
	if (pid_chld != 0) {
		wait(NULL);
		printf("(%d) Tuve %d hijos, murieron todos, que desgracia!\n", pid,
				cont_hijos);
	}
	exit(1);

}

void sigchld_handler() {

	int status;
	pid_t rx_pid;
	 do
	{

	rx_pid = waitpid(-1, &status, WNOHANG|WUNTRACED|WCONTINUED);

	}while ((rx_pid == (pid_t)0) && (rx_pid != (pid_t)-1));

	 printf("(%d) Se murio mi hijo de pid: %d  \n", pid, rx_pid);

}
