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

pid_t pid_padre;
Info_hijo *hijos;
int cant_procesos = 0;

/*Funciones*/

void int_signals(){

	/*Reemplazo del controlador de SIGCHLD*/

	struct sigaction ctrl_nuevo_sigchld;
	struct sigaction ctrl_viejo_sigchld;
	struct sigaction ctrl_nuevo_sigalrm;
	struct sigaction ctrl_viejo_sigalrm;

		if (sigemptyset(&ctrl_nuevo_sigchld.sa_mask) == ERROR) {
			perror("sigemptyset-sig_block_mask");

			exit(EXIT_FAILURE);
		}
		if (sigaddset(&ctrl_nuevo_sigchld.sa_mask, SIGSTOP) == ERROR) {
			perror("sigaddset-sig_block_mask");

			exit(EXIT_FAILURE);
		}
		ctrl_nuevo_sigchld.sa_flags = SA_SIGINFO | SA_NOCLDSTOP | SA_RESTART;
		ctrl_nuevo_sigchld.sa_sigaction = sigchld_handler;

		if (sigaction(SIGCHLD, &ctrl_nuevo_sigchld, &ctrl_viejo_sigchld) == ERROR) {
			perror("signal-SIGCHLD, sigchld_handler");

			exit(EXIT_FAILURE);
		}
		/*Reemplazo del controlador de SIGALRM*/


		ctrl_nuevo_sigalrm.sa_sigaction =  sigalrm_handler;

		if (sigaction( SIGALRM, &ctrl_nuevo_sigalrm, &ctrl_viejo_sigalrm) == ERROR) {
					perror("signal-SIGALRM,  sigalrm_handle");

					exit(EXIT_FAILURE);
				}
}

//validacion de argumentos
void validar_arg(int cant_arg, char const *arg_v[]){

		if (cant_arg!= 4) {
			fprintf(stderr, "Argumento insuficiente \n");
			exit(1);
		}

		if (arg_v[1] < 0) {
			fprintf(stderr, "Error en la cantidad de procesos ingresados\n");
			exit(1);
		}

		if (atoi(arg_v[2]) < 100) {
			fprintf(stderr, "Debe ingresar un tiempo de vida mayor a 100 ms\n");
			exit(1);
		}

		if (atoi(arg_v[3]) < 100 && atoi(arg_v[3]) != 0) {
			fprintf(stderr, "Debe ingresar una demora mayor a 100 ms o igual a 0\n");
			exit(1);
		}

}

/*Handler*/

void sigchld_handler() {

	int status,cont=0;
	pid_t rx_pid;
	 do
	{

	rx_pid = waitpid(-1, &status, WNOHANG|WUNTRACED|WCONTINUED);

	for (cont = 0; cont < cant_procesos; cont++) {

		if ((hijos+cont)->pid_hijo== rx_pid) {    //es un proceso hijo
			if((hijos+cont)->tiempo_vida==0){

				printf("(%d) Mi hijo %d murio con dignidad",pid_padre,rx_pid);
			}
			else{
				printf("(%d) Mi hijo %d antes de tiempo, le falta vivir %d",pid_padre,rx_pid,(hijos+cont)->tiempo_vida);
			}

		}else
			break;


	}

	}while ((rx_pid == (pid_t)0) && (rx_pid != (pid_t)-1));

	 printf("(%d) Se murio mi hijo de pid: %d  \n",pid_padre, rx_pid);


}

void  sigalrm_handler(){

	int cont;

	for (cont = 0; cont < cant_procesos; cont++) {

			if ((hijos+cont)->pid_hijo!=0) {    //es un proceso hijo

				(hijos+cont)->tiempo_vida=(hijos+cont)->tiempo_vida-1;
			}
			else
				break;

	}
}

/*funcion que carga la estructura que utiliza la funcion setitimer para general la señal alarma
 * parametros:  struct timeval
 *				int sec
 *				int int usec 																	*/

void set_t_repeticon(struct timeval *t_nuevo, int sec,int usec){

	t_nuevo->tv_sec=sec;
	t_nuevo->tv_usec=usec;


}


int main(int argc, char const *argv[]) {

	int  t_vida = 0, t_demora = 0, cont_hijos = 0;
	pid_t pid_chld , pid_c;
	struct itimerval contador;
	struct timeval tiempoRepeticion;

	pid_padre = getpid(); // pid padre

	set_t_repeticon(&tiempoRepeticion,0,1000); // creo una señal de alarma cada 1 ms

	contador.it_value=tiempoRepeticion;
	contador.it_interval=tiempoRepeticion;

	int_signals();
	validar_arg(argc,argv);

	cant_procesos = atoi(argv[1]);
	hijos = (Info_hijo *) malloc(sizeof(Info_hijo) * cant_procesos);
	t_vida = atoi(argv[2]);
	t_demora = atoi(argv[3]);

	setitimer (ITIMER_REAL, &contador, 0); // antes de inicial el ciclo de creacion de hijos inicio el contador para que emita la señal alarma cada 1ms

	do {

		pid_chld = fork();


		if (pid_chld != 0) {

			printf("(%d) se creo un nuevo hijo con numero de PID :%d\n", pid_padre,
					pid_chld);
			(hijos+cont_hijos)->pid_hijo = pid_chld;
			(hijos+cont_hijos)->demora = t_demora;
			(hijos+cont_hijos)->tiempo_vida = t_vida;

			printf("(%d) de la lista el pid del hijo creado es %d y va a vivir %d ms \n",pid_padre,(hijos+cont_hijos)->pid_hijo,(hijos+cont_hijos)->tiempo_vida);

			cont_hijos++;
			usleep(t_demora * 1000); //useep toma el tiempo en microsegundos

		} else { /*Codigo del hijo*/

			pid_c=getpid();
			printf("(%d) Soy un nuevo hijo\n", pid_c);


			usleep(t_vida * 1000);
			printf("(%d) Adios mundo cruel!\n", pid_c);

		}
	} while (cont_hijos < cant_procesos && pid_chld != 0);

	if (pid_chld != 0) {

		wait(0);
		printf("(%d) Tuve %d hijos, murieron todos, que desgracia!\n", pid_padre,
				cont_hijos);
	}

	exit(1);

}


