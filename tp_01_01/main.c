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


Lista lista = NULL;

/*Handler*/

void sigchld_handler() {

	int status,cont=0,i=0;
	pid_t rx_pid;
	 do
	{

	rx_pid = waitpid(-1, &status, WNOHANG|WUNTRACED|WCONTINUED);



	}while ((rx_pid == (pid_t)0) && (rx_pid != (pid_t)-1));

//	for (cont = 0; cont < cant_procesos; cont++) {
//
//		if ((hijos+cont)->pid_hijo== rx_pid) {    //es un proceso hijo
//			if((hijos+cont)->tiempo_vida==0){
//
//				printf("(%d) Mi hijo %d murio con dignidad\n",pid_padre,rx_pid);
//				cont_hijos_muertos++;
//			}
//			else{
//				printf("(%d) Mi hijo %d antes de tiempo, le falta vivir %d \n",pid_padre,rx_pid,(hijos+cont)->tiempo_vida);
//				while(*(tab_relanzar+i)!=NULL){ //recorro la tabla hasta la ultima posicion
//					i++;
//				}
//				*(tab_relanzar+i)=(hijos+cont)->tiempo_vida;
//				printf("(%d) info tabla relanzar tiempo de vida: %d\n",pid_padre,*(tab_relanzar+i));
//
//				i=0;
//			}
//
//		}
//	}



}
//
void  sigalrm_handler(){
//
// int cont;
//
//
//	for (cont = 0; cont < cant_procesos; cont++) {
//
//		if (ListaVacia(&lista) ){
//			break;
//		}
//		else{
//			if ((hijos+cont)->tiempo_vida > 0 ) {    //es un proceso hijo
//
//				(hijos+cont)->tiempo_vida=(hijos+cont)->tiempo_vida-1;
//			}
//
//		}
//
//	}
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

	pid_hijo=fork();

	if(pid_hijo==0)   /*codigo del hijo */
	{
		my_pid_hijo=getpid();
		printf("(%d) Soy un nuevo hijo\n", my_pid_hijo);


		usleep(tiempo_vida * 1000); //el timpo de vida enta en ms y usleep usa us
		printf("(%d) Adios mundo cruel!\n", my_pid_hijo);
		exit(1);

	}

	return	pid_hijo;

}
/*funcion encargada de relazar los proceos que se terminaron antes de tiempo
 * devuelve 0 si no se relanzo ningun proceoso o la contidad de proceoso relanzados */
//int relanzar_procesos(){
//
//	int cant_revividos,indice_hijos,i;
//	pid_t pid_hijo_revivido;
//
//
//	do{ 							//recorro la tabla de procesos a relanzar
//		if(*(tab_relanzar+i)==NULL){
//			return 0;					// si la tabla esta vacia salgo del do while y espero la demora
//		}
//		else if(*(tab_relanzar+i)>0){
//
//			pid_hijo_revivido = crear_hijo(*(tab_relanzar+i));
//			printf("(%d) revivi un hijo con pid: %d que le faltaba vivir: %d ms \n",pid_padre,pid_hijo_revivido,*(tab_relanzar+i));
//			*(tab_relanzar+i)=NULL;// elimino al hijo revivido de la tabla a relanzar
//			pid_chld=0;
//			while((hijos+indice_hijos)->pid_hijo!=NULL){
//				indice_hijos++;
//			}
//			(hijos+indice_hijos)->pid_hijo = pid_chld;
//			(hijos+indice_hijos)->demora = t_demora;
//			(hijos+indice_hijos)->tiempo_vida = t_vida;
//			cant_revividos++;
//			i++;
//		}
//
//	}while(*(tab_relanzar+i)!=NULL && pid_hijo_revivido!=0);
//	i=0;
//	return cant_revividos;
//
//}


int main(int argc, char const *argv[]) {

	int cont_hijos = 0,cont,cont_demora=0;
	pid_t pid_hijo_revivido=0;
	pid_t pid_chld;
	Info_hijo nuevo_hijo;

	struct itimerval contador;
	struct timeval tiempoRepeticion;

	pid_padre = getpid(); // pid padre

	//set_t_repeticon(&tiempoRepeticion,0,1000); // creo una señal de alarma cada 1 ms

	tiempoRepeticion.tv_sec=T_SEC_REPETICION;
	tiempoRepeticion.tv_usec=T_USEC_REPETICION;
	contador.it_value=tiempoRepeticion;
	contador.it_interval=tiempoRepeticion;

	int_signals();
	validar_arg(argc,argv);

	cant_procesos = atoi(argv[1]);
	t_vida = atoi(argv[2]);
	t_demora = atoi(argv[3]);

	setitimer (ITIMER_REAL, &contador, 0); // antes de inicial el ciclo de creacion de hijos inicio el contador para que emita la señal alarma cada 1ms

	do {

		pid_chld = crear_hijo(t_vida);

		if (pid_chld != 0) {

			printf("(%d) se creo un nuevo hijo con numero de PID :%d\n", pid_padre,
					pid_chld);

			nuevo_hijo.pid_hijo = pid_chld;
			nuevo_hijo.demora = t_demora;
			nuevo_hijo.tiempo_vida = t_vida;

			Insertar_al_final(&lista,nuevo_hijo);

			cont_hijos++;
			usleep(t_demora * 1000);
			//for (cont_demora = 0; cont_demora < DIV_DEMORA; ++cont_demora) {

				//usleep(t_demora * (1000/DIV_DEMORA)); //divido el tiempo de demora para que el padre pueda analizar la tabla para relanzar procesos

				//pid_hijo_revivido=relanzar_procesos();


			//}

		}


	} while (cont_hijos < cant_procesos && pid_chld != 0);

		if (pid_chld != 0) {

//			while(cont_hijos_muertos < cant_procesos){
//
//				//relanzar_procesos();
//				pause();
//			}
			wait(NULL);

			printf("(%d) Tuve %d hijos, murieron todos, que desgracia!\n", pid_padre,
					cont_hijos);

			printf("Imprimo lista\n");
			MostrarLista(lista);
			BorrarLista(&lista);
			exit(1);


		}





}


