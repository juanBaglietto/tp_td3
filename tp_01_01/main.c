/*
 * main.c
 *
 *  Created on: 27/08/2016
 *      Author: juan
 */

#include "main.h"

pid_t pid_padre;
int cant_procesos = 0;
int cont_hijos_muertos=0;
int tiempo_faltante;
int *tab_relanzar;
int  t_vida = 0, t_demora = 0;


Lista lista_hijos = NULL;
Lista lista_revivir_hijos = NULL;

/*Handler*/

void sigchld_handler() {

	int status;
	Info_hijo *encontrado;
	Info_hijo nuevo_hijo;

	pid_t rx_pid;
	do
	{

		rx_pid = waitpid(-1, &status, WNOHANG|WUNTRACED|WCONTINUED);


	}while ((rx_pid == (pid_t)0) && (rx_pid != (pid_t)-1));

	if((encontrado=Buscar_PID(lista_hijos,rx_pid))!=NULL){

		if(encontrado->tiempo_vida==0){
			printf("(%d) Murio con dignidad mi hijo (%d)\n",pid_padre,rx_pid);
			Borrar_PID(&lista_hijos,rx_pid);
		}
		else{
			printf("(%d) Murio antes de tiempo mi hijo (%d) le falta vivir (%d)\n",pid_padre,rx_pid,encontrado->tiempo_vida);
			nuevo_hijo.pid_hijo = 0;
			nuevo_hijo.demora = 0;
			nuevo_hijo.tiempo_vida =encontrado->tiempo_vida;

			Insertar_al_final(&lista_revivir_hijos,nuevo_hijo);
			Borrar_PID(&lista_hijos,rx_pid);
		}
	}


}

void  sigalrm_handler(){

 	 Decremetar_tiempo(lista_hijos);


}

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

		ctrl_nuevo_sigalrm.sa_flags = SA_SIGINFO  | SA_RESTART;
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

pid_t crear_hijo(int tiempo_vida){

	pid_t pid_hijo;
	pid_t my_pid_hijo;
	Info_hijo nuevo_hijo;

	pid_hijo=fork();

	if(pid_hijo==0)   /*codigo del hijo */
	{
		my_pid_hijo=getpid();
		printf("(%d) Soy un nuevo hijo\n", my_pid_hijo);


		usleep(tiempo_vida*1000);
		printf("(%d) Adios mundo cruel!\n", my_pid_hijo);
		exit(1);

	}
	else  				/*codigo del padre */
	{

		nuevo_hijo.pid_hijo = pid_hijo;
		nuevo_hijo.demora = t_demora;
		nuevo_hijo.tiempo_vida = tiempo_vida;

		Insertar_al_final(&lista_hijos,nuevo_hijo);
	}

	return	pid_hijo;

}
/*funcion encargada de relazar los proceos que se terminaron antes de tiempo
 * devuelve 0 si no se relanzo ningun proceoso o la contidad de proceoso relanzados */
void relanzar_procesos(){

	pid_t pid_hijo_revivido;
	pNodo aux=lista_revivir_hijos;

	if((ListaVacia(lista_revivir_hijos))==0){
		do{
			pid_hijo_revivido=crear_hijo(aux->hijo.tiempo_vida);
			printf("(%d) revivi a un hijo con pid %d y tiempo de vida %d \n",pid_padre,pid_hijo_revivido,aux->hijo.tiempo_vida);
			aux = aux->siguiente;
		}while(aux!=NULL && pid_hijo_revivido!=0);
		if(pid_hijo_revivido!=0){
			BorrarLista(&lista_revivir_hijos);
		}

	}


}


int main(int argc, char const *argv[]) {

	pid_t pid_chld;
	int cont_demora=0,cont_hijos=0;

	pid_padre = getpid();					 // pid padre
	int_signals();
	validar_arg(argc,argv);

	cant_procesos = atoi(argv[1]);
	t_vida = atoi(argv[2]);  				//el timpo de vida enta en ms y usleep usa us
	t_demora = atoi(argv[3]);

	ualarm(T_SIGALRM_USEG,T_SIGALRM_USEG);	//seteo para tener una interrpcion por sigalrm cada 1 ms

	do {
		pid_chld = crear_hijo(t_vida);

		if (pid_chld != 0) {

			printf("(%d) se creo un nuevo hijo con numero de PID :%d\n", pid_padre,pid_chld);
			cont_hijos++;

			for (cont_demora = 0; cont_demora < DIV_DEMORA; ++cont_demora) {
				usleep(t_demora * (1000/DIV_DEMORA)); //divido el tiempo de demora para que el padre pueda analizar la tabla para relanzar procesos
				relanzar_procesos();
			}
		}
	} while (cont_hijos < cant_procesos && pid_chld != 0);

	if (pid_chld != 0) {
		while((ListaVacia(lista_hijos))==0){
			relanzar_procesos();
			pause();
		}
		printf("(%d) Tuve %d hijos, murieron todos, que desgracia!\n", pid_padre,cont_hijos);
		printf("Imprimo lista_hijos\n");
		MostrarLista(lista_hijos);
		BorrarLista(&lista_hijos);
		exit(1);
	}
	return 0;
}


